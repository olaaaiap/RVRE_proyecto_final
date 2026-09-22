#pragma once
#include "RenderProgram.h"
class SDLProgram:public RenderProgram
{
public :
	virtual void addShader(std::string filename){};
	virtual void linkProgram(){};
	virtual void getVarList(){};

	virtual void setUniformMatrix(glm::mat4 m, std::string name){};
	virtual void setUniformVec3(glm::vec3 v, std::string name){};
	virtual void setUniformVec4(glm::vec4 v, std::string name){};

	virtual void setUniformInt(int i, std::string name){};
	virtual void setUniformFloat(float i, std::string name){};

	//funciones set de datos
	virtual void setMVP(glm::mat4 m){};
	virtual void setM(glm::mat4 m){};

	virtual void setLight(Light* l){};
	virtual void setShinny(int shinny){};
	virtual void setMatColor(glm::vec4 color){};

	virtual void setColorTextSampler(int idx, int use){};
	virtual void setColorCubicSampler(int idx, int use){};
	virtual void setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type){};
	virtual void use(){};
	virtual void setAlphaMode(alphaModes_e mode){};
	virtual void setDepthEnable(bool enable){};
};

