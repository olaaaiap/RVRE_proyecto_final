#pragma once
#include "BillBoard.h"
class Particle :
    public BillBoard
{
public:
    double lifeSpan = 0;
    glm::vec3 direction = { 0,0,0 };
    float speed=0;

    Particle(std::string fileName, glm::vec3 position, float lifeSpan, glm::vec3 direction, float speed):
        BillBoard(fileName, position), lifeSpan(lifeSpan),direction(direction),speed(speed)
    {

    }
    virtual void step(float timeStep) override
    {
        pos += direction * speed * timeStep;
        lifeSpan -= timeStep;
        //actualizar transparencia en función del tiempo de vida restante
  
    }
};

