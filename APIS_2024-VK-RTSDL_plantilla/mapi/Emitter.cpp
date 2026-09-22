#include "Emitter.h"
#include "System.h"

void Emitter::step(float timeStep)
{
	//generar nuevas partículas
		//contador nuevas partículas
	numParticulasNuevas += timeStep * emitterRate;
	int partToEmit = (int)numParticulasNuevas;
	numParticulasNuevas -= (int)partToEmit;
	
	for (int i = 0; i < partToEmit; i++)
	{
		// por cada nueva partícula
		// crear random
		Particle* p = new Particle(model, position,
			glm::linearRand(lifeSpanMin, lifeSpanMax),
			glm::linearRand(dirMin, dirMax),
			glm::linearRand(speedMin, speedMax));
		// setup en render
			System::render->setupObject(p);
			// añadir a lista
			p->step(0.05f);
			float dist = glm::distance(System::activeCamera->pos, p->pos);
			particleList[dist]=p;
	}
	
	std::map<float, Particle*> aux;
	//actualizar lista
	for (auto& p : particleList)
	{
		//eliminar partículas con lifeSpan <0
		if (p.second->lifeSpan > 0)
		{		//step en particulas
			p.second->step(timeStep);
			float dist = glm::distance(System::activeCamera->pos, p.second->pos);
			aux[dist] = p.second;
		}
		else
		{
			//ojo mallas y datos compartidos
			delete p.second;
			//eliminar de render
		}
	}
	//ordenar por distancia a cámara
	particleList = aux;

}
