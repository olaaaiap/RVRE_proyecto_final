#pragma once
#include "RenderProgram.h"
#include "Texture.h"



class Material
{
public:
	//constantes iluminación
	float ka = 1.0f;
	float kd = 0.8f;
	float ks = 1.0f;

	//pdf probabilities:

	float absorbed = 0.0f;
	float reflectedDiffuse = 0.0f;
	float reflectedSpecular = 0.0f;
	float refracted = 0.0f;



	RenderProgram* program=nullptr;
	alphaModes_e alphaMode=none;

	glm::vec4 color = { 1.0f,1.0f, 1.0f, 1.0f };
	int shinny = 1;
	bool depthMaskActive = true;
	int receiveLight = 1;
	
	int reflectionEnable = 0;
	int refractionEnable = 0;
	float refractIndex = 1;
	int shadowEnable = 0;

	std::map<std::string, Texture*> textures;
	void setTexture(std::string name, std::string fileName, std::string type);

	Material();

	void loadPrograms(std::vector<std::string> files);

	void prepare();

	void computePDF();
	void getPDF( float& absorbed, float& reflectedDiffuse, float& reflectedSpecular, float& refracted);

	
};