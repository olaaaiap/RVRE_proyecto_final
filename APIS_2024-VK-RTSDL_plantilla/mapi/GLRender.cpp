#include "GLRender.h"
#include "System.h"

void GLRender::drawObject(Object3D* obj)
{
	System::activeModelMatrix = obj->computeModelMatrix();
	System::activeObject = obj;

	for (auto& mesh : obj->meshList)
	{
		auto buffer = bufferObjectList[mesh->id];
		//activar
		glBindVertexArray(buffer.bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.indexBufferId);
		//usar
		mesh->mat->prepare();
		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->idList.size(), GL_UNSIGNED_INT, nullptr);
		//deactivar
	}
}


 GLRender::GLRender() {
	initGLFW();
	initGL();
	initFrameBuffers();
}

 void GLRender::initGLFW()
{

	int error = glfwInit();


	//código de gráficos/opengl

	//iniciar ventana
	window = glfwCreateWindow(w, h, "APIS3D 2024",
		nullptr, nullptr);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0.0, 0.0);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);



}

 void GLRender::initGL()
{
	//activar opciones
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

 void GLRender::setupObject(Object3D* obj)
{
	//si la malla no estaba en la lista de buffers
	//cargarla en GPU
	for (auto& mesh : obj->meshList)
	{
		bufferObject_t buffer;
		//reservar identificadores
		
		glGenVertexArrays(1, &buffer.bufferId);
		glGenBuffers(1, &buffer.vertexBufferId);
		glGenBuffers(1, &buffer.indexBufferId);
		//copiar datos de vertices
		glBindVertexArray(buffer.bufferId);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh->vertexList.size(),
			mesh->vertexList.data(), GL_STATIC_DRAW);

		//copiar datos de identificadores
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->idList.size(),
			mesh->idList.data(), GL_STATIC_DRAW);

		bufferObjectList[mesh->id] = buffer;

		mesh->mat->program->use();
		mesh->mat->program->setVertexPos(sizeof(vertex_t), (void*)offsetof(vertex_t, pos), 4, GL_FLOAT);
		mesh->mat->program->setVertexColor(sizeof(vertex_t), (void*)offsetof(vertex_t, color), 4, GL_FLOAT);
		mesh->mat->program->setVertexNormal(sizeof(vertex_t), (void*)offsetof(vertex_t, normal), 4, GL_FLOAT);
		mesh->mat->program->setVertexCoordText(sizeof(vertex_t), (void*)offsetof(vertex_t, coordText), 2, GL_FLOAT);
		mesh->mat->program->setVertexTangent(sizeof(vertex_t), (void*)offsetof(vertex_t, tangent), 4, GL_FLOAT);
		mesh->mat->program->setVertexBonesIdx(sizeof(vertex_t), (void*)offsetof(vertex_t, boneIdx), 4, GL_FLOAT);
		mesh->mat->program->setVertexBonesWeights(sizeof(vertex_t), (void*)offsetof(vertex_t, boneWeights), 4, GL_FLOAT);

	}

}

 bool GLRender::isClosed()
{
	return glfwWindowShouldClose(window);
}

 void GLRender::swapBuffers()
{
	glfwSwapBuffers(window);
}

 GLRender::~GLRender()
{
	glfwDestroyWindow(window);
	//borrar buffer objects de gpu
	glfwTerminate();
}

 void GLRender::initFrameBuffers() {
	colorBuffer = new GLTextureFB(GLTexture::textureType::colorBuffer, w, h);
	depthBuffer = new GLTextureFB(GLTexture::textureType::depthBuffer, w, h);
	glGenFramebuffers(1, &backBufferId); //crear el identificador de frameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, backBufferId); //asociar la textura anterior a ese nuevo framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->GLId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->GLId, 0);
	glDrawBuffer(GL_NONE); //no se generarán datos de pantalla
	glReadBuffer(GL_NONE);
	//volver al principal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//check errores
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR\n");
	}
}

 void GLRender::setupFrameBuffer()
{
	if (mode == offscreen)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, backBufferId);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum drawBuf = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &drawBuf);
	}
	if (mode == onscreen)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

 Texture* GLRender::getBuffer(std::string bufferName) {
	return buffers[bufferName];
}

 void GLRender::setOutBuffer(std::string type, std::string bufferName) {
	mode = offscreen;
	if (type == "color")//guardar los buffers de salida con un nuevo nombre
		buffers[bufferName] = colorBuffer;
	if (type == "depth")
		buffers[bufferName] = depthBuffer;
	if (type == "screen")//si se activa el modo pantalla
		mode = onscreen;
}

 void GLRender::drawObjects(std::map<float, Object3D*>& objs) {
	for (auto& obj : objs)
	{
		//dibujar
		drawObject(obj.second);
	}
}

 GLFWwindow* GLRender::getWindow()
{
	return window;
}
