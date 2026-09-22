#include "SDLRayTraceRender.h"
#include "System.h"
#include "SDLInputManager.h"
#include <omp.h>



inline int toRGB888(glm::vec4 colorIn)
{
	int rgbColor = 0;
	//variable en formato RGBA 32bit (4 bytes)
	glm::u8vec4 color = glm::u8vec4(0xFF, 0xFF, 0xFF, 0xFF);

	color = glm::u8vec4((unsigned char)(colorIn.r * 255.0f), (unsigned char)(colorIn.g * 255.0f),
		(unsigned char)(colorIn.b * 255.0f), (unsigned char)(colorIn.a * 255.0f));

	//y retornar el pixel resultado
	rgbColor = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	return rgbColor;
}

SDLRayTrace::SDLRayTrace()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return;
	}
	window = SDL_CreateWindow(
		"RayTracer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		this->w, this->h,
		SDL_WINDOW_SHOWN
	);
	if (window == NULL) {
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return;
	}


	screenSurface = SDL_GetWindowSurface(window);
	std::cout << SDL_GetPixelFormatName(screenSurface->format->format) << "\n";
	std::cout << screenSurface->w << "\n";
	std::cout << screenSurface->h << "\n";
	std::cout << screenSurface->pitch << "\n";


	initRandRays();
}


void SDLRayTrace::initRandRays()
{

	for (float phi = 0.0f; phi < (2.0f * M_PI); phi += sphereRaysStep)
		for (float theta = 0.0f; theta < 2.0f * M_PI; theta += sphereRaysStep)
		{
			sphereRays.push_back(glm::normalize(glm::vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta))));
		}

}



void SDLRayTrace::mapPhotons( Light* l)
{
	int numBounces = 128;
	int globalPhotonCount = 1000*100;
	int causticPhotonCount = 1000*10000;

	//calcular número de fotones por luz
	//la intensidad se debería calcular dependiendo de la luz y de su color:
	//	area* flujo*(sum(color.rgb))

	//El número de fotones debería ser proporcional a la contribución de cada luz
	int numLightPhotons = (float)globalPhotonCount * l->intensity;
	int numCausticPhotons = (float)causticPhotonCount * l->intensity;

	int numThreads = 8;
	std::vector<std::vector<Photon>> photonLists(numThreads);
	std::vector<std::vector<Photon>> causticsLists(numThreads);

#pragma omp parallel for  num_threads(numThreads)
	for (int photonId = 0; photonId < numLightPhotons; photonId++)
	{
		int threadId = omp_get_thread_num();
		glm::vec3 origin = l->pos;
		glm::vec3 randDir = glm::normalize(glm::vec3(((float)(std::rand() % 2000) / 1000.0f) - 1.0f,
			((float)(std::rand() % 2000) / 1000.0f) - 1.0f,
			((float)(std::rand() % 2000) / 1000.0f) - 1.0f));

		//crear un rayo random desde el origen de la luz
		collisionRay_t ray;
		ray.dir = randDir;
		ray.origin = origin + 0.001f * ray.dir;
		ray.inside = false;

		tracePhoton( photonLists[threadId], ray, l->color/3.0f, numBounces, true, false);

	}

#pragma omp parallel for  num_threads(numThreads)
	for (int photonId = 0; photonId < numCausticPhotons; photonId++)
	{
		int threadId = omp_get_thread_num();
		glm::vec3 origin = l->pos;
		glm::vec3 randDir = glm::normalize(glm::vec3(((float)(std::rand() % 2000) / 1000.0f) - 1.0f,
			((float)(std::rand() % 2000) / 1000.0f) - 1.0f,
			((float)(std::rand() % 2000) / 1000.0f) - 1.0f));

		//crear un rayo random desde el origen de la luz
		collisionRay_t ray;
		ray.dir = randDir;
		ray.origin = origin + 0.001f * ray.dir;
		ray.inside = false;

		tracePhoton( causticsLists[threadId], ray, l->color/3.0f, numBounces, true, true);
	

	}
	//iniciar/resetear el grid de fotones
	// parámetros: coordenadas mínimas/máximas del sistema
	//             tamaño de "celda", será la distancia máxima para calcular interacciones de fotones
	photonMap.init(systemSize.minCoords, systemSize.maxCoords, glm::vec3(0.1f), numLightPhotons);
	causticMap.init(systemSize.minCoords, systemSize.maxCoords, glm::vec3(0.1f), numLightPhotons);

	for (auto l : photonLists)
	{
		for (auto p : l)
		{
			photonMap.addObject(p, p.pos);
		}
	}
	photonMap.update();

	bool updateCaustics = false;
	for (auto l : causticsLists)
	{
		for (auto p : l)
		{
			causticMap.addObject(p, p.pos);
			updateCaustics = true;
		}
	}
	//if(causticMap.)
	if(updateCaustics)
		causticMap.update();
}



/// <summary>
/// Método para averiguar si hay objetos entre el punto "p1" y "p2"
/// </summary>
/// <param name="p1">Primer punto desde el que se quiere comprobar visibilidad con el segundo punto</param>
/// <param name="normal">Normal en el primer punto. Si no apunto a "p2" significa que no lo está mirando, no es visible</param>
/// <param name="p2">Segundo punto</param>
/// <returns>True si son visibles, false si hay objetos en medio</returns>
bool SDLRayTrace::isVisible(glm::vec3 p1, glm::vec3 normal, glm::vec3 p2)
{
	bool visible = true;
	glm::vec3 origin = p1;

	glm::vec3 dir = p2 - origin;
	dir = glm::normalize(dir);

	collisionRay_t ray;
	ray.dir = dir;
	ray.origin = origin + 0.001f * glm::normalize(normal);
	ray.inside = false;

	collision_t coll;
	bool collisionDone = false;
	do {
		
		coll= System::getCollisions(ray);
		if ((coll.distance < FLOAT_MAX) && !coll.mesh->collisionEnable)
		{
			ray.origin = glm::vec3(coll.point.pos) + 0.0001f * ray.dir;
		}
		else
			collisionDone = true;
	} while (!collisionDone);

	float distanceLight = glm::length(p2 - origin);

	if (coll.distance < FLOAT_MAX)
	{
		float distance = glm::length(coll.point.pos - glm::vec4(origin, 1.0f));
		//Ha chocado y está entre el objeto y la luz
		//se deja en negro/iluminación ambiental/porcentaje de oscuridad
		if ((distance < distanceLight))
		{
			visible = false;
		}
	}
	return visible;
}





void SDLRayTrace::drawObjects(std::map<float, Object3D*>& objs)
{
	SDLInputManager* keys = (SDLInputManager*)(System::inputManager);
	if (keys->isPressedOnce(SDLK_ESCAPE) || keys->isClosed())
	{
		this->exit = true;
	}
	else {
		SDL_LockSurface(screenSurface);
		//activamos la cámara, iluminación, etc...
	
		//update lights
		for (auto l : lights)
			delete l;
		lights.resize(0);
		auto ITView = glm::inverse(glm::transpose(System::activeViewMatrix));
		for (auto& l : System::lights[System::stepNumber])
		{
		

			Light* l2 = new Light( System::activeViewMatrix * glm::vec4(l->pos, 1.0f),l->type, l->color,l->direction);
			l2->size=l->size;
			l2->linearAttenuation=(l->linearAttenuation);
			l2->intensity=(l->intensity);
			lights.push_back(l2);
		}


		//Variables para eventos de teclado
		//iniciamos el renderizado, nadie más accede a la ventana
		SDL_LockSurface(screenSurface);
		//activamos la cámara, iluminación, etc...
		//calculamos objetos
		int nobjects = (int)objs.size();
		//Array de píxels en formato ARGB 32bit.
		int* pixels = (int*)screenSurface->pixels;
		//seteamos la imagen a blanco
		//memset(pixels, 0xFF, (int)(screenSurface->h * screenSurface->pitch));
		//actualizamos las jerarquías de detección de colisiones
		glm::mat4 model;


		//actualizar el sistema para el mapa de fotones
		systemSize.maxCoords = glm::vec3(FLOAT_MIN);
		systemSize.minCoords = glm::vec3(FLOAT_MAX);

		for (auto& obIt : objs)
		{
			auto ob = obIt.second;
			model=ob->computeModelMatrix();
			int nMesh = (int)ob->meshList.size();
			auto mat = System::activeViewMatrix * model;

			for (auto& mesh : ob->meshList)
			{
				//actualizar transformando el modelo por sus matrices Model y View
				mesh->updateAABB(mat);
				for (auto& v : mesh->getVertList())
				{
					auto vmov = mat * v.pos;
					systemSize.maxCoords.x = std::max(systemSize.maxCoords.x, vmov.x);
					systemSize.maxCoords.y = std::max(systemSize.maxCoords.y, vmov.y);
					systemSize.maxCoords.z = std::max(systemSize.maxCoords.z, vmov.z);

					systemSize.minCoords.x = std::min(systemSize.minCoords.x, vmov.x);
					systemSize.minCoords.y = std::min(systemSize.minCoords.y, vmov.y);
					systemSize.minCoords.z = std::min(systemSize.minCoords.z, vmov.z);
				}

			}
		}

		//mapeo de fotones en el sistema
		std::cout << "Computing Photon Map\n";
		if (enableGlobalLight || enableCaustics)
			for (auto l : lights)
			{
				mapPhotons(l);
			}
		std::cout << "Photon Map done!\n";

		//Datos de la imagen a generar
		float AAscale = enableAntiAliasing ? 2.0 : 1.0;

		int width = screenSurface->w* AAscale;
		int height = screenSurface->h* AAscale;
		float scale = (float)tan(glm::radians(51.52f * 0.5f));
		float imageAspectRatio = width / (float)height;
		pixelsScaled.resize(height * width);
		
		//memset(pixelsScaled.data(), 0, sizeof(glm::vec4)* height*width);

#pragma omp parallel for  collapse(2) num_threads(8) schedule(dynamic)
		for (int i = height-height; i < height; i++) {
			for (int j = 0; j < width ; j++)
			{

				glm::vec4 finalColor = glm::vec4(0.0f);
				int d = 0;
					//calcular un nuevo rayo desde la cámara
				float x = (2 * ((float)j  + 0.5f) / (float)width - 1) * imageAspectRatio * scale;
				float y = (1 - 2 * ((float)i  + 0.5f) / (float)height) * scale;
				glm::vec4 dir = glm::vec4(x, y, -1.0f, 0);
				dir = glm::normalize(dir);
				glm::vec4 orig = System::activeViewMatrix * glm::vec4(System::activeCamera->pos, 1.0);
				//creamos el rayo con los datos
				collisionRay_t ray;
				ray.dir = dir;
				ray.origin = orig;
				ray.inside = false;
				//y lanzamos el rayo
				finalColor += traceRay( ray, 16, true);
				pixelsScaled[i * width + j] = (finalColor);

			}//cada dos "scanlines horizontales", actualizar la ventana 
			if (i % 2)
			{
				SDL_UnlockSurface(screenSurface);
					for (int y = 0; y < screenSurface->h; y++)
						for (int x = 0; x < screenSurface->w; x++)
						{
							if (enableAntiAliasing)//Reescalado usando 4 píxeles
								pixels[y * screenSurface->w + x] = toRGB888(
										(pixelsScaled[2 * y * width  + 2 * x] +
										pixelsScaled[2 * y * width  + (2 * x + 1)] +
										pixelsScaled[(2 * y + 1) * width  + 2 * x] +
										pixelsScaled[(2 * y + 1) * width  + (2 * x + 1)]) / 4.0f);
							else
								pixels[y * screenSurface->w + x] = toRGB888(pixelsScaled[y * width + x]);
						}
				
				SDL_UpdateWindowSurface(window);
				SDL_LockSurface(screenSurface);
			}
		}
		frameCount++;
		saveData(screenSurface->w, screenSurface->h, pixels);
		SDL_UnlockSurface(screenSurface);
		SDL_UpdateWindowSurface(window);
		std::cout << "Frame\n";

	}
}





glm::vec3 SDLRayTrace::getRandDir(glm::vec3 coneDir, float coneAngle)
{


	coneDir = glm::normalize(coneDir);
	glm::vec3 north = glm::normalize(glm::vec3(0, 0, 1));

	//Generate points on the spherical cap around the north pole[1].
	// [1] See https ://math.stackexchange.com/a/205589/81266
	float rand1 = (((float)(std::rand() % 1000) / 1000.0f));
	float rand2 = (((float)(std::rand() % 1000) / 1000.0f));
	float z = rand1 * (1.0f - cos(coneAngle)) + cos(coneAngle);
	float phi = rand2 * 2.0f * (float)M_PI;
	float x = sqrt(1 - z * z) * cos(phi);
	float y = sqrt(1 - z * z) * sin(phi);

	glm::vec3 randRay = glm::normalize(glm::vec3(x, y, z));
	//std::cout << "rr" << randRay.x << " " << randRay.y << " " << randRay.z << "\n";

	float coneRot = glm::angle(north, randRay);// glm::acos(glm::dot(coneDir, north));

	//% If the spherical cap is centered around the north pole, we're done.
	//	if all(coneDir(:) == [0; 0; 1])
	//		r = [x; y; z];
	//return;
	//end

		//% Find the rotation axis `u`and rotation angle `rot`[1]
	if (coneDir != north && -coneDir != north)
	{
		glm::vec3 u = glm::normalize(glm::cross(north, coneDir));
		float rot = glm::angle(coneDir, north);// glm::acos(glm::dot(coneDir, north));
		randRay = glm::rotate(randRay, rot, u);
		randRay = glm::normalize(randRay);
	}

	if (std::isnan(randRay.x))
	{
	//	std::cout << "ERROR";
	}
	else {

		float lastangle = glm::angle(coneDir, randRay);
		if (lastangle > coneAngle)
		{
			//std::cout << "ERROR";
			randRay = -randRay;
			lastangle = glm::angle(coneDir, randRay);
			if (lastangle > coneAngle)
			{
				//std::cout << "ERROR";
			}
		}
	}
	return randRay;

	//end
}





void SDLRayTrace::saveData(int width, int height, int* pixels)
{
	std::ofstream ofs("./out" + std::to_string(frameCount) + ".ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < height * width; ++i) {
		char r = (char)((glm::u8vec4*)(pixels))[i].z;
		char g = (char)((glm::u8vec4*)(pixels))[i].y;
		char b = (char)((glm::u8vec4*)(pixels))[i].x;
		ofs << r << g << b;
	}
	ofs.close();
}
