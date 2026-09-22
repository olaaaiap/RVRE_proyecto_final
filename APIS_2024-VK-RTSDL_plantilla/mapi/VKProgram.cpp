#include "VKProgram.h"
#include "vertex.h"


void VKShader::compile()
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	bool optimize = true;
	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_performance);

	module = compiler.CompileGlslToSpv(this->source, (shaderc_shader_kind)this->type, (const char*)fileName.c_str(), options);
	compiledCode = { 0,nullptr };

	if (module.GetCompilationStatus() == shaderc_compilation_status_success) {

		std::vector<uint32_t> v = { module.cbegin(), module.cend() };
		compiledCode.length = v.size();
		compiledCode.data = new uint32_t[compiledCode.length];
		memcpy(compiledCode.data, v.data(), sizeof(uint32_t) * compiledCode.length);
	}
}


void VKShader::getErrors()
{
	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << "ERROR: " << fileName << "\n";
		std::cerr << module.GetErrorMessage();
		compiledCode.data = NULL;
		compiledCode.length = 0;
	}
}

VKShader::VKShader(std::string filename, unsigned int type)
{
	this->fileName = filename;
	this->type = type;
	source = loadFile(filename);
	compile();
	getErrors();
}

VkShaderModule VKShader::createShaderModule(VkDevice device, volatile VKShader::shaderCode_t* code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code->length * sizeof(uint32_t);

	createInfo.pCode = code->data;

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;

}


VKProgram::VKProgram(){}


void VKProgram::addShader(std::string filename) {

	if (filename.ends_with(".vert"))
	{
		shaderList[shaderc_glsl_vertex_shader] = new VKShader(filename, shaderc_glsl_vertex_shader);
	}
	else if (filename.ends_with(".frag"))
	{
		shaderList[shaderc_glsl_fragment_shader] = new VKShader(filename, shaderc_glsl_fragment_shader);
	}
	else {
		std::cout << "ERROR: extensión del fichero " << filename << " no soportada\n";
	}


}
void VKProgram::linkProgram() {
	//no se linka hasta que no se tienen todos los datos
	//se inician las listas de variables uniform
	getVarList();
	createDescriptorSetLayout();

}
void VKProgram::getVarList(){
	//añadir lista de variables uniform y sampler2D al programa
	//Necesitaríamos interrogar al shader al estilo OpenGL, pero no tenemos esas opciones
	// 
	//Por defecto sabemos qué variables hay y sus locations:

	//MVP en binding 0, shader de vértices
	//Sampler textureColor en binding 1, textura de color 
	
	shaderVariable_t MVP = { .dataType = GL_FLOAT,
		.numComponents = 16,
		.blockSize = sizeof(float) * 16,
		.offset = (void*)0,
		.binding = 0, //binding indicado en el código del shader, lo sabemos a priori
		.shaderStage = shaderc_glsl_vertex_shader,
		.data=vector<unsigned char>(sizeof(float) * 16)
	};
	uniformList["MVP"] = MVP;

	shaderVariable_t textureColor = { .dataType = GL_SAMPLER_2D,
		.numComponents = 1,
		.blockSize = 0,
		.offset = (void*)0,
		.binding = 1, //binding indicado en el código del shader, lo sabemos a priori
		.shaderStage = shaderc_glsl_fragment_shader,
		.data = vector<unsigned char>(0)
	};
	samplerList["textureColor"] = textureColor;

}

void VKProgram::addImageBuffer(VKTexture* img) { 
	images.push_back(img); 
}

void VKProgram::setUniformMatrix(glm::mat4 m, std::string name){}
void VKProgram::setUniformVec3(glm::vec3 v, std::string name){}
void VKProgram::setUniformVec4(glm::vec4 v, std::string name){}

void VKProgram::setUniformInt(int i, std::string name){}
void VKProgram::setUniformFloat(float i, std::string name){}

//funciones set de datos
void VKProgram::setMVP(glm::mat4 m) {
	//copiar datos de matriz MVP al buffer correspondiente
	uniformBuffers[vulkanContext::currentFrame][uniformList["MVP"].assignedBuffer]->copyDataToBuffer(uniformList["MVP"].blockSize, &m[0][0]);

}
void VKProgram::setM(glm::mat4 m){}

void VKProgram::setLight(Light* l){}
void VKProgram::setShinny(int shinny){}
void VKProgram::setMatColor(glm::vec4 color){}

void VKProgram::setColorTextSampler(int idx, int use){}
void VKProgram::setColorCubicSampler(int idx, int use){}

void VKProgram::setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type) {
	attributeList["vertexPosition"].blockSize = stride;
	attributeList["vertexPosition"].offset = offset;
	attributeList["vertexPosition"].numComponents = count;
	attributeList["vertexPosition"].dataType = type;
	attributeList["vertexPosition"].location = 0; //lo sabemos a priori, habría que interrogar al shader
}
void VKProgram::setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type) {
	attributeList["vertexColor"].blockSize = stride;
	attributeList["vertexColor"].offset = offset;
	attributeList["vertexColor"].numComponents = count;
	attributeList["vertexColor"].dataType = type;
	attributeList["vertexColor"].location = 1;//lo sabemos a priori, habría que interrogar al shader

}
void VKProgram::setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type) {
	attributeList["vertexNormal"].blockSize = stride;
	attributeList["vertexNormal"].offset = offset;
	attributeList["vertexNormal"].numComponents = count;
	attributeList["vertexNormal"].dataType = type;
	attributeList["vertexNormal"].location = 3;//lo sabemos a priori, habría que interrogar al shader
}
void VKProgram::setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type) {
	attributeList["vertexCoordText"].blockSize = stride;
	attributeList["vertexCoordText"].offset = offset;
	attributeList["vertexCoordText"].numComponents = count;
	attributeList["vertexCoordText"].dataType = type;
	attributeList["vertexCoordText"].location = 2;//lo sabemos a priori, habría que interrogar al shader

}
void VKProgram::setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type){}
void VKProgram::setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type){}
void VKProgram::setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type){}

void VKProgram::use(){}




//modificar para tener una lista de varias fases de shaders
VkShaderStageFlags getShaderStage(unsigned int stage)
{
	switch (stage)
	{
	case shaderc_glsl_vertex_shader:
		return VK_SHADER_STAGE_VERTEX_BIT; //será usado en el shader de vértices
		break;
	case shaderc_glsl_fragment_shader:
		return VK_SHADER_STAGE_FRAGMENT_BIT; //será usado en el shader de vértices
		break;
	case shaderc_glsl_compute_shader:
		return VK_SHADER_STAGE_COMPUTE_BIT; //será usado en el shader de vértices
		break;

	};
	return -1;
}


void VKProgram::createDescriptorSetLayout()
{
	//descriptor del uniform buffer object (en nuestro caso, MVP)
	std::vector<VkDescriptorSetLayoutBinding> ubos;// uboLayoutBinding{};
	for (auto& it : uniformList)
	{

		//sampler2d van aparte


		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = it.second.binding;//el primer buffer enlazado
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //de tipo uniform
		uboLayoutBinding.descriptorCount = 1;//array de 1 elemento
		uboLayoutBinding.stageFlags = getShaderStage(it.second.shaderStage);
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional, para samplers de textura
		ubos.push_back(uboLayoutBinding);
		uniformBindings.push_back(it.second.binding);

	}

	for (auto& it : samplerList)
	{
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = it.second.binding;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = getShaderStage(it.second.shaderStage);

		ubos.push_back(samplerLayoutBinding);
		imageBindings.push_back(it.second.binding);
	}
	//tenemos que crear un identificador para el layout de la estructura que almacenará el MVP
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (uint32_t)ubos.size();
	layoutInfo.pBindings = ubos.data();

	if (vkCreateDescriptorSetLayout(vulkanContext::device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}


VkVertexInputBindingDescription VKProgram::getVertexBindingDescription() {

	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(vertex_t);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}


std::vector<VkVertexInputAttributeDescription> VKProgram::getVertexAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription>  attributeDescriptions{};
	
	//ivec2: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed integers
	//uvec4: VK_FORMAT_R32G32B32A32_UINT, a 4 - component vector of 32 - bit unsigned integers
	//double : VK_FORMAT_R64_SFLOAT, a double - precision(64 - bit) float
	

	for (auto& it : attributeList)
	{
		VkVertexInputAttributeDescription attributeDescription{};
		attributeDescription.binding = 0; //el primer buffer object enganchado (sólo hay uno, el de vértices)
		attributeDescription.location = it.second.location;//lo cargamos en la posición indicada
		switch (it.second.numComponents) //vamos a suponer que siempre son floats
		{
		case 1:
			attributeDescription.format = VK_FORMAT_R32_SFLOAT; //es un float
			break;
		case 2:
			attributeDescription.format = VK_FORMAT_R32G32_SFLOAT; //es un vec2 de floats
			break;
		case 3:
			attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT; //es un vec3 de floats
			break;
		case 4:
			attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT; //es un vec4 de floats
			break;
		default:
			std::cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " tipo no soportado\n";
			break;
		}
		attributeDescription.offset = (uint32_t)it.second.offset; //su desplazamiento
		attributeDescriptions.push_back(attributeDescription);
	}

	return attributeDescriptions;
}







void VKProgram::createGraphicsPipeline() {

	if (inited) return;
	//Antes de cargar los shaders, necesitamos cargar su descripción
	auto bindingDescription = getVertexBindingDescription();
	auto attributeDescriptions = getVertexAttributeDescriptions();

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;


	//y se asignan a una parte del pipeline, hay que crear una lista:
	//vamos a crear un pipeline de dos fases programables, vertices y fragmentos
	for (auto it : shaderList)
	{
		//creamos módulos, OJO, se destruyen al final
		VKShader::shaderCode_t code = ((VKShader*)it.second)->compiledCode;

		VkShaderModule module = VKShader::createShaderModule(vulkanContext::device, &code);
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.module = module;
		shaderStageInfo.pName = "main";
		switch (it.second->type)
		{
		case shaderc_glsl_vertex_shader:
			shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case shaderc_glsl_fragment_shader:
			shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		}

		shaderStages.push_back(shaderStageInfo);

	}


	//Y el resto? las dejamos con las funciones "fixed" //funciones fijas y por defecto

	//fase de descripción de geometría
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	//forma de conectar los Uniform y attributes, de momento vacío
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();




	//cargaremos listas de triángulos

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	//el viewport, tamaños en 3D
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vulkanContext::swapChainExtent.width;
	viewport.height = (float)vulkanContext::swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;


	//área de recorte
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = vulkanContext::swapChainExtent;


	//programamos la estructura para el viewport

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;


	//fase de rasterizado
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;

	//modo "polígonos´"

	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//culling
	rasterizer.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;



	//no tenemos multisample, se hace mezcla de varios fragmentos que van a parar al mismo pixel
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//buffer de profundidad
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	//modo de mezclado de color
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	//de momento, sin mezclado
	//colorBlendAttachment.blendEnable = VK_FALSE;

	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	//de momento, sin mezclado
	//modo de mezclado estándar
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	//	Push Constants, para las matrices y datos propios del modelo
	//VkPushConstantRange psRange;
	//psRange.offset = 0;
	//psRange.size = sizeof(VKProgram::ShaderLocalData_t);
	//psRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//las leemos de los bloques de estructuras, todos deben ser push_constant
	std::vector<VkPushConstantRange> psRanges;
	
	//Creamos el layout (lista de etapas) que tendrá nuestro pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;//vamos a tener variables uniform, añadimos una que enlaza con el vertex shader
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//inicializada anteriormente
	//nuevo, para las push constants
	pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)psRanges.size();
	pipelineLayoutInfo.pPushConstantRanges = psRanges.data();

	if (vkCreatePipelineLayout(vulkanContext::device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		std::cout << "failed to create pipeline layout!\n";
	}
	//con las fases de renderizado
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = vulkanContext::renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	//pipelineInfo.pDynamicState


	if (vkCreateGraphicsPipelines(vulkanContext::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		std::cout << "failed to create graphics pipeline!\n";
	}
	//Se destruye para liberar recursos
	for (auto it : shaderStages) {
		vkDestroyShaderModule(vulkanContext::device, it.module, nullptr);
	}
	inited = true;
}





void VKProgram::createUniformBuffers() {

	uniformBuffers.resize(vulkanContext::swapChainImages.size());

	for (int i = 0; i < vulkanContext::swapChainImages.size(); i++)
		for (auto& it : uniformList)
		{
			if (it.second.data.size() > 0) {
				it.second.assignedBuffer = uniformBuffers[i].size();
				uniformBuffers[i].push_back(new VKUniformBufferObject(it.second.data.size(), it.first));
			}
		}

}



void VKProgram::createDescriptorPool() {

	//descriptores de buffers de variables
	//dos tipos de buffers
	std::vector<VkDescriptorPoolSize> poolSizes{};
	poolSizes.resize(2);
	//uniforms
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(vulkanContext::swapChainImages.size() * 2);
	//y samplers2D
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(vulkanContext::swapChainImages.size() * 2);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(vulkanContext::swapChainImages.size());

	if (vkCreateDescriptorPool(vulkanContext::device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}


void VKProgram::createDescriptorSets() {


	std::vector<VkDescriptorSetLayout> layouts(vulkanContext::swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanContext::swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	//descripción final de los tipos de variables que se van a usar
	descriptorSets.resize(vulkanContext::swapChainImages.size());
	if (vkAllocateDescriptorSets(vulkanContext::device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	for (size_t i = 0; i < vulkanContext::swapChainImages.size(); i++) {

		std::vector <VkDescriptorBufferInfo> bufferInfos{};
		bufferInfos.resize(uniformBuffers[i].size());
		std::vector <VkDescriptorImageInfo> imageInfos{};
		std::vector <VkWriteDescriptorSet> descriptorWrites{};
		int bufferCount = 0;

		for (auto it : uniformBuffers[i])
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = it->getBufferID();
			bufferInfo.offset = 0;
			bufferInfo.range = it->size;//OJO, que esto depende del tipo de uniform que usemos			
			bufferInfos[bufferCount] = bufferInfo;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.pBufferInfo = &bufferInfos[bufferCount];
			descriptorWrite.dstBinding = uniformBindings[bufferCount++];// usar binding de la descripción
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrites.push_back(descriptorWrite);
		}
		int imageCount = 0;
		for (auto it : images)//este array debe coincidir su orden con array imageBindings
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = it->getImageView();
			imageInfo.sampler = it->getImageSampler();
			imageInfos.push_back(imageInfo);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = imageBindings[imageCount++];//tiene que coincidir con el binding usado en getVarList()
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfos[imageInfos.size() - 1];
			descriptorWrites.push_back(descriptorWrite);

		}

		vkUpdateDescriptorSets(vulkanContext::device, (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

	}
}