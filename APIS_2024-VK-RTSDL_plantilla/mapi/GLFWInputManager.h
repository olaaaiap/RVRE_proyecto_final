#pragma once
#include "common.h"
#include "InputManager.h"

class GLFWInputManager : public InputManager
{
public:
	GLFWInputManager() {};
	~GLFWInputManager() {};
	void init();
	virtual void refresh() override;
};


