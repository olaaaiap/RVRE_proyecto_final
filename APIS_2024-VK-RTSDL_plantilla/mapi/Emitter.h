#pragma once
#include "Particle.h"

class Emitter: public Entity
{
public:
	std::map<float, Particle*> particleList;
	float emitterRate = 0;
	std::string model;
	glm::vec3 position = { 0,0,0 };
	float numParticulasNuevas = 0;

	glm::vec3 dirMin,dirMax;
	float speedMin,speedMax;
	float lifeSpanMin,lifeSpanMax;
	Emitter(glm::vec3 position, std::string model,
		glm::vec3 dirMin, glm::vec3 dirMax,
		float speedMin, float speedMax,
		float lifeSpanMin, float lifeSpanMax,
		float emitterRate
		):
		model(model),
		position(position),
		dirMin(dirMin), dirMax(dirMax),
		speedMin(speedMin), speedMax(speedMax),
		lifeSpanMin(lifeSpanMin), lifeSpanMax(lifeSpanMax),
		emitterRate(emitterRate)

	{
		//glm::linearRand()
	}
	

	void step(float timeStep);
};

