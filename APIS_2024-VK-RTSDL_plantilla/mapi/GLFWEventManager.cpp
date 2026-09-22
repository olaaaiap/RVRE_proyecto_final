#include "GLFWInputManager.h"
#include "System.h"
#include <chrono>
#include <thread>
#include "FactoryEngine.h"
#include "GLRender.h"
#include "VKRender.h"

void updateMouse(GLFWwindow* win, double  xpos, double ypos)
{

	System::inputManager->getMouseState().xpos = xpos;
	System::inputManager->getMouseState().ypos = ypos;
}

void updateKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {

	switch (action)
	{
	case GLFW_PRESS:
	{
		System::inputManager->getPressedKeys()[key] = 1;
		System::inputManager->getReleasedKeys()[key] = 0;
	}

	break;


	case GLFW_REPEAT:
		System::inputManager->getKeptKeys()[key] = 1;
		break;

	case GLFW_RELEASE:
		System::inputManager->getPressedKeys()[key] = 0;
		System::inputManager->getKeptKeys()[key] = 0;
		System::inputManager->getReleasedKeys()[key] = 1;
		break;


	}

}

void GLFWInputManager::init()
{
	//memset(keybEvent, 0, sizeof(char) * 512);
	switch (FactoryEngine::getSelectedGraphicsBackend())
	{
	case backend_e::GL1:
	case backend_e::GL4:
	{
		glfwSetKeyCallback(((GLRender*)System::render)->getWindow(), updateKeys);
		glfwSetCursorPosCallback(((GLRender*)System::render)->getWindow(), updateMouse);
		glfwSetInputMode(((GLRender*)System::render)->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(((GLRender*)System::render)->getWindow(), 0.0, 0.0);

	}break;
	case backend_e::VK:
	{
		glfwSetKeyCallback(((VKRender*)System::render)->getWindow(), updateKeys);
		glfwSetCursorPosCallback(((VKRender*)System::render)->getWindow(), updateMouse);
		glfwSetInputMode(((VKRender*)System::render)->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(((VKRender*)System::render)->getWindow(), 0.0, 0.0);

	}break;
	}
}

void GLFWInputManager::refresh()
{


	for (auto& pressed : keybRefresh)
	{
		keybPressed[pressed.first] = 0;
		keybKeep[pressed.first] = pressed.second;
		pressed.second = 0;
	}
	keybRefresh.clear();
	glfwPollEvents();

}
