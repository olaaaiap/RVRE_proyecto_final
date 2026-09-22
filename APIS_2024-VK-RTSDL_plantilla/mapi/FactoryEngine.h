#pragma once

#include "InputManager.h"
#include "Render.h"

typedef enum  class backend_e {
	GL1, GL4, VK, RT_SDL
}backend_e;

class FactoryEngine
{


private:


	static inline backend_e selectedGraphicsBackend = backend_e::GL4, selectedInputBackend = backend_e::GL1;
public:
	static backend_e getSelectedGraphicsBackend() { return selectedGraphicsBackend; }
	static backend_e getSelectedInputBackend() { return selectedInputBackend; }

	static void setSelectedGraphicsBackend(backend_e backend) { selectedGraphicsBackend = backend; }
	static void setSelectedInputBackend(backend_e backend) { selectedInputBackend = backend; }
	static Render* getNewRender();
	static InputManager* getNewInputManager();
	
	//nuevo
	static Material* getNewMaterial();
	static RenderProgram* getNewProgram();
	//nuevo2
	static Texture* getNewTexture();
	static Texture* getNewTexture(string fileName);
	static Texture* getNewTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
		std::string dback, std::string  dfront);


};


