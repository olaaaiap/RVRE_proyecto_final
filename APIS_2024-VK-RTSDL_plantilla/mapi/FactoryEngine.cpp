#include "FactoryEngine.h"
#include "GLFWInputManager.h"
#include "GLRender.h"
#include "GLProgram.h"
#include "VKRender.h"
#include "VKProgram.h"
#include "SDLProgram.h"
#include "SDLTexture.h"
#include "SDLRayTraceRender.h"
#include "SDLInputManager.h"

Render* FactoryEngine::getNewRender()
{
    Render* r = nullptr;
    switch (selectedGraphicsBackend)
    {
    case backend_e::GL4:
    case backend_e::GL1:
    {
        r = new GLRender();
    }
    break;
    case backend_e::VK:
    {
        r = new VKRender();
    }
    break;
    case backend_e::RT_SDL:
    {
        r = new SDLRayTrace();
    }break;
    }
    return r;
}

InputManager* FactoryEngine::getNewInputManager()
{
    InputManager* r = nullptr;
    switch (selectedInputBackend)
    {
    case backend_e::GL1:
    case backend_e::GL4:
    case backend_e::VK:
    {
        r = new GLFWInputManager();
    }break;
    case backend_e::RT_SDL:
    {
        r = new SDLInputManager();
    }
    break;
    }
    return r;
}

Material* FactoryEngine::getNewMaterial()
{
    Material* r = nullptr;
    r = new Material();
    return r;
}

RenderProgram* FactoryEngine::getNewProgram()
{
    RenderProgram* r = nullptr;
    switch (selectedGraphicsBackend)
    {
    case backend_e::GL1:
    case backend_e::GL4: 
    {
        r = new GLProgram();
    }
    break;
    case backend_e::VK:
    {
        r = new VKProgram();
    }
    break;
    case backend_e::RT_SDL:
    {
        r = new SDLProgram();
    }
    break;
    }
    return r;
}

Texture* FactoryEngine::getNewTexture()
{
    Texture* r = nullptr;
    switch (selectedGraphicsBackend)
    {
    case backend_e::GL1:
    case backend_e::GL4:
    {
        r = new GLTexture();
    }
    break;
    case backend_e::VK:
    {
        r = new VKTexture();
    }
    break;
    case backend_e::RT_SDL:
    {
        r = new SDLTexture();
    }break;
    }
    return r;
}
Texture* FactoryEngine::getNewTexture(string fileName)
{
    Texture* r = nullptr;
    switch (selectedGraphicsBackend)
    {
    case backend_e::GL1:
    case backend_e::GL4:
    {
        r = new GLTexture(fileName);
    }
    break;
    case backend_e::VK:
    {
        r = new VKTexture(fileName);
    }
    break;
    case backend_e::RT_SDL:
    {
          r = new SDLTexture(fileName);
    }break;
    }
    return r;
}



Texture* FactoryEngine::getNewTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
    std::string dback, std::string  dfront)
{
    Texture* r = nullptr;
    switch (selectedGraphicsBackend)
    {
    case backend_e::GL1:
    case backend_e::GL4:
    {
        r = new GLTexture(dright, dleft, ddown, dup,
            dback, dfront);
    }
    break;
    case backend_e::VK:
    {
        r = new VKTexture(dright, dleft, ddown, dup,
            dback, dfront);
    }
    break;
    case backend_e::RT_SDL:
    {
        //no cube map support
        r = nullptr;//new SDLTexture(dright, dleft, ddown, dup,
            //dback, dfront);
    }break;
    }
    return r;
}