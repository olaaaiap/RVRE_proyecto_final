#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#pragma once
#define _USE_MATH_DEFINES
#ifdef GLAD_BIN
#define GLAD_GL_IMPLEMENTATION
#endif
#include <glad/gl.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>
#include <vector>
#include <list>

#include <map>
#include <fstream>
#include <string>
#include <math.h>
#include <array>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <limits> 

using namespace std;
template <typename T>
std::vector<T> splitString(const std::string& str, char delim) {
	std::vector<T> elems;
	std::stringstream sstream(str);
	std::string item;
	T tipoDato;
	if (str != "") {
		while (std::getline(sstream, item, delim))
		{
			std::istringstream str(item);
			str >> tipoDato;
			elems.push_back(tipoDato);
		}
	}
	return elems;
}


inline std::string loadFile(std::string fileName)
{
	string code = "";
	std::ifstream f(fileName);
	if (f.is_open()) {
		code = std::string(std::istreambuf_iterator<char>(f), {});
		f.close();
	}
	else {
		std::cout << "ERROR: FICHERO NO ENCONTRADO " << __FILE__ << ":" << __LINE__ << " " << fileName << "\n";
	}
	return code;
}