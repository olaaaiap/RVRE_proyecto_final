#pragma once
#include "common.h"
 
typedef struct  {
	glm::vec4 pos;
	glm::vec4 color;
	glm::vec4 normal;
	glm::vec2 coordText;
	glm::vec4 tangent;
	glm::vec4 boneIdx;
	glm::vec4 boneWeights;

}vertex_t;

//
//Estructura para simular un rayo lanzado desde un punto
//
typedef struct collisionRay_t
{
	glm::vec3 origin; //origen del rayo
	glm::vec3 dir;  //dirección
	float distance; //distancia máxima que puede recorrer
	bool inside;
}collisionRay_t;