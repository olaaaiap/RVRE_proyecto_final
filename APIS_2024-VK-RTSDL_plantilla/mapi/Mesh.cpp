
#include "Mesh.h"

void Mesh::computeAABB()
{
	this->colls->addMesh("file", this, this->mat);
}

void Mesh::updateAABB(glm::mat4 mat)
{
	colls->update(mat);
}
