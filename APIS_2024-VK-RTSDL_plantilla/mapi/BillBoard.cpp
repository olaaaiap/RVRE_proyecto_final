#include "BillBoard.h"
#include "System.h"

glm::mat4 BillBoard::computeModelMatrix()
{
    glm::mat4 cam=glm::transpose(System::activeViewMatrix);
    cam[0][3] = 0;
    cam[1][3] = 0;
    cam[2][3] = 0;
    cam[3] = glm::vec4(pos, 1);

    cam = glm::scale(cam, size);

    return cam;
}

 void BillBoard::step(float timeStep)
{
}
