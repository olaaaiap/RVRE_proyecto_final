#pragma once
#include "common.h"
#include "Camera.h"
#include "Light.h"
#include "Emitter.h"
#include "Render.h"
#include "InputManager.h"
#include "FactoryEngine.h"

class pipelineNode {
public:
	int nodeIdx = 0;
	//<tipo, nombre>
	std::map<std::string, std::string>input;
	//<tipo, nombre>
	std::map<std::string, std::string>output;
};



class System
{
public:
	static inline glm::vec3 ambient = glm::vec3(0.4f);
    static inline Render* render = nullptr;
	static inline InputManager* inputManager = nullptr;
	static inline Object3D* activeObject;
	static inline glm::mat4 activeModelMatrix;
	static inline glm::mat4 activeViewMatrix;
	static inline glm::mat4 activeProjectionMatrix;

	static inline Camera* activeCamera;

	static inline std::map<int, std::vector<Object3D*>> objectList;
	static inline std::map<int, std::vector<Camera*>> cameras;
	static inline std::map<int, std::vector<Light*>> lights;
	static inline std::map<int, std::vector<Emitter*>> emitters;

	static inline std::map<void*, Entity*> uniqueEntities;
	
	static inline int stepNumber = 0;
	static inline std::vector<pipelineNode> pipeline;
	static inline std::map<float, Object3D*> orderedObjectList;

	
	static  void readPipeLine(std::string fileName);

	static  void  initSystem(enum backend_e backend);


	static  void setActiveCamera(Camera* cam);

	static  void addCamera(int pipelineStep, Camera* c);

	static  void addLight(int pipelineStep, Light* l);

	static  void addEmitter(int pipelineStep, Emitter* l);

	static  void addObject(int pipelineStep, Object3D* obj);

	static void destroySystem();

	static  void mainLoop();
	
	static  collision_t getCollisions(collisionRay_t ray);

};
