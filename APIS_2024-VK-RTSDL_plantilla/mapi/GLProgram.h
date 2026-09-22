#pragma once
#include "common.h"
#include "Light.h"
#include "RenderProgram.h"

class GLShader:public Shader
{
public:
	
	void compile();
	void getErrors();
	GLShader(std::string filename, unsigned int type);
};



class GLProgram: public RenderProgram
{
public:
	

	GLProgram();
	void addShader(std::string filename);
	void linkProgram();
	void getVarList();

	void setUniformMatrix(glm::mat4 m, std::string name);
	void setUniformVec3(glm::vec3 v, std::string name);
	void setUniformVec4(glm::vec4 v, std::string name);

	void setUniformInt(int i, std::string name);
	void setUniformFloat(float i, std::string name);

	//funciones set de datos
	void setMVP(glm::mat4 m);
	void setM(glm::mat4 m);

	void setLight(Light *l);
	void setShinny(int shinny);
	void setMatColor(glm::vec4 color);

	void setColorTextSampler(int idx, int use);	
	void setColorCubicSampler(int idx, int use);
	void setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type);
	void setAlphaMode(alphaModes_e mode);
	void setDepthEnable(bool enable);


	void use() {
		glUseProgram(programId);
	}

};

