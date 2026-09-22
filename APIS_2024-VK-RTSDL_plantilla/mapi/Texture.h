#pragma once
#include "common.h"

class Texture {
public:
	typedef enum {
		color2D, cubic, colorBuffer, depthBuffer
	}textureType;
	textureType type;
	typedef struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	}pixel;

	typedef struct {
		int w;
		int h;
	}resolution;

	std::vector<pixel> pixels[6];
	resolution res;
	unsigned int GLId = -1;

	Texture() {};
	virtual void bind(int idx) = 0;

	glm::vec4 getColorAtXY(int numLayer, int x, int y)
	{
		pixel color = pixels[numLayer][(y * res.w + x)];
		glm::vec4 colorOut=glm::vec4(0.0f);
		colorOut.r = ((float)color.r) / 255.0f;
		colorOut.g = ((float)color.g) / 255.0f;
		colorOut.b = ((float)color.b) / 255.0f;
		colorOut.a = ((float)color.a) / 255.0f;
		return colorOut;
	}
};