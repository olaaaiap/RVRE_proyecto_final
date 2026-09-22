#pragma once
#include "RenderProgram.h"
#include <shaderc/shaderc.hpp>
#include "VKTexture.h"

class VKShader:public  Shader
{
public:
	typedef struct shaderCode_t
	{
		size_t length;
		uint32_t* data;
	}shaderCode_t;

	shaderc::SpvCompilationResult module;
	shaderCode_t compiledCode;

	virtual void compile() ;
	virtual void getErrors() ;

	VKShader(std::string filename, unsigned int type);
	static VkShaderModule createShaderModule(VkDevice device, volatile VKShader::shaderCode_t* code);

};



typedef struct {

	unsigned int dataType;
	int numComponents;
	size_t blockSize;
	size_t dataSize;
	void* offset;
	int binding; //para variables uniform
	int location;//para variables attribute

	unsigned int shaderStage;//modificar para tener una lista de fases accesibles
	vector<unsigned char> data;
	int assignedBuffer;
}shaderVariable_t;

class VKProgram : public RenderProgram
{
public:

	std::vector<std::vector< VKBufferMemory*>> uniformBuffers;
	std::vector<int> uniformBindings;
	std::vector < VKTexture*> images;
	std::vector<int> imageBindings;
	VkDescriptorSetLayout descriptorSetLayout;

	//y enlazarla en el cauce gráfico, para ser accesible desde partes concretas (shaders)
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	bool inited = false;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::map<std::string, shaderVariable_t> attributeList;
	std::map<std::string, shaderVariable_t> uniformList;
	std::map<std::string, shaderVariable_t> samplerList;

	VKProgram();
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

	void setLight(Light* l);
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

	void use();


	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	VkVertexInputBindingDescription getVertexBindingDescription();
	vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();

	void addImageBuffer(VKTexture* img);
	void setAlphaMode(alphaModes_e mode) {//TODO
	}

	void setDepthEnable(bool enable){//TODO
	};
};