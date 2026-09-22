#include "GLProgram.h"


void GLShader::compile()
{
	const char* c = source.c_str();
	glShaderSource(glId, 1, &c, nullptr);
	glCompileShader(glId);
}

void GLShader::getErrors()
{
	//TODO
	GLint fragment_compiled;
	glGetShaderiv(glId, GL_COMPILE_STATUS, &fragment_compiled);
	if (fragment_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(glId, 1024, &log_length, message);
		std::cout << "ERROR " << fileName << "\n" << message << "\n\n";
	}
}

GLShader::GLShader(std::string filename,unsigned int type):Shader(filename,type)
{
	 source = loadFile(filename);
	 //crear id
	 this->glId = glCreateShader(type);

	 //compilar codigo
	 compile();
	 //mostrar errores
	 getErrors();
}

GLProgram::GLProgram()
{
	//crear identificador
	programId = glCreateProgram();
}

void GLProgram::addShader(std::string filename)
{

	if (filename.ends_with(".vert"))
	{
		shaderList[GL_VERTEX_SHADER] = new GLShader(filename, GL_VERTEX_SHADER);
	}else if (filename.ends_with(".frag"))
	{
		shaderList[GL_FRAGMENT_SHADER] = new GLShader(filename, GL_FRAGMENT_SHADER);
	}
	else {
		std::cout << "ERROR: extensión del fichero "<<filename<<" no soportada\n";
	}


}

void GLProgram::linkProgram()
{
	for (auto& sh : shaderList)
	{
		glAttachShader(programId, sh.second->glId);
	}
	glLinkProgram(programId);


	GLint program_linked;
	glGetProgramiv(programId, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog(programId, 1024, &log_length, message);
		std::cout << "ERROR  "<< message << "\n\n";
	}




	getVarList();
}

void GLProgram::getVarList()
{
	//listar variables Atributo
	int count = 0;
	glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &count);
	//por cada variable
	while (--count >= 0) {
		std::string varName;
		GLsizei length;
		GLint size;
		GLenum type;
		varName.resize(512);
		//conseguir location
			//conseguir nombre

		glGetActiveAttrib(programId, (GLuint)count, 512, &length, &size, &type, varName.data());
		varName = std::string(varName.c_str());
		//interrogar con nombre
		//guardar con nombre
		varList[varName]=glGetAttribLocation(programId, varName.c_str());
	}
	//listar variables Uniform
	//por cada variable
		//conseguir location

	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);
	//por cada variable
	while (--count >= 0) {
		std::string varName;
		GLsizei length;
		GLint size;
		GLenum type;
		varName.resize(512);
		//conseguir location
			//conseguir nombre

		glGetActiveUniform(programId, (GLuint)count, 512, &length, &size, &type, varName.data());
		varName = std::string(varName.c_str());
		//interrogar con nombre
		if (varName[varName.length() - 1] == ']') {//si es de tipo array
			std::string arrName = varName.substr(0, varName.find('['));
			for (int i = 0; i < size; i++) //coneguir la lista completade nombres
			{
				std::string arrNameIdx = arrName + "[" + std::to_string(i) + "]";
				varList[arrNameIdx] = glGetUniformLocation(programId, arrNameIdx.c_str());
			}
		}
		else
			varList[varName] = glGetUniformLocation(programId, varName.c_str());
	}

	int numUniformsBlocks = 0;

	glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformsBlocks);
	for (int i = 0; i < numUniformsBlocks; i++)
	{
		char blockName[100];

		int bufSize = 100, length = 0, size = 0;
		GLenum type = -1;

		glGetActiveUniformBlockName(programId, (GLuint)i, bufSize, &length, blockName);
		int blockID = glGetUniformBlockIndex(programId, blockName);
		GLint blockSize;
		glGetActiveUniformBlockiv(programId, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

		unsigned int bo;
		glGenBuffers(1, &bo);
		bindingBufferObjects[blockName] = bo;
		glBindBuffer(GL_UNIFORM_BUFFER, bo);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, blockID, bo, 0, blockSize);
		
	}
}

void GLProgram::setUniformMatrix(glm::mat4 m, std::string name)
{
	//buscar location
	std::string key = name;
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glUniformMatrix4fv(location->second, 1, GL_FALSE, &m[0][0]);
	}
	else {

		auto location = bindingBufferObjects.find(key);
		if (location != bindingBufferObjects.end())
		{
			unsigned int bo = location->second;

			glBindBuffer(GL_UNIFORM_BUFFER, bo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), &m[0][0], GL_DYNAMIC_DRAW);
		}
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setUniformVec3(glm::vec3 v, std::string name)
{
	//buscar location
	std::string key = name;
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glUniform3fv(location->second, 1, &v[0]);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setUniformVec4(glm::vec4 v, std::string name)
{
	//buscar location
	std::string key = name;
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glUniform4fv(location->second, 1, &v[0]);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setUniformInt(int i, std::string name)
{
	//buscar location
	std::string key = name;
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glUniform1i(location->second, i);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setUniformFloat(float i, std::string name)
{
	//buscar location
	std::string key = name;
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glUniform1f(location->second, i);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}


void GLProgram::setMVP(glm::mat4 m)
{
	setUniformMatrix(m, "MVP");
}



void GLProgram::setM(glm::mat4 m)
{		
	setUniformMatrix(m,"M");
}

void GLProgram::setLight(Light* l)
{
	setUniformVec3(l->pos, "light.pos");
	setUniformInt(1, "light.enable");
	setUniformVec4(l->color, "light.color");
}

void GLProgram::setShinny(int shinny)
{
	setUniformInt(shinny, "mat.shinny");
}

void GLProgram::setMatColor(glm::vec4 color)
{
	setUniformVec4(color, "mat.color");
}

void GLProgram::setColorCubicSampler(int idx, int use)
{
	//buscar location
	std::string key1 = "textSamplerCube";
	std::string key2 = "mat.useCubeTexture";

	auto location1 = varList.find(key1);
	auto location2 = varList.find(key2);

	if (location1 != varList.end() && location2 != varList.end())
	{
		glUniform1i(location1->second, idx);
		glUniform1i(location2->second, use);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key1 << " no existe\n";
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key2 << " no existe\n";

	}
}


void GLProgram::setColorTextSampler(int idx,int use)
{
	//buscar location
	std::string key1 = "colorText";
	std::string key2 = "useColorText";

	auto location1 = varList.find(key1);
	auto location2 = varList.find(key2);

	if (location1 != varList.end() && location2 != varList.end())
	{
		glUniform1i(location1->second, idx);
		glUniform1i(location2->second, use);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key1 << " no existe\n";
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key2 << " no existe\n";

	}
}

void GLProgram::setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location
	std::string key = "vPos";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location
	std::string key = "vColor";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type)
{	//buscar location
	std::string key = "vNormal";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}



void GLProgram::setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type)
{	//buscar location
	std::string key = "vTangent";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}


void GLProgram::setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type)
{	//buscar location
	std::string key = "vBoneIdxs";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type)
{	//buscar location
	std::string key = "vBoneWeights";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}



void GLProgram::setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location
	std::string key = "vCoordText";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else {
		//std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";

	}
}

void GLProgram::setAlphaMode(alphaModes_e mode) {
	//mezcla alpha TODO vulkan
	switch (mode) {
		case (none):
		{
			glBlendFunc(GL_ONE, GL_ZERO);
			
		}break;
		case alpha:
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		}break;
		case add:
		{
			//mezcla aditiva:
			glBlendFunc(GL_ONE, GL_ONE);
			
		}break;
		case mult:
		{
			//mezcla multiplicativa:
			glBlendFunc(GL_DST_COLOR, GL_ZERO);

		}break;
	}
}

void GLProgram::setDepthEnable(bool enable) {
	if (enable)
	{
		glEnable(GL_DEPTH_TEST);
	}else
		glDisable(GL_DEPTH_TEST);
}
