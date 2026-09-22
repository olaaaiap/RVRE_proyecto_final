#pragma once
#include "common.h"
#include "vertex.h"
#include "Entity.h"
#include "Mesh.h"
#include "Armature.h"


class Object3D: public Entity{
public:
	 
	//static  std::map<std::string, Mesh*> globalObjectList;
	static inline int idGenerator = 0;
	int id = 0;
	
	Armature* armature = nullptr;
	int numFrames = 0;
	std::vector<Mesh*> meshList;

	float speed = 0.05f;

	Object3D() { this->entitySubType = OBJECT3D; };
	Object3D(std::string mshFile, glm::vec3 pos = { 0,0,0 }) ;

	void loadMsh(std::string mshFile);

	void loadObj(std::string obj, Material* mat,bool collisionEnable);

	void recomputeNormals();

	virtual void step(float deltaTime)=0;

	

};





class Skybox : public Object3D
{
public:

	Skybox() : Object3D("data/skybox1/skybox_RT.msh") {
		this->size = glm::vec3(100.0f);
	};
	Skybox(glm::vec3 pos) : Object3D("data/skybox1/skybox_RT.msh", pos)
	{
		this->size = glm::vec3(100.0f);
	}

	virtual void step(float deltaTime)
	{}
};

class Generic3D : public Object3D
{
public:

	Generic3D(std::string fileName) : Object3D(fileName) {
	};
	Generic3D(std::string fileName,glm::vec3 pos) : Object3D(fileName, pos)
	{
	}


	float timeAccum = 0;
	int frame = 0;
	virtual void step(float deltaTime);
};


class Plane2D : public Object3D
{
public: 

	Plane2D() : Object3D("data/plane2D.msh") {};
	Plane2D(glm::vec3 pos) : Object3D("data/plane2D.msh", pos)
	{
	}


	virtual void step(float deltaTime);

};






