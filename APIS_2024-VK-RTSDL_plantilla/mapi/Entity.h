#pragma once
#include "common.h"

typedef enum EntityTypes_e
{
	NONE,OBJECT3D,LIGHT,CAMERA,EMITTER
}EntityTypes_e;
class Entity
{
public:

	EntityTypes_e entitySubType=NONE;

	glm::vec3 pos = { 0,0,0 };
	glm::vec3 rot = { 0,0,0 };
	glm::vec3 size = { 1.0f,1.0f,1.0f };

	virtual void step(float deltaTime) = 0;
	virtual glm::mat4 computeModelMatrix()
	{

		glm::mat4 model = glm::translate(glm::mat4(1.0), pos);
		model = glm::rotate(model, rot.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, rot.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rot.z, glm::vec3(0, 0, 1));

		model = glm::scale(model, size);

		return model;

	}
};
