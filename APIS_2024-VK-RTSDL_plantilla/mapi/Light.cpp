#include "Light.h"
#include "System.h"

 void Light::step(float deltaTime) {
    if (System::inputManager->isPressedOnce(GLFW_KEY_1))
    {
        enable = !enable;
    }
}

Light::Light(glm::vec3 pos, LightType type, glm::vec4 color, glm::vec4 direction) :
    type(type), color(color), direction(direction)
{
    this->pos = pos;
    enable = true;
}
