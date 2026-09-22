#include "Camera.h"
#include "System.h"
glm::mat4 Camera::getProjectionMatrix()
{
    switch (type)
    {
    case ortho:
    {
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 1000.0f);
    } break;
    case perspective:
    {
        float aspect = (float)System::render->w / (float)System::render->h;
        return glm::perspective(glm::radians(90.0f), aspect, 0.01f, 1000.0f);
    }
    };
    return glm::mat4(1.0f);

}

 Camera::Camera(glm::vec3 pos, CameraType type, glm::vec3 lookAt, glm::vec3 up) {
    this->pos = pos;
    this->lookAt = lookAt;
    this->up = up;
    this->type = type;
}

 glm::mat4 Camera::getViewMatrix()
{

    return glm::lookAt(pos, lookAt, up);

}

 void Camera::step(float deltaTime)
{
    if (System::inputManager->isPressed(GLFW_KEY_A))
    {
        pos.x -= speed * deltaTime;
        lookAt.x -= speed * deltaTime;

    }
    if (System::inputManager->isPressed(GLFW_KEY_D))
    {
        pos.x += speed * deltaTime;
        lookAt.x += speed * deltaTime;

    }

    if (System::inputManager->isPressed(GLFW_KEY_W))
    {
        pos.z -= speed * deltaTime;
        lookAt.z -= speed * deltaTime;

    }
    if (System::inputManager->isPressed(GLFW_KEY_S))
    {
        pos.z += speed * deltaTime;
        lookAt.z += speed * deltaTime;

    }

}

 CameraStatic::CameraStatic(glm::vec3 pos, CameraType type, glm::vec3 lookAt, glm::vec3 up)
    :Camera(pos, type, lookAt, up)
{
}


void CameraFPS::step(float deltaTime)
{
    if (System::inputManager->isPressed(GLFW_KEY_A))
    {
        pos += speed * deltaTime * strafeDirection;
        lookAt += speed * deltaTime * strafeDirection;

    }
    if (System::inputManager->isPressed(GLFW_KEY_D))
    {
        pos -= speed * deltaTime * strafeDirection;
        lookAt -= speed * deltaTime * strafeDirection;

    }

    if (System::inputManager->isPressed(GLFW_KEY_W))
    {
        pos -= speed * deltaTime * direction;
        lookAt -= speed * deltaTime * direction;

    }
    if (System::inputManager->isPressed(GLFW_KEY_S))
    {
        pos += speed * deltaTime * direction;
        lookAt += speed * deltaTime * direction;

    }

    //matriz rotación x,y

    static double oldX = 0;
    static double oldY = 0;

    double velX = oldX - System::inputManager->getMouseX();
    double velY = oldY - System::inputManager->getMouseY();

    rot.y += (float)velX * 0.01f;
    rot.x += (float)velY * 0.01f;

    if (glm::abs(rot.x) > glm::radians(45.0f))rot.x -= (float)velY * 0.01f;



    //glm::mat4 mrot = glm::rotate(glm::mat4(1.0f), rot.y, { 0.0f,1.0f,0.0f });
    //mrot= glm::rotate(mrot, rot.x, {1.0f,0.0f,0.0f});

    glm::mat4 mrot = glm::rotate(glm::mat4(1.0f), rot.y, { 0.0f,1.0f,0.0f });
    mrot = glm::rotate(mrot, rot.x, { 1.0f,0.0f,0.0f });


    lookAt = radious * (-directionOriginal);
    lookAt = mrot * glm::vec4(lookAt, 1.0f);
    lookAt += pos;

    up = glm::inverse(glm::transpose(mrot)) * glm::vec4(upOriginal, 0.0f);
    direction = glm::normalize(pos - lookAt);
    strafeDirection = glm::normalize(glm::cross(direction, up));

    oldX = System::inputManager->getMouseX();
    oldY = System::inputManager->getMouseY();


}

 CameraFPS::CameraFPS(glm::vec3 pos, glm::vec3 lookAt) :
     Camera(pos, perspective, lookAt, { 0,1,0 })
 {

     direction = directionOriginal = glm::normalize(pos - lookAt);
     upOriginal = up;
     strafeDirection = glm::normalize(glm::cross(direction, up));
     radious = glm::distance(pos, lookAt);

 }
