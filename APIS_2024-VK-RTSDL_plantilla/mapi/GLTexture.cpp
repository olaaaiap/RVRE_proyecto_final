#include "GLTexture.h"

///este define SOLO EN ESTE ARCHIVO CPP
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLTexture::GLTexture(std::string fileName)
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
		//generar textura de opengl
		glGenTextures(1, &GLId);
		//activar
		glBindTexture(GL_TEXTURE_2D, GLId);
		//seteo parámetros
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//filtros
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//cargar datos

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);

			//generar mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		pixels[0].resize(res.h * res.w * 4);
		memcpy(pixels[0].data(), data, pixels[0].size());
		stbi_image_free(data);
	}


}

GLTexture::GLTexture(std::string dleft, std::string dright,
	std::string dfront, std::string dback,
	std::string dup, std::string ddown)
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
	
	//cargar las 6 imágenes en 6 arrays de datos: dright,dleft,ddown,dup,dback,dfront
	glGenTextures(1, &GLId);
	//activar para modificaciones
	glBindTexture(GL_TEXTURE_CUBE_MAP, GLId);
	//modificaciones
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//copiar bytes
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, res.w, res.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[5]);



	for (int i = 0; i < 6; i++) {
		pixels[i].resize(res.h * res.w * 4);
		memcpy(pixels[i].data(), data[i], pixels[i].size());
		stbi_image_free(data[i]);
	}
}

void GLTexture::bind(int idx)
{
	glActiveTexture(GL_TEXTURE0 + idx);

	switch(type)
	{
		case color2D:
		case colorBuffer:
		case depthBuffer:

			glBindTexture(GL_TEXTURE_2D, GLId);
			break;
		case cubic:
			glBindTexture(GL_TEXTURE_CUBE_MAP, GLId);
		break;
	}

}


GLTextureFB::GLTextureFB(textureType type, int w, int h)
{
	this->type = type;
	res = { w,h };
	glGenTextures(1, &GLId);
	glBindTexture(GL_TEXTURE_2D, GLId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	switch (type)
	{
	case colorBuffer:
	{
		//color
		//al reservar el espacio de la textura, no se inician los colores (nullptr)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}break;
	case depthBuffer:
	{
		//profundidad
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	}break;
	}
}