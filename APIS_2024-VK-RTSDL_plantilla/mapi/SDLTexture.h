#pragma once
#include "Texture.h"
class SDLTexture :
    public Texture
{
public:


    SDLTexture() {};
    SDLTexture(std::string fileName);
    SDLTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
        std::string dback, std::string  dfront);

    void bind(int idx) override {};

};

