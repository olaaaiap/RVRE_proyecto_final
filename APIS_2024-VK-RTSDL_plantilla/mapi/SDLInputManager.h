#pragma once
#include "InputManager.h"
#include <SDL.h>


class SDLInputManager : public InputManager
{
private:
	bool closed = false;
public:
	SDLInputManager() {};
	~SDLInputManager() {};
	void init();
	virtual void refresh() override;
	void dispatchEvent(SDL_Event e);
	bool isClosed() { return closed; }
};

