#include "SDLTexture.h"
#include "stb_image.h"

SDLTexture::SDLTexture(std::string fileName)
{
	int comp;
	unsigned char* data = stbi_load(fileName.c_str(), &res.w, &res.h, &comp, 4);
	this->type = color2D;


	if (data == nullptr)
	{
		std::cout << __FILE__ << ":" << __LINE__ << " ERROR: fichero " << fileName << " no encontrado\n";
	}
	else
	{
		pixels[0].resize(res.h * res.w * 4);
		memcpy(pixels[0].data(), data, pixels[0].size());
		stbi_image_free(data);
	}
}

SDLTexture::SDLTexture(std::string dright, std::string dleft, std::string ddown, std::string dup, std::string dback, std::string dfront)
{
	int comp;
	unsigned char* data[6];
	this->type = cubic;
	data[0] = stbi_load(dleft.c_str(), &res.w, &res.h, &comp, 4);
	data[1] = stbi_load(dright.c_str(), &res.w, &res.h, &comp, 4);
	data[2] = stbi_load(ddown.c_str(), &res.w, &res.h, &comp, 4);
	data[3] = stbi_load(dup.c_str(), &res.w, &res.h, &comp, 4);
	data[4] = stbi_load(dback.c_str(), &res.w, &res.h, &comp, 4);
	data[5] = stbi_load(dfront.c_str(), &res.w, &res.h, &comp, 4);

	for (int i = 0; i < 6; i++) {
		pixels[i].resize(res.h * res.w * 4);
		memcpy(pixels[i].data(), data[i], pixels[i].size());
		stbi_image_free(data[i]);
	}
}
