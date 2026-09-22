#include "mapi/common.h"
#include "mapi/System.h"

 void asianTown() {

	Skybox* sky = new Skybox();
	Generic3D* atown = new Generic3D("data/asian_town/asian_town.msh");
	atown->size = { 30,30,30 };
	atown->pos = { 0,-1,0 };

	System::addObject(0, atown);
	System::addObject(0, sky);

	System::addCamera(0, new CameraFPS({ 0,0,3 }, { 0,0,0 }));
	System::addLight(0, new Light({ 3,5,3 }));
}

 void asianTownShadowMap() {

	Skybox* sky = new Skybox();
	Generic3D* atown = new Generic3D("data/asian_town/asian_town.msh");
	atown->size = { 30,30,30 };
	atown->pos = { 0,-1,0 };

	if (FactoryEngine::getSelectedGraphicsBackend() == backend_e::GL4)
	{
		System::readPipeLine("data/shadowMap.pipeline");

		System::addObject(0, sky);
		System::addObject(0, atown);

		System::addObject(1, sky);
		System::addObject(1, atown);

		System::addCamera(0, new CameraStatic({ 30,50,60 }, Camera::CameraType::ortho, { 0,0,0 }));
		System::addLight(0, new Light({ 30,50,60 }));

		System::addCamera(1, new CameraFPS({ 0,0,3 }, { 0,0,0 }));

	}

	if (FactoryEngine::getSelectedGraphicsBackend() == backend_e::RT_SDL)
	{
		System::addObject(0, sky);
		System::addObject(0, atown);
		System::addLight(0, new Light({ 30,50,60 }));
		System::addCamera(0, new CameraFPS({ 0,0,3 }, { 0,0,0 }));
	}
}

 void fengShui() {

	Generic3D* drg = new Generic3D("data/feng_shui/Feng_Shui.msh");
	drg->size = { 0.05f,0.05f,0.05f };

	System::addObject(0, drg);
	glm::vec3 dir = glm::normalize(glm::vec3(3.86f, 2.77f, 3.46f));
	glm::vec3 pos = 15.0f * dir;
	System::addCamera(0, new CameraFPS(pos, { 0,0,0 }));
	System::addLight(0, new Light({ 3,5,3 }));
}

 void skybox1GL() {
	Skybox* sky = new Skybox();
	Generic3D* teapot = new Generic3D("data/teapot/metalTeapot.msh");
	Generic3D* suzanne = new Generic3D("data/suzanne/glassSuzanne.msh");
	Generic3D* cubeNormal = new Generic3D("data/skybox1/normalMapCubeObj.msh");

	teapot->pos = glm::vec3(0, 0.0f, -3.0f);
	suzanne->pos = glm::vec3(-1.51f, 0.0f, 0.0f);
	cubeNormal->pos = glm::vec3(-3.0f, 0.0f, 0.0f);

	System::addObject(0, sky);
	System::addObject(0, teapot);
	System::addObject(0, suzanne);
	System::addObject(0, cubeNormal);

	System::addCamera(0, new CameraFPS({ 0,0,3 }, { 0,0,0 }));
	System::addLight(0, new Light({ 3,5,3 }));
}




 void shadowMap() {
	System::readPipeLine("data/shadowMap.pipeline");

	Skybox* sky = new Skybox();

	Generic3D* scene = new Generic3D("data/shadowmap/scene.msh");
	scene->pos = glm::vec3(0.0f, -2.0f, 0.0f);
	scene->size = glm::vec3(5);

	System::addObject(0, sky);
	System::addObject(0, scene);

	System::addObject(1, sky);
	System::addObject(1, scene);

	System::addCamera(0, new CameraStatic({ 3,5,3 }, Camera::CameraType::ortho, { 0,0,0 }));
	System::addCamera(1, new CameraFPS({ 0,0,3 }, { 0,0,0 }));

	System::addLight(0, new Light({ 3,5,3 }));
}

 void VulkanDemo()
{

	Generic3D* plane2D = new Generic3D("data/vkPlane2D.msh");
	plane2D->pos = glm::vec3(0.0f, 0.0f, 0.0f);

	System::addObject(0, plane2D);

	System::addCamera(0, new CameraFPS({ 0,0,3 }, { 0,0,0 }));
	//System::addCamera(0, new Camera({ 0,0,3 },Camera::perspective, { 0,0,0 }));
	System::addLight(0, new Light({ 3,5,3 }));

}

 void particles() {

	Generic3D* column = new Generic3D("data/column/column.msh");
	column->size = { 0.005f,0.005f, 0.005f };

	System::addObject(0, column);
	System::addEmitter(0, new Emitter({ 0,3,0 }, "data/column/flame.msh",
		{ -0.5f,0.5,-0.5 }, { 0.5f,0.5,0.5 }, //direcciones
		2.0f, 5.0f,//velocidades
		2, 5,//tiempos de vida
		5
	));
	System::addEmitter(0, new Emitter({ 0,3,0 }, "data/column/smoke.msh",
		{ -0.5f,0.5,-0.5 }, { 0.5f,0.5,0.5 }, //direcciones
		2.0f, 5.0f,//velocidades
		2, 5,//tiempos de vida
		5
	));
	System::addCamera(0, new CameraFPS({ 0,0,3 }, { 0,0,0 }));
	System::addLight(0, new Light({ 3,5,3 }));

}

 void animation() {
	Generic3D* dwarf = new Generic3D("data/dwarf/dwarf.msh");
	dwarf->size = glm::vec3(0.05f);
	dwarf->pos = glm::vec3(0.0f, 0.0f, 0.0f);
	dwarf->rot = glm::vec3(0.0f, 0.0f, 0.0f);
	System::addObject(0, dwarf);

	System::addCamera(0, new CameraFPS({ 0,2,6 }, { 0,2,0 }));
	System::addLight(0, new Light({ 3,5,3 }));

}

 void skybox1RT() {
	 System::render->w = 720;
	 System::render->h = 480;
	 System::render->enableGlobalLight = false;
	 System::render->enableCaustics = false;
	 System::render->enableAntiAliasing = true;
	 System::render->enableReflection = true;
	 System::render->enableRefraction = true;

	 float scale = 1.0f;

	 Skybox* sky = new Skybox();
	 Generic3D* teapot = new Generic3D("data/teapot/metalTeapot.msh");
	 Generic3D* suzanne = new Generic3D("data/suzanne/glassSuzanne.msh");
	 Generic3D* texturedCube = new Generic3D("data/normalMap/normalMapCubeObj.msh");

	 teapot->pos = glm::vec3(0, 0.0f, -3.0f)*scale;
	 teapot->size *= scale;
	 suzanne->pos = glm::vec3(1, 0, -1)* scale;
	 suzanne->size *= scale;
	 texturedCube->pos = glm::vec3(0, 0, 5)* scale;
	 texturedCube->pos *= scale;


	 System::addObject(0, sky);
	 System::addObject(0, teapot);
	 System::addObject(0, suzanne);
	 System::addObject(0, texturedCube);

	 System::addCamera(0, new CameraFPS(glm::vec3(0,0,3 ) * scale, glm::vec3( 0,0,0 ) * scale));
	 Light* sun = new Light(vec3(1, 0, 2) * scale, Light::point, glm::vec4(0.9, 0.9, 0.5, 1.0f));
	 sun->intensity = 1.0f;
	 System::addLight(0, sun);


 }
 void cornellBox2()
{
	System::render->w = 720;
	System::render->h = 480;
	System::render->enableGlobalLight = true;
	System::render->enableCaustics = true;
	System::render->enableAntiAliasing = true;
	System::render->enableShadows = true;

	float scale = 1.0f;
	Generic3D* aCBox = new Generic3D("data/cornellBox/connellBoxRT.msh");
	aCBox->pos *= scale;
	aCBox->size *= scale;

	System::addObject(0, aCBox);

	Generic3D* ateapot = new Generic3D("data/teapot/glassTeapot.msh");

	ateapot->size = (glm::vec3(0.5, 0.5, 0.5) * scale);
	ateapot->pos = (glm::vec3(0.0, -0.5, 0) * scale);
	System::addObject(0, ateapot);

	//Punto de luz de la escena
	Light* sun = new Light(vec3(0, 1.1f, 0.0f) * scale, Light::point, glm::vec4(0.9, 0.9, 0.8, 1.0f));
	sun->size = (glm::vec3(0.2f, 0.001f, 0.2f) * scale);
	sun->linearAttenuation = (true);
	sun->intensity = 0.75f;

	System::addCamera(0, new CameraFPS(vec3(0, 0, 3) * scale, { 0,0,0 }));
	System::addLight(0, sun);
}

 void cornellBox1()
{

	System::render->w = 720;
	System::render->h = 480;
	System::render->enableGlobalLight = true;
	System::render->enableCaustics = true;
	System::render->enableAntiAliasing = false;
	System::render->enableShadows = true;

	float scale = 2.0f;
	Generic3D* aCBox = new Generic3D("data/cornellBox/connellBoxRT.msh");
	aCBox->pos *= scale;
	aCBox->size *= scale;

	System::addObject(0, aCBox);
	
	Generic3D* sphere1 = new Generic3D("data/sphere/metalSphere.msh");
	sphere1->size = (glm::vec3(0.5, 0.5, 0.5) * scale);
	sphere1->pos = (glm::vec3(-0.5, -1.0f, 0.0) * scale);

	System::addObject(0, sphere1);

	Generic3D* sphere2 = new Generic3D("data/sphere/glassSphere.msh");
	sphere2->size = (glm::vec3(0.25, 0.25, 0.25) * scale);
	sphere2->pos = (glm::vec3(0.75f, -1.0f, 0.0) * scale);
	System::addObject(0, sphere2);

	//Punto de luz de la escena
	Light* sun = new Light(vec3(0, 1.1f, 0.0f) * scale, Light::point, glm::vec4(0.9, 0.9, 0.8, 1.0f));
	sun->size = (glm::vec3(0.1f, 0.001f, 0.1f) * scale);
	sun->linearAttenuation = (true);
	sun->intensity = 0.75f;

	System::addCamera(0, new CameraFPS(vec3(0, 0, 3) * scale, { 0,0,0 }));
	System::addLight(0, sun);
}


 void cornellBox3()
 {
	System::render->w = 720;
	System::render->h = 480;
	System::render->enableGlobalLight = true;
	System::render->enableCaustics = true;
	System::render->enableAntiAliasing = false;
	System::render->enableShadows = true;

	 float scale = 2.0f;
	 Generic3D* aCBox = new Generic3D("data/cornellBox/connellBoxRT.msh");
	 aCBox->pos *= scale;
	 aCBox->size *= scale;
	 System::addObject(0, aCBox);

	 Generic3D* abunny = new Generic3D("data/bunny/glassBunny.msh");

	 abunny->size = (glm::vec3(0.25, 0.25, 0.25) * scale);
	 abunny->pos = (glm::vec3(0.0, -0.7, 0) * scale);
	 System::addObject(0, abunny);

	 //Punto de luz de la escena
	 Light* sun = new Light(vec3(0, 0.50f, 1.0f) * scale, Light::point, glm::vec4(0.9, 0.9, 0.8, 1.0f));
	 sun->size = (glm::vec3(0.2f, 0.2f, 0.2f) * scale);
	 sun->linearAttenuation = (true);
	 sun->intensity = 0.75f;

	 System::addCamera(0, new CameraFPS(vec3(0, 0, 3) * scale, { 0,0,0 }));
	 System::addLight(0, sun);
 }