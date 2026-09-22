#pragma once
#include "Entity.h"

class Light :
    public Entity
{
public:
    typedef enum  {
        point, directional, spotLight
    }LightType;

    float intensity = 1.0f;

    LightType type = point;
    glm::vec4 color;
    bool enable = false;
    glm::vec4 direction = glm::vec4( 1 );
    bool linearAttenuation = false;

    Light(glm::vec3 pos, LightType type = LightType::point, glm::vec4 color = { 1,1,1,1 }, glm::vec4 direction={1,1,1,1});

    virtual void step(float deltaTime);

    float getPdf() {
        //difusión a lo largo del área de un círculo de radio 1
        return 1.f / (2.f * (float)M_PI);
    }
};
