#pragma once
#include "common.h"
#include "Light.h"


typedef enum alphaModes_e {
	alpha, add, mult, none
}alphaModes_e;

class Shader
{
public:
	unsigned int type;
	unsigned int glId = -1;
	std::string source;
	std::string fileName;
	virtual void compile() = 0;
	virtual void getErrors() = 0;
	Shader() {};
	Shader(std::string filename, unsigned int type) :type(type), fileName(filename) {};

};

class RenderProgram {
public:
	std::map<unsigned int, Shader*> shaderList;
	unsigned int programId = -1;
	std::map<std::string, unsigned int> varList;
	std::map<std::string, unsigned int> bindingBufferObjects;


	virtual void addShader(std::string filename) = 0;
	virtual void linkProgram() = 0;
	virtual void getVarList() = 0;

	virtual void setUniformMatrix(glm::mat4 m, std::string name) = 0;
	virtual void setUniformVec3(glm::vec3 v, std::string name) = 0;
	virtual void setUniformVec4(glm::vec4 v, std::string name) = 0;

	virtual void setUniformInt(int i, std::string name) = 0;
	virtual void setUniformFloat(float i, std::string name) = 0;

	//funciones set de datos
	virtual void setMVP(glm::mat4 m) = 0;
	virtual void setM(glm::mat4 m) = 0;

	virtual void setLight(Light* l) = 0;
	virtual void setShinny(int shinny) = 0;
	virtual void setMatColor(glm::vec4 color) = 0;

	virtual void setColorTextSampler(int idx, int use) = 0;
	virtual void setColorCubicSampler(int idx, int use) = 0;
	virtual void setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type) = 0;
	virtual void use() = 0;
	virtual void setAlphaMode(alphaModes_e mode)=0;
	virtual void setDepthEnable(bool enable)=0;
};