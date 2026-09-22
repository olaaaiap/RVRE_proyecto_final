#include "Material.h"
#include "System.h"
#include "FactoryEngine.h"

void Material::prepare()
{
	program->use();
	//seteo de variables en shader
	program->setMVP(System::activeProjectionMatrix*
		            System::activeViewMatrix * System::activeModelMatrix);
	program->setM(System::activeModelMatrix);
	program->setShinny(shinny);
	program->setMatColor(color);
	program->setUniformInt(0, "mat.enable");
	program->setUniformInt(receiveLight, "mat.receiveLight");
	program->setUniformInt(reflectionEnable, "mat.reflectionEnable");
	program->setUniformInt(refractionEnable, "mat.refractionEnable");
	program->setUniformInt(shadowEnable, "mat.shadowEnable");

	program->setUniformFloat(refractIndex, "mat.refractionIndex");

	//reset de variables:
	program->setUniformInt(0, "textureColor");
	program->setUniformInt(0, "textureNormal");
	program->setUniformInt(0, "textureDepth");
	program->setUniformInt(3, "cubetextureColor");
	//texturas cúbicas después de texturas 2D
	program->setUniformInt(0, "mat.usetextureColor");
	program->setUniformInt(0, "mat.usecubetextureColor");
	program->setUniformInt(0, "mat.usetextureNormal");
	program->setUniformInt(0, "mat.usetextureDepth");


	if (System::lights[System::stepNumber].size()>0)
	{
		Light* l = System::lights[System::stepNumber][0];
		if (l->enable)
			program->setLight(l);
		else
			program->setUniformInt(0, "light.enable");
	}

	program->setUniformVec3(System::activeCamera->pos,"cameraPos");

	if (System::activeObject->armature != nullptr)
	{
		program->setUniformInt(1, "mat.computeBones");
		for (auto b : System::activeObject->armature->idList)
		{
			std::string name = "bones[" + std::to_string(b->id) + "]";
			program->setUniformMatrix(b->boneMatrix, name);
		}
	}
	else
	{
		program->setUniformInt(0, "mat.computeBones");
	}

	int countText = 0;
	for (auto t : textures)
	{
		std::string name = t.first;
		auto texture = t.second;
		switch (texture->type)
		{
		case Texture::textureType::colorBuffer:
		case Texture::textureType::depthBuffer:
		case Texture::textureType::color2D:
			texture->bind(countText);
			program->setUniformInt(countText, name);
			program->setUniformInt(1, "mat.use" + name);
			break;
		case Texture::textureType::cubic:
			texture->bind(countText + 3);//3 primeras texturas de color,
			program->setUniformInt(countText + 3, "cube" + name);
			program->setUniformInt(1, "mat.usecube" + name);
			break;
		};
		countText++;
	}
	

	if (shadowEnable)
	{
		glm::mat4 depthBiasMat(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		glm::mat4 depthViewMatrix = System::cameras[0][0]->getViewMatrix();
		glm::mat4 depthProjMatrix = System::cameras[0][0]->getProjectionMatrix();
		depthBiasMat = depthBiasMat * depthProjMatrix *
			depthViewMatrix * System::activeModelMatrix;
		program->setUniformMatrix(depthBiasMat, "depthBias");
	}


	program->setAlphaMode(alphaMode);

	program->setDepthEnable(depthMaskActive);

}

void Material::computePDF()
{
	if (!reflectionEnable && !refractionEnable)
	{
		this->absorbed = 0.25f;
		this->reflectedDiffuse = 0.75f;
	}

	if (reflectionEnable && !refractionEnable)
	{
		this->absorbed = 0.05f;
		float shinnyPercent = (float)shinny / 100.0f;
		this->reflectedDiffuse = (1.0f - shinnyPercent) * 0.95f;
		this->reflectedSpecular = shinnyPercent * 0.95f;
	}

	if (refractionEnable)
	{
		this->absorbed = 0.05f;
		float shinnyPercent = (float)shinny / 100.0f;
		this->reflectedDiffuse = (1.0f - shinnyPercent) * 0.95f;
		this->reflectedSpecular = (shinnyPercent * 0.95f) / 2.0f;
		this->refracted = (shinnyPercent * 0.95f) / 2.0f;
	}

}

void Material::getPDF( float& absorbed, float& reflectedDiffuse, float& reflectedSpecular, float& refracted)
{
	absorbed = this->absorbed;
	reflectedDiffuse = this->reflectedDiffuse;
	reflectedSpecular = this->reflectedSpecular;
	refracted = this->refracted;
}

 void Material::setTexture(std::string name, std::string fileName, std::string type) {
	if (type == "color2D") {
		textures[name] = FactoryEngine::getNewTexture(fileName);//new VKTexture(fileName);

	}if (type == "cubeMap")
	{
		auto files = splitString<std::string>(fileName, ',');
		textures[name] = FactoryEngine::getNewTexture(files[0], files[1], files[2], files[3], files[4], files[5]);//new VKTexture(files[0], files[1], files[2], files[3], files[4], files[5]);
	}
}

 Material::Material() {
	program = FactoryEngine::getNewProgram();
}

 void Material::loadPrograms(std::vector<std::string> files) {
	for (auto& f : files)
	{
		program->addShader(f);
	}
	program->linkProgram();
}
