#pragma once
#include "common.h"
#include "Texture.h"

class GLTexture:public Texture
{
public:

	GLTexture() {};
	GLTexture(std::string fileName);
	GLTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
		std::string dback, std::string  dfront);

	void bind(int idx) override;

};


class GLTextureFB : public GLTexture {
public:

	GLTextureFB(textureType type, int w, int h);

};
