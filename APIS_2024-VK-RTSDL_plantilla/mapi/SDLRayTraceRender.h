#pragma once
#include "Render.h"

#include <SDL.h>
#include "CellGrid.h"

#define PI          3.14159265358979323846

class SDLRayTrace:public Render
{
private :

	struct {
		glm::vec3 minCoords;
		glm::vec3 maxCoords;
	}systemSize;
	/// <summary>
	/// Estructura que almacena propiedades de un fotón
	/// </summary>
	typedef struct Photon {
		glm::vec4 color; //Color
		glm::vec3 pos; //Posición
		glm::vec3 dir; //Dirección
	}Photon;

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	vector<glm::vec4> pixelsScaled;
	bool exit = false;
	std::vector<Light*> lights;
	int frameCount;
	CellGrid<Photon> photonMap; //mapa de fotones normales precomputados por la función "mapPhotons"
	CellGrid<Photon> causticMap; //mapa de fotones de tipo cáusticas precomputados por la función "mapPhotons"

	float sphereRaysStep =(float)(2.0f*M_PI/360.0f);//Usado para precómputo de rayos random. Cuanto más pequeño, más rayos
	std::vector<glm::vec3> sphereRays; //Array de vectores inscritos en una esfera de radio 1 y que pasan por su centro 
								   //precómputo de rayos random normalizados


public:


	 SDLRayTrace();
	 void setupObject(Object3D* obj){};
	 void drawObject(Object3D* obj){};
	 bool isClosed() { return exit; };
	 void swapBuffers(){};
	 void setupFrameBuffer(){};
	 Texture* getBuffer(std::string bufferName) { return nullptr; };
	 void setOutBuffer(std::string type, std::string bufferName){};
	 void drawObjects(std::map<float, Object3D*>& objs);

	 void initRandRays() ;
	 void mapPhotons(Light* l);
	 void tracePhoton(std::vector<Photon>& photonList,
		 collisionRay_t rayIn, glm::vec4 rayColor,
		 int bounces, bool firstBounce, bool caustic);

	 glm::vec4 traceRay( collisionRay_t ray, int bounces, bool firstBounce);
	 glm::vec4 computeColorLight(int objID, collision_t coll, Material* mat, std::vector<Light* > lights, collisionRay_t rayIn, int bounces, bool firstBounce);

	 void saveData(int width, int height, int* pixels);
	 bool isVisible(glm::vec3 p1, glm::vec3 normal, glm::vec3 p2);
	 glm::vec4 computeLambertLight(collisionRay_t rayIn, collision_t coll, Light* light, Material* mat, glm::vec3 ambient);

	 glm::vec3 getRandDir(glm::vec3 coneDir, float coneAngle);
};

