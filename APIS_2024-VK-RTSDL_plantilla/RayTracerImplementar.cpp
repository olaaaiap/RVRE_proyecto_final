#include "mapi/SDLRayTraceRender.h"
#include "mapi/System.h"
#include "mapi/SDLInputManager.h"



/// <summary>
/// Función a implementar: Cálculo de contribución de un emisor de luz en un punto de un objeto 
/// sobre el que incide un rayo lanzado desde el "visor"
/// </summary>
/// <param name="rayIn">Rayo lanzado desde el "visor" (puede ser la cámara o un rayo rebotado)</param>
/// <param name="coll">Una colisión debida a la intersección de un rayo con un objeto del sistema
/// Representa el punto en sobre el que se desea calcular la luz. Contiene información del material para cálculo de luz</param>
/// <param name="light">Emisor de luz</param>
/// <param name="mat">Material del objeto al que pertenece el punto de colisión (el mismo que el existente en variable "coll")</param>
/// <param name="ambient">Propiedad de intensidad/color de luz ambiente</param>
/// 
/// <returns>Color de iluminación sobre el punto de colisión pasado por parámetros</returns>
glm::vec4 SDLRayTrace::computeLambertLight(collisionRay_t rayIn, collision_t coll, Light* light, Material* mat, glm::vec3 ambient)
{
	float pdf = light->getPdf();//acceso a propiedad de difusión de la luz (1/(PI*Radio))
	glm::vec3 fpos = coll.point.pos; //posición 3D de la colisión (equivalente a la posición de un fragmento en Shaders) 
	float distance = 1.0f; //Variable que se debe actualizar con la distancia enrtre el objeto y la luz
	glm::vec3 lightPos = light->pos;//posición 3D de la luz
	glm::vec3 L = glm::vec3(0.0f);//Variable a actualizar: Vector de dirección de la luz
	glm::vec3 EYE = rayIn.origin; //Posición del "visor", se obtiene del origen del rayo
	glm::vec3 viewDir = glm::normalize( fpos - EYE); //Vector "dirección del visor", cálculo de luz especular
	glm::vec3 N = glm::normalize(coll.point.normal);//Vector normal en el punto de colisión 
	float Id = light->intensity;//Intensidad de la luz, misma para Ia,Id,Is
	glm::vec4 lightColor = (light->color); //Color del emisor de luz
	glm::vec4 finalLight = glm::vec4(0); //Variable resultado del cálculo de la luz


	switch (light->type)//Dependiendo del tipo de luz, actualizar vector "L"
	{
	case Light::LightType::directional:
	{
		//Luz direccional, se puede usar la dirección almacenada en la clase "light" 
		//light->direction 
		L = -light->direction;
	}break;

	case Light::LightType::point:
	{
		//luz puntual, se calcula L con el vector que une la posición de la luz con
		//la posición de colisión "fpos"
		//normalizar al final
		L = lightPos - fpos;
		L = normalize(L);
	}break;
	default:
		break;
	}

	//Cálculo de contribución difusa:
	//difusse=PDF*kd*id*cos( ángulo(NL) )
	float difusse = pdf * mat->kd * Id * (glm::max(glm::dot(N, L), 0.0f));

	//Cálculo de contribución especular:
	//Specular=Ks*Is*cos( ángulo(viewDir Reflejo))
	auto R = reflect(L, N);

	float specular = mat->ks * Id * glm::pow(glm::max(glm::dot(viewDir, R), 0.0f), mat->shinny);

	if (light->linearAttenuation)//si está activa la atenuación lineal, calcular finalLight con atenuación
	{
		//finalLight=...
	}
	else//si no, cálculo básico sin atenuación
	{
		finalLight = (vec4(difusse + specular) + vec4(ambient, 1.0f)) * lightColor;
	}

	return finalLight;
}



/// <summary>
/// Función de calculo de color en un punto del sistema donde ha habido una colisión entre un objeto y un rayo
/// lanzado desde algún "visor"
/// </summary>
/// <param name="objID">Identificador del objeto con el que se trabaja (sólo para debug)</param>
/// <param name="coll">Punto de colisión </param>
/// <param name="mat">Material del objeto con el que colisionó. Propiedades color, textura, Ka, Kd, Ks...</param>
/// <param name="lights">Vector de todas las luces del sistema (sólo contendrá 1 en la práctica)</param>
/// <param name="rayIn">Rayo que colisionó con el objeto. Contiene propiedades de dirección, origen, color...</param>
/// <param name="bounces">Número de rebotes que le quedan por realizar al rayo. Si es 0, no se debe hacer nada </param>
/// <param name="firstBounce">Variable para indicar si es el primer rebote/colisión del rayo. Necesaria para materiales refractantes (cristal)</param>
/// <returns></returns>
glm::vec4 SDLRayTrace::computeColorLight(int objID, collision_t coll, Material* mat, std::vector<Light* > lights, collisionRay_t rayIn, int bounces, bool firstBounce)
{
	//variable color en formato float. Se debe actualizar con los cálculos en este método, y retornar al final 
	glm::vec4 finalColor = vec4(0);
	float shadow = 1.0f; //variable para simular sombras, representa el "porcentaje de luz" que quedará
						// 1 = Totalmente iluminado
						// 0 = Totalmente oscuro.
						// Se debe calcular más adelante, "Parte 3"
	Light l = *lights[0]; //Variable que almacena una copia de la "única" luz que usaremos
	glm::vec3 lightPos = l.pos;//copia de la posición de la luz. Útil para el cálculo de sombras

	//.....TO DO.....//
	//1º Parte: Cálculo de color.
	//acceder al color del punto de colisión, y copiarlo a "finalColor"
	finalColor = coll.point.color;
	if (mat->receiveLight) //si el material admite iluminación
	{

		//2º Parte: Cálculo de luz
		//LLamar a función "computeLambertLight" con los datos de colisión, luz, etc...
		//Eso calcula la luz en el punto actual, mezclarla con la variable "finalColor"
		//finalColor= .... computeLambertLight ....
		/// finalColor *= computeLambertLight(rayIn, coll, &l, mat, System::ambient); (la hemos movido al final para que le afecte el shadow)

		if (enableShadows) //Si está activo el cálculo de sombras
		{
			//3º Parte: Sombras
			//actualizar la variable "shadow" en función del porcentaje de iluminación


			//Opción 1: Sombras duras
				//Usar el método "isVisible(posición1,normal en posición 1,posición2)
				//El método "isVisible" devuelve true o false en función de si el punto en "posición2"
				//es visible desde posición2 (no hay obstáculos)
				
				//Si la posición del punto de colisión  (coll.point.pos) no ve el punto en la posición de la luz
				// shadow=0 (sombra)
				//Si no,
				//	shadow=1 (iluminado)
			/*if (isVisible(coll.point.pos, coll.point.normal, l.pos)) {
				shadow = 1.0f;
			}
			else {
				shadow = 0.0f;
			}*/

			//Opción 2: Sombras difuminadas
				//Se comprueba la visibilidad a lo largo de un área de luz:
					//La luz tiene un tamaño en x,y,z= light->size();
					//Escoger varios puntos a lo largo de la superficie de la luz, y comprobar visibilidad con cada uno de ellos
					//La variable "shadow" será la media de los puntos visibles entre el total de puntos:
					// Ej: Si de 6 puntos, sólo 3 son visibles, el objeto está al 50% de sombra (shadow=0.5)


			shadow = 0;
			int counter = 0;
			int samples = 10;
			float rango = 0.2f;
			float step = 0.01f;

			for (float i = -rango; i <= rango; i += step) {
				for (float j = -rango; j <= rango; j += step) {
					counter++;
					glm::vec3 puntoLight = lightPos + glm::vec3(i, j, 0.0f);
					if (isVisible(coll.point.pos, coll.point.normal, puntoLight)) {
						shadow += 1;
					}
				}
			}
			shadow = shadow / (float)counter;
			

			//Opción 3: Sombras difuminadas+área luz:
				//Lo anterior presupone que la luz sigue siendo un punto, a pesar de tener un tamaño
				//Si cada punto de luz elegido para cálculo de sombra emite un rayo de luz con un vector de incidencia distinto
				//se pueden acumular todas esas contribuciones y calcular la media entre el número de puntos visibles.
		

			//Una vez calculado el porcentaje de iluminación-sombra, actualizar finalColor
			//finalColor=...shadow...averageLight...
		}
		finalColor *= shadow*computeLambertLight(rayIn, coll, &l, mat, System::ambient);
		
	}

	//5º Parte: Global light (implementar primero reflexión y refracción)
	// Cálculo a través de mapeo de fotones (Photon Map)
	// Más info:
	//https://github.com/ReillyBova/Global-Illumination#radiance-sampling

	if (enableGlobalLight && (mat->receiveLight) && (bounces > 0))
	{

		//iluminación global
		int samples = 200;//Número de fotones de iluminación global más cercanos al punto de colisión (Más fotones=más iluminación)
		float radious = 1.6;//Radio de acción para buscar fotones (más grande= más lento, mejor media)
		int causticSamples = 400; //Número de fotones de "cáusticas" (efecto lupa) 
		float causticRadious = 0.1f;//Radio de acción a buscar fotones de cáusticas

		std::vector<Photon> photons = photonMap.getNNearestObjects(coll.point.pos, samples, radious); //obtener "N" fotones en un radio "R"
		std::vector<Photon> causticPhotons = causticMap.getNNearestObjects(coll.point.pos, causticSamples, causticRadious);

		glm::vec4 globalColor(0.0f);//almacena el color final de iluminación global
	

		for (auto& p : photons)//por cada fotón

		{
			if (isVisible(coll.point.pos, coll.point.normal, p.pos))//si es visible desde el punto de colisión
			{
				//Obtener la contribución difusa (opcionalmente la especular) de luz de ese fotón en el punto de colisión
				//Usar la dirección del fotón como vector de dirección de luz:  "p.dir"
				//Simular "degradado" de intensidad de luz del fotón: "0.01 de intensidad"
				//Mezclar con el color del objeto: Sumar colores
				//Acumular esa contribución en "globalColor"

			}
		}

		if (enableCaustics)
			for (auto& p : causticPhotons)
			{

				if (isVisible(coll.point.pos, coll.point.normal, p.pos))
				{
					//Obtener la contribución difusa (opcionalmente la especular) de luz de ese fotón en el punto de colisión
					//Usar la dirección del fotón como vector de dirección de luz:  "p.dir"
					//Simular "degradado" de intensidad de luz del fotón: "0.01 de intensidad"
					//Mezclar con el color del objeto: Sumar colores
					//Acumular esa contribución en "globalColor"
				}
			}

		if (photons.size() > 0)//si había fotones
		{
			//multiplicar por pdf de luz
			//añadir a finalColor
		}


	}

	//4º Parte: Cálculo de reflexión
	if (enableReflection)//si está activo el cálculo
	{
		if (mat->reflectionEnable)//y el objeto es reflectante
		{
			glm::vec4 bouncedColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);//variable que acumula los colores de rebotes de rayos
			
			
			//podemos usar "número de rebotes máximo"
			if (bounces > 0) //si aún quedan rebotes por realizar
			{
			//apartir del ray in vamos a calcular un rayo rebotado
				//si hay rebotes
				//Crear un rayo "bouncedRay" con la dirección reflejada (glm::reflect)
				collisionRay_t bouncedRay;
				//Asignar el origen del rayo (bouncedRay.origin) usando el punto de colisión. Moverlo 0.001 unidades 
				//a lo largo de la dirección del rayo para que no colisione consigo mismo
				//
				bouncedRay.dir = glm::reflect(rayIn.dir, vec3(coll.point.normal));
				bouncedRay.origin=coll.point.pos+coll.point.normal*0.001f;
				bouncedRay.inside = rayIn.inside; //el rayo puede rebotar tambien internamente

				// lanzar el rayo, y conseguir la contribución de color reflejado. Usar la función "traceRay":
				// 
				bouncedColor = traceRay(bouncedRay, bounces - 1, false);
			}
			//Mezclar con color actual
			//Crear una función de mezcla de color usando como referencia la propiedad de pulido "shinny" del material
			//Valor máximo de mat->shinny = 100
			//opcion sin shinny: porcentaje fijo
			//finalColor = bouncedColor * 0.75f + finalColor * 0.25f;
			float factor = mat->shinny / 100.0f; // normalizado 0..1
			finalColor = bouncedColor * factor + finalColor * (1.0f - factor);
			
			//Añadir información especular:
				//Al mezclar con colores rebotados, se pierde información de luz especular
				//Si es el primer rebote (rebote superficial)
				//Calcular contribución especular
			//utilizar isVisible y el calculo anterior de specular 
			if (firstBounce) {
				//Calcular contribución especular
				glm::vec3 N = glm::normalize(glm::vec3(coll.point.normal));
				glm::vec3 fpos = glm::vec3(coll.point.pos);
				glm::vec3 L = glm::normalize(glm::vec3(l.pos) - fpos);
				glm::vec3 viewDir = glm::normalize(fpos - glm::vec3(rayIn.origin)); 
				glm::vec3 R = glm::reflect(L, N); 
				float specular = mat->ks * l.intensity * glm::pow(glm::max(glm::dot(viewDir, R), 0.0f), mat->shinny);

				//finalColor += shadow * specular * l.color;
				finalColor +=  specular * l.color;
			}
		}
	}


	//4º Parte: Refracción
	//Calcular la refracción, igual que con reflexsión, dependerá del número de rebotes sobrante y de la distancia recorrida
	if (enableRefraction)//si está habilitado el cálculo
	{
		if (mat->refractionEnable)//y el objeto es refractante
		{
			glm::vec4 bouncedColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			//si quedan rebotes 
			if (bounces > 0)
			{
				collisionRay_t bouncedRay;
				//crear un nuevo rayo, con la dirección refractada
				//Puede estar entrando o saliendo del objeto: Actualizar "bouncedRay" en fución de propiedad "inside" del rayo "rayIn"
			
				//ojo, no es lo mismo entrar al objeto que salir
				//si el rayo entra, el coeficiente de refracción es IR_Aire/IR_Objeto
				//si el rayo sale,  el coeficiente de refracción es IR_Objeto/IR_Aire
				//IR_Aire=1.0f , IR_Objeto=mat->refractIndex
				// 
				float IR_Aire = 1.0f;
				float IR_Objeto = mat->refractIndex;
				glm::vec3 N = glm::normalize(glm::vec3(coll.point.normal));
				float coeficiente;
				if (!rayIn.inside)
				{
					coeficiente = IR_Aire / IR_Objeto;
				}
				else
				{
					coeficiente = IR_Objeto / IR_Aire;
					N = -N; //cuando el rayo sale del objeto, hay que invertir la normal
				}
				glm::vec3 dirRefraccion = glm::refract(glm::vec3(rayIn.dir), N, coeficiente);
				if (glm::length(dirRefraccion) < 0.0001f) //rebote interno, no hay refraccion
				{
					dirRefraccion = glm::reflect(glm::vec3(rayIn.dir), N);
				}

				//asignar el origen del rayo, movido a lo largo de la dirección del mismo (evitar que choque contra la misma superficie)
				//bouncedRay.origin = ...
				//trazar el rayo para conseguir el resultado
				//bouncedColor = traceRay(.....)
				bouncedRay.dir = dirRefraccion;
				bouncedRay.origin = glm::vec3(coll.point.pos) + dirRefraccion * 0.001f;
				bouncedRay.inside = !rayIn.inside;

				bouncedColor = traceRay(bouncedRay, bounces - 1, false);

				//Mezclar color refractado con finalColor
					//ecuación mezcla de color. Igual que en reflexión, se pueden usar propiedades "shinny" o un porcentaje de mezcla fijo
				
				/*float factor = mat->shinny / 100.0f;
				finalColor = bouncedColor * factor + finalColor * (1.0f - factor);*/
				float factor = 0.95f; //prueba para que se parezca más al render del enunciado
				finalColor = bouncedColor * factor + finalColor * (1.0f - factor);
				if (firstBounce) {
					
					//Añadir información especular al objeto
					glm::vec3 fpos = glm::vec3(coll.point.pos);
					glm::vec3 L = glm::normalize(glm::vec3(l.pos) - fpos);
					glm::vec3 viewDir = glm::normalize(fpos - glm::vec3(rayIn.origin)); 
					glm::vec3 R = glm::reflect(L, N); 
					float specular = mat->ks * l.intensity * glm::pow(glm::max(glm::dot(viewDir, R), 0.0f), mat->shinny);
				
					finalColor +=  specular * l.color;
				}
			}
		}
	}

	//FIN, actualizar color evitando que salga de márgenes
	//asegurar que los colores están entre 0 y 1
	finalColor = glm::clamp(finalColor, 0.0f, 1.0f);
	//retornar color
	finalColor *= shadow;
	return finalColor;

}



/// <summary>
// Trazador de fotones. Dado un rayo de entrada emitido por una luz y su color
// rellenará la lista con "fotones", que representan cada uno de los puntos donde el rayo
// ha chocado y generado un punto de luz
// Es una función recursiva, cada rebote genera un rayo nuevo que puede seguir chocando en el escenario.
// La función acaba cuando no hay más rebotes (bounces==0) ó cuando no hay colisiones entre el rayo y el escenario.
/// </summary>

/// <param name="photonList">Lista de fotones generados por los rebotes del rayo trazado</param>
/// <param name="rayIn">Rayo de entrada que hay que testear. Genera al menos un nuevo fotón por cada rebote. Puede ser reflejado o refractado según las propiedades de los objetos</param>
/// <param name="rayColor">Color de la luz que generó el rayo que se está trazando</param>
/// <param name="bounces">Número de rebotes que quedan por realizar. Si es "0", acaba la función</param>
/// <param name="firstBounce">Parámetro para averiguar si es el primer rebote (importante para reflejos por "cáusticas")</param>
/// <param name="caustic">Parámetro para activar simulación de fotones realizados por cáusticas</param>

void SDLRayTrace::tracePhoton(std::vector<Photon>& photonList,
	collisionRay_t rayIn, glm::vec4 rayColor,
	int bounces, bool firstBounce, bool caustic)
{
	if (bounces > 0)
	{
		// Testear si el rayo choca con el sistema (método "System::getCollisions(ray)")
		collision_t coll;
		collisionRay_t newRay = rayIn;
		do {
			coll = System::getCollisions(newRay); //Obtener colisiones del rayo con el sistema. Devuelve la más cercana 
			
			if (coll.distance < FLOAT_MAX && !coll.mesh->collisionEnable)
			{
				newRay.origin = vec3(coll.point.pos) + 0.001f * newRay.dir;
			}
		} while (coll.distance < FLOAT_MAX && !coll.mesh->collisionEnable);//repetir mientras no choque

		Material* mat = coll.mat;//obtener el material

		if (coll.distance < FLOAT_MAX && !(firstBounce && (glm::dot(glm::vec3(coll.point.normal), -rayIn.dir) < 0)))
		{
			//si choca contra un elemento y la normal apunta a esa luz:
			// - crear un fotón (Estructura "Photon")
			// - con el color del rayo (Photon.color)
			// - la posición del fotón es la del punto de colisión (Photon.pos)
			// - la dirección del fotón es la dirección del rayo de entrada (Photon.dir)
			// - añadir a la lista de fotones 
			Photon photon;
			//photon.color Actualizar color con el color del rayo de entrada//
			//photon.pos Actualizar la posición del fotón con la posición de la colisión. Moverlo 0.0001 unidades a lo largo de la normal
			//photon.dir Actualizar la dirección del fotón con la dirección del rayo de entrada

			//Si no es fotón de cáusticas, se añade a la lista de fotones: photonList.pushBack (....)
			//else
			//	Si es fotón generado por cáusticas, se añade __si_no__ es el primer rebote

			//TODO: Photon tracing:
			//Dependiendo de las propiedades del material, tenemos cuatro posibilidades de rebote:
			// - Absorbido: Termina (no rebota)
			// - Reflejado: 
			//		- Por propiedad difusa
			//		- Por propiedad especular
			// - Refractado :
			//		- Por índice de refracción
			// 
			//Obtener las probabilidades de cada propiedad, y decidir por montecarlo (random en una distribución de probabilidades):
			// 
			//	- El material tiene colores dependiendo de cada componente luz:
			//		Colores difuso y especular para propiedades "reflectivas"
			//		Color de refracción para propiedades "refractivas"		
			//		Cada uno de ellos se "mezclará" con el color del fotón según atraviese la superficie
			//		***	Por simplificar, todos usan el mismo color: coll.point.color 
			//		*** si el material no es refractable, su probabilidad de refracción es "0" : mat->getRefractionEnable()
			//		*** si el material tiene reflejo, su probabilidad especular es 1 (100% de reflejo) :  mat->getReflectionEnable()
			// 
			//	- Cada color indica la intensidad esa propiedad, por lo que podemos crear probabilidades
			//	  de que ocurran esos rebotes a partir de sus colores reflejados
			// 
			//	- Usaremos la componente del color más alta para cálculo de probabilidades:
			//			*** Sólo componentes RGB ***
			//			glm::compMax(glm::vec3 color)
			// 
			//  - Para cada tipo de propiedad, encontrar su probabilidad (PDF):
			//			Calcular color reflejado: Color de material * color de luz
			//			
			//                mat->getPDF(color reflejado,absorbed, reflectedDiffuse,reflectedSpecular,refracted);
			//			
			//		Propiedades: 
			//			Se debe ver como una distribución en una línea de probabilidades, van en orden
			//	
			//				| % absorbed   |   % difuse   |    % specular | % refract |         
			//              0 ........................................................1
			//  		
			//               absorbed + reflectedDiffuse + reflectedSpecular + refracted= 1.0f
			//				
			//	- Calcular un número random entre 0.0 y 1.0: float random=(float)(std::rand()%1000)/1000.0f;
			//			El fotón continúa si el número random cae en alguna de las secciones de 
			//          rebote: difusa, especular o refracción
			//			- (rand<absorbed) -> absorbido (termina)
			//			- (rand<(absorbed + reflectedDiffuse)) -> rebote difuso
			//			- (rand<(absorbed + reflectedDiffuse + reflectedSpecular)) -> rebote especular
			//			- (rand<(absorbed + reflectedDiffuse + reflectedSpecular + refract)) -> rebote refractado


			float absorbed = 0.0f;
			float reflectedDiffuse = 0.0f;
			float reflectedSpecular = 0.0f;
			float refracted = 0.0f;


			collisionRay_t newRay;//nuevo rayo a calcular
			//crear un nuevo color para un nuevo rayo/fotón, mezclando con el color del objeto y el rayo original
			glm::vec4 newColor = (coll.mat->reflectionEnable || mat->refractionEnable) ? (rayColor) : (rayColor * coll.point.color);

			//Actualizar su dirección, origen y propiedad "inside" en función del tipo de rebote

			mat->getPDF(absorbed, reflectedDiffuse, reflectedSpecular, refracted);
			float random = (float)(std::rand() % 1000) / 1000.0f;

			//Trazar un nuevo fotón en función de las estadísticas
			//Si no absorbido
			//	Si rebote difuso:
			//		Los rebotes por componente difusa no son perfectos, rebotará en un ángulo random, menor de 90º respecto de la normal
			//			usar getRandDir para conseguir un rayo random
			//		¡Las cáusticas no continúan si hay rebote difuso!
			//		
			//		crear un rayo nuevo: 
			//			Dirección: "random", 90º sobre la normal.
			//						Tip opcional: Usar la propiedad shinniness para calcular un porcentaje ángulo proporcional a su rugosidad
			//							          Valor máximo de Shinniness: 128. Ese valor significa "máximo de pulido" (elegido por nosotros). En ese caso
			//			Posición: la posición de la colisión movida 0.0001 unidades sobre la dirección de rebote
			//			Propiedad: "inside", la misma del rayo original
			// 
			//	else Si rebote especular:
			//		 Rebote perfecto respecto de la normal
			//			usar glm::reflection
			//		 crear un rayo nuevo: 
			//			Dirección reflejada
			//			Posición, la posición de la colisión movida 0.0001 unidades sobre la dirección de rebote
			//			Propiedad "inside", la misma del rayo original
			//  else Si rebote refractado:
			//		 Calcular dirección rebote refractado			
			//			usar glm::refract
			//		 crear un rayo nuevo: 
			//			Dirección refractada
			//			Posición, la posición de la colisión movida 0.0001 unidades sobre la dirección de rebote
			//			Propiedad "inside", ******la negada del rayo original******
			//	
			// trazar el fotón con el nuevo rayo, el color modificado, y el número de rebotes restado una unidad
			//

			if (random > absorbed)
			{
				//rebote difuso
				if (random < (absorbed + reflectedDiffuse))
				{
					//si no es un fotón de cáusticas
					if (!caustic)
					{
						//crear un nuevo rayo rebotado "newRay"
						//rebote en función de shinny, los muy pulidos hacen un rebote cercano a 0

						//Obtener un rayo "random" dentro de un ángulo de apertura desde una normal dada:
						//usar método getRandDir(coll.point.normal, angle);
						
						//llamar recursivamente a trazar fotones
						//tracePhoton(photonList, newRay, newColor, bounces - 1, false, caustic);
					}
				}
				else if (random < (absorbed + reflectedDiffuse + reflectedSpecular))
				{//rebote especular
					//si no es un fotón de cáusticas
					if (!caustic)
					{
						//rebote perfecto
					
						//llamar recursivamente a trazar fotones
						//tracePhoton(photonList, newRay, newColor, bounces - 1, false, caustic);
					}
				}
				else if (random < (absorbed + reflectedDiffuse + reflectedSpecular + refracted))
				{//rebote refractado

					//cálculo de dirección refractada, en función del índice de refracción y si el rayo está dentro.
				
					//trazar nuevo fotón
					//tracePhoton(photonList, newRay, newColor, bounces - 1, false, caustic);
				}
			}
		}
	}


}


//Función encargada de detectar rayos que colisionan contra los objetos. Por cada colisión encontrada, calcula
//la iluminación/color en ese punto y devuelve el resultado
//
glm::vec4 SDLRayTrace::traceRay( collisionRay_t ray, int bounces, bool firstBounce)
{
	glm::vec4 colorValue(0, 0, 0, 0);//color de fondo blanco "clearColor" 
	//por cada objeto, y mientras no haya encontrado colisiones
	//std::map<float, collision_t> arrayCollsFinal;
	collision_t collision;
	collision = System::getCollisions(ray);
	
	//si hay alguna colisión, nos quedamos con las de la malla más cercana
	if (collision.distance < FLOAT_MAX)
	{
		//la primera posición es la más cercana

		//podemos calcular el color e iluminación en ese punto
		colorValue = computeColorLight( 0, collision, collision.mat, lights, ray, bounces, firstBounce);
		//colorValue = collision.point.color;

	}

	return colorValue;
}