#pragma once
#include "common.h"
#include "Object3D.h"
#include "Render.h"
#include "GLTexture.h"
class GLRender:public Render {

public:
	typedef struct  {
		unsigned int bufferId;
		unsigned int vertexBufferId;
		unsigned int indexBufferId;
	}bufferObject_t;

	GLFWwindow* window = nullptr;


	GLTexture* colorBuffer = nullptr;
	GLTexture* depthBuffer = nullptr;
	unsigned int backBufferId = 0;
	std::map<std::string, GLTexture*> buffers;

	std::map<int, bufferObject_t> bufferObjectList;

	GLRender();

	void initGLFW();

	void initGL();

	void setupObject(Object3D* obj);
	void drawObject(Object3D* obj);

	bool isClosed();

	void swapBuffers();

	~GLRender();

	void initFrameBuffers();

	void setupFrameBuffer();

	Texture* getBuffer(std::string bufferName);

	void setOutBuffer(std::string type, std::string bufferName);

	void drawObjects(std::map<float, Object3D*>& objs);;

	GLFWwindow* getWindow();

};