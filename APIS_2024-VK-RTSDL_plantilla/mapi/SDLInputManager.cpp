#include "SDLInputManager.h"
#include "FactoryEngine.h"
void SDLInputManager::init()
{
	switch (FactoryEngine::getSelectedGraphicsBackend())
	{
	case backend_e::GL1:
	case backend_e::GL4:
	case backend_e::VK:
		break;
	case backend_e::RT_SDL:
	{

	}break;
	}
}



void SDLInputManager::dispatchEvent(SDL_Event e)
{
	int k = e.key.keysym.sym;
	if (k>= 'a' && k <= 'z')
	{
		k = k - 'a' + 'A';
	}
	switch (e.type) {

	case SDL_KEYDOWN:
		getPressedKeys()[k] = 1;
		break;
	case SDL_KEYUP:
		getPressedKeys()[k] = 0;
		getKeptKeys()[k] = 0;
		getReleasedKeys()[k] = 1;
		break;
	case SDL_WINDOWEVENT:

		switch (e.window.event) {

		case SDL_WINDOWEVENT_CLOSE:
			//exit = true;
			closed = true;
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

void SDLInputManager::refresh()
{
	for (auto& pressed : keybRefresh)
	{
		keybPressed[pressed.first] = 0;
		keybKeep[pressed.first] = pressed.second;
		pressed.second = 0;
	}
	keybRefresh.clear();

	SDL_Event e;
	SDL_PollEvent(&e);
	dispatchEvent(e);
}
