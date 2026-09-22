#include "Object3D.h"
#include <pugixml.hpp>
#include "System.h"


 void Object3D::loadMsh(std::string mshFile) {
	//abrir/comprobar fichero
	//si correcto
	//por cada buffer
	//	leer material
	//leer color
	//leer textura
	//leer shaders/programas
	//  leer meshData
	//si no correcto
	//error

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(mshFile.c_str());
	if (result) {
		// Cargado correctamente, podemos analizar su contenido ...
		pugi::xml_node buffersNode = doc.child("mesh").child("buffers");
		for (pugi::xml_node bufferNode = buffersNode.child("buffer");
			bufferNode;
			bufferNode = bufferNode.next_sibling("buffer"))
		{
			// Iteramos por todos los buffers
			//leer material
			//leer color
			//leer textura
			//leer shinniness
			//leer shaders/programas
			pugi::xml_node material = bufferNode.child("material");
			Material* mat = new Material();

			glm::vec4 color(0);
			auto colorVec = splitString<float>(material.child("color").text().as_string(), ',');
			color = glm::vec4(colorVec[0], colorVec[1], colorVec[2], colorVec[3]);
			mat->color = color;
			bool collisionEnable = true;

			if (material.child("blendMode"))
			{
				string bmode = material.child("blendMode").text().as_string();

				if (bmode == "none")
					mat->alphaMode = none;
				if (bmode == "alpha")
					mat->alphaMode = alpha;
				if (bmode == "add")
					mat->alphaMode = add;
				if (bmode == "mult")
					mat->alphaMode = mult;
			}
			if (material.child("depthWrite"))
			{
				mat->depthMaskActive = material.child("depthWrite").text().as_bool();
			}

			if (material.child("shininess"))
			{
				mat->shinny = material.child("shininess").text().as_int();
			}
			if (material.child("ka"))
			{
				mat->ka = material.child("ka").text().as_float();
			}
			if (material.child("kd"))
			{
				mat->kd = material.child("kd").text().as_float();
			}
			if (material.child("ks"))
			{
				mat->ks = material.child("ks").text().as_float();
			}

			if (material.child("light"))
			{
				mat->receiveLight =
					material.child("light").text().as_bool() ? 1 : 0;
			}
			if (material.child("reflection"))
			{
				mat->reflectionEnable =
					material.child("reflection").text().as_bool() ? 1 : 0;
			}
			if (material.child("collision"))
			{
				collisionEnable =
					material.child("collision").text().as_bool() ? true : false;
			}

			if (material.child("shadow"))
			{
				mat->shadowEnable =
					material.child("shadow").text().as_bool() ? 1 : 0;
			}
			if (material.child("refraction"))
			{
				mat->refractionEnable =
					material.child("refraction").text().as_bool() ? 1 : 0;

				if (material.child("refractIndex"))
				{
					mat->refractIndex =
						material.child("refractIndex").text().as_float();
				}
			}
			//por cada capa

			std::string textType = material.child("texture").attribute("type").as_string();
			for (pugi::xml_node layerNode = material.child("texture").child("layer");
				layerNode;
				layerNode = layerNode.next_sibling("layer"))
			{
				//si es la capa textureColor, usarla
				std::string name = layerNode.attribute("name").as_string();
				mat->setTexture(name, layerNode.text().as_string(), textType);

			}
			//leer programas
			auto programList = splitString<std::string>(material.child("shader").text().as_string(), ',');
			mat->loadPrograms(programList);

			//  leer meshData
			loadObj(bufferNode.child("meshData").text().as_string(), mat,collisionEnable);
			//actualizar distribución probabilidades
			mat->computePDF();
		}
		auto bonesBuffer = doc.child("mesh").child("bones");
		if (bonesBuffer)
		{
			armature = new Armature();
			armature->loadArmature(bonesBuffer);
		}
		auto framesBuffer = doc.child("mesh").child("lastFrame");
		if (framesBuffer) {
			numFrames = framesBuffer.text().as_int();
		}
	}
	else {
		// No se ha podido cargar
		std::cout << __FILE__ << ":" << __LINE__ << " " << result.description() << std::endl;

	}


}

 void Object3D::loadObj(std::string obj, Material* mat,bool collisionEnable)
{
	std::ifstream f(obj, std::ios_base::in);
	std::vector<glm::vec4> vPos;
	std::vector<glm::vec2> vTC;
	std::vector<glm::vec4> vNorm;
	std::vector<glm::vec4> vTangent;
	std::vector<glm::vec4> vBoneIdx;
	std::vector<glm::vec4> vBoneWeight;

	std::string line;
	bool computeNormals = false;
	Mesh* m = nullptr;
	int vertexOffset = 0;
	while (std::getline(f, line, '\n')) {
		std::istringstream str(line);
		std::string key;
		str >> key;
		if (key[0] != '#' || key == "#vta" || key == "#vboneidx" || key == "#vbonew") {
			if (key == "o")
			{
				if (m) {
					meshList.push_back(m);
					m->computeAABB();
				};
				m = new Mesh();
				m->collisionEnable = collisionEnable;
				m->mat = mat;
				vertexOffset = (int)vPos.size();
			}
			else if (key == "#vboneidx")
			{
				glm::ivec4 v(0);
				str >> v.x >> v.y >> v.z >> v.w;
				vBoneIdx.push_back(v);
			}
			else if (key == "#vbonew")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z >> v.w;
				vBoneWeight.push_back(v);
			}
			else if (key == "v")
			{
				glm::vec4 v(1.0f);
				str >> v.x >> v.y >> v.z;
				vPos.push_back(v);
				m->vertexList.push_back(vertex_t{});
			}
			else if (key == "vn")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z;
				vNorm.push_back(v);
			}
			else if (key == "#vta")
			{
				glm::vec4 v(0);
				str >> v.x >> v.y >> v.z;
				vTangent.push_back(v);
			}
			else if (key == "vt")
			{
				glm::vec2 v(0);
				str >> v.x >> v.y;
				vTC.push_back(v);
			}
			else if (key == "f")
			{
				if(vTC.size()==0)
					vTC.resize(vPos.size());
				std::string vert;
				vertex_t v[3];
				int vIndex[3] = { 0 };
				for (int i = 0; i < 3; i++)
				{
					str >> vert;
					auto indexes = splitString<int>(vert, '/');
					if (indexes.size() == 3)
					{
						v[i] = { vPos[indexes[0] - 1],mat->color,vNorm[indexes[2] - 1],vTC[indexes[1] - 1] };
						if (vTangent.size() >(indexes[0] - 1))
						{
							v[i].tangent = vTangent[indexes[0] - 1];
						}

						if (vBoneIdx.size() > (indexes[0] - 1))
						{
							v[i].boneIdx = vBoneIdx[indexes[0] - 1];
						}
						if (vBoneWeight.size() > (indexes[0] - 1))
						{
							v[i].boneWeights = vBoneWeight[indexes[0] - 1];
						}
					}
					else if (indexes.size() == 1)
					{
						v[i] = { vPos[indexes[0] - 1],mat->color,{ 0,0,0,0 },{ 0,0 } };

						if (vBoneIdx.size() > (indexes[0] - 1))
						{
							v[i].boneIdx = vBoneIdx[indexes[0] - 1];
						}
						if (vBoneWeight.size() > (indexes[0] - 1))
						{
							v[i].boneWeights = vBoneWeight[indexes[0] - 1];
						}
						computeNormals = true;
					}
					m->vertexList[indexes[0] - 1 - vertexOffset] = v[i];
					m->idList.push_back(indexes[0] - 1 - vertexOffset);
				}
			}
		}
	}
	if (m) {
		meshList.push_back(m);
		m->computeAABB();

	}

	if (computeNormals)
		recomputeNormals();
}

 void Object3D::recomputeNormals()
{
	for (auto& m : meshList)
	{
		for (auto& v : m->vertexList)
			v.normal = { 0,0,0,0 };
		for (auto it = m->idList.begin(); it != m->idList.end();)
		{
			vertex_t& v1 = m->vertexList[*it]; it++;
			vertex_t& v2 = m->vertexList[*it]; it++;
			vertex_t& v3 = m->vertexList[*it]; it++;
			//v1.normal = v2.normal = v3.normal = { 0,0,0,0 };
			glm::vec3 l1 = glm::normalize(v2.pos - v1.pos);
			glm::vec3 l2 = glm::normalize(v2.pos - v3.pos);

			glm::vec3 norm = glm::normalize(glm::cross(l2, l1));
			v1.normal = glm::normalize(v1.normal + glm::vec4(norm, 0.0f));
			v2.normal = glm::normalize(v2.normal + glm::vec4(norm, 0.0f));
			v3.normal = glm::normalize(v3.normal + glm::vec4(norm, 0.0f));
		}
	}

}

 void Generic3D::step(float deltaTime)
{
	static float timeAccum = 0;
	timeAccum += deltaTime;
	float FPS = 1.0f / 16.0f;
    if (armature != nullptr)
	{
		if (numFrames > 0) {
			if (timeAccum > FPS) {
				frame = (frame + 1) % numFrames;
				timeAccum -= FPS;
				armature->updateBones(frame);
			}
		}
	}
}

 void Plane2D::step(float deltaTime)
{

	if (System::inputManager->isPressed(GLFW_KEY_COMMA))
		speed += 0.01f;


	if (System::inputManager->isPressed(GLFW_KEY_PERIOD))
		speed -= 0.01f;


	if (System::inputManager->isPressed(GLFW_KEY_R))
	{
		rot.x -= 0.01f;
	}
	if (System::inputManager->isPressed(GLFW_KEY_T))
	{
		rot.x += 0.01f;
	}
	if (System::inputManager->isPressed(GLFW_KEY_F))
	{
		size.x -= 0.1f;
	}
	if (System::inputManager->isPressed(GLFW_KEY_G))
	{
		size.x += 0.10f;
	}
}


Object3D::Object3D(std::string mshFile, glm::vec3 pos)
{
	this->entitySubType = OBJECT3D;
	id = idGenerator++;
	this->pos = pos;
	//si la malla no está en globalObjectList
	//cargarla 
	//aniadirla
	//else 
	//reutilizarla
	loadMsh(mshFile);
}
