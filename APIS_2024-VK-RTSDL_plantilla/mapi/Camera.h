#pragma once
#include "Entity.h"

class Camera :
    public Entity
{
public:
    glm::vec3 lookAt = { 0,0,0 };
    glm::vec3 up = { 0,1,0 };

    float speed =5.0f;

    typedef enum {

        ortho, perspective
    }CameraType;

    CameraType type;

    Camera(glm::vec3 pos, CameraType type = perspective, glm::vec3 lookAt = { 0,0,0 }, glm::vec3 up = { 0,1,0 });

    glm::mat4 getViewMatrix();

    glm::mat4 getProjectionMatrix();
   

    virtual void step(float deltaTime);


};

class CameraStatic :public Camera
{
public:
    CameraStatic(glm::vec3 pos, CameraType type = perspective, glm::vec3 lookAt = { 0,0,0 }, glm::vec3 up = { 0,1,0 });
    virtual void step(float deltaTime) override
    {}
};

class CameraFPS :public Camera
{
public:

    glm::vec3 direction,directionOriginal,strafeDirection;
    glm::vec3 upOriginal;
    float radious=0;

    CameraFPS(glm::vec3 pos, glm::vec3 lookAt);

    virtual void step(float deltaTime);
};

