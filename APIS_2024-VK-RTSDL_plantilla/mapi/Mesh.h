#pragma once
#include "vertex.h"
#include "Material.h"
#include "AABB.h"

class Mesh
{
public:
	static inline int idGenerator = 0;
	int id= 0;

	std::vector < vertex_t> vertexList;
	std::vector < unsigned int> idList;

	Material* mat = nullptr;

	//colisiones
	glm::vec3 min = glm::vec3(10000000, 10000000, 10000000);
	glm::vec3 max = glm::vec3(-10000000, -10000000, -10000000);
	glm::vec3 center = (min + max) / 2.0f;
	AABBColls* colls = nullptr;
	bool collisionEnable = true;
	Mesh() {
		id = idGenerator++;
		this->colls = new AABBColls(this->getVertList());
	}

	void computeAABB();
	void updateAABB(glm::mat4 mat);
	AABBColls* getAABB() { return colls; }
	
	std::vector < vertex_t>& getVertList() {
		return vertexList;
	};

	std::vector < unsigned int>& getTriangleIdxList() {
		return idList;
	};

};
