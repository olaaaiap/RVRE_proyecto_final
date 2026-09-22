#pragma once
#include "common.h"
#include <pugixml.hpp>

using namespace glm;
class Bone {
public:
	int id = 0;

	static inline int idCounter = 0;

	bool updated = false;
	std::string name = "";
	glm::mat4 invPos = mat4(1.0f);
	glm::mat4 boneMatrix = mat4(1.0f);

	std::map<int, glm::vec3> positions;
	std::map<int, glm::quat> rotations;
	std::map<int, glm::vec3> scales;
	Bone* parent = nullptr;

	Bone(std::string name, Bone* parent) : name(name), parent(parent)
	{
		id = idCounter;
		idCounter++;
	}


	void updateParent();

	void update(int frame);

};


class Armature {
public:
	std::map<std::string, Bone*> nameList;
	std::vector<Bone*> idList;
	Armature() {};
	void loadArmature(pugi::xml_node boneBuffer);

	void updateBones(int frame);
};