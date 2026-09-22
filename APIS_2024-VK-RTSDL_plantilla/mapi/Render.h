#pragma once
#include "Object3D.h"
#include "Texture.h"
class Render {

public:
	typedef enum renderModes {
		offscreen, onscreen
	}renderModes;
	renderModes mode = onscreen;

	int w = 720;
	int h = 480;

	virtual void setupObject(Object3D* obj) = 0;
	virtual void drawObject(Object3D* obj) = 0;
	virtual bool isClosed() = 0;
	virtual void swapBuffers() = 0;
	virtual void setupFrameBuffer() = 0;
	virtual Texture* getBuffer(std::string bufferName) = 0;
	virtual void setOutBuffer(std::string type, std::string bufferName) = 0;
	virtual void drawObjects(std::map<float, Object3D*>& objs) = 0;

	bool enableShadows = true;
	bool enableGlobalLight = true;
	bool enableCaustics = true;
	bool enableReflection = true;
	bool enableRefraction = true;
	bool enableAntiAliasing = true;
};