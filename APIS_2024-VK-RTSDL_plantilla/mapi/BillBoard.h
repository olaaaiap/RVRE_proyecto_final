#pragma once
#include "Object3D.h"
class BillBoard :
    public Object3D
{
public:
    float spin = 0.0f;//rotación en eje z
    BillBoard(std::string mshFile, glm::vec3 pos = { 0,0,0 }) :Object3D(mshFile, pos)
    {
    }

    virtual  glm::mat4 computeModelMatrix() override;

    virtual void step(float timeStep);
};

