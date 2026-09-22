#pragma once
#include "common.h"
#include <string.h>
typedef struct mouse_State_t
{
	double xpos;
	double ypos;
}mouse_State_t;

class InputManager {

protected:
	std::map<int, int> keybPressed;
	std::map<int, int> keybKeep;
	std::map<int, int> keybReleased;
	std::map<int, int> keybRefresh;
	mouse_State_t mouse_state;


public:

	virtual void init() = 0;
	virtual bool isPressed(int key) { return keybPressed[key] + keybKeep[key]; }
	virtual bool isPressedOnce(int key) { keybRefresh[key] = keybPressed[key]; return keybPressed[key]; }

	virtual bool isKept(int key) { return keybKeep[key]; }
	virtual bool isReleased(int key) { return keybReleased[key]; }
	std::map<int, int>& getPressedKeys() { return keybPressed; }
	std::map<int, int>& getKeptKeys() { return keybKeep; }
	std::map<int, int>& getReleasedKeys() { return keybReleased; }

	virtual void refresh() = 0;
	mouse_State_t& getMouseState() { return mouse_state; }

	double getMouseX() {
		return mouse_state.xpos;
	}
	double getMouseY() {
		return mouse_state.ypos;
	}
};