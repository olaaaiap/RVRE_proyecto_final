#include "System.h"
#include "FactoryEngine.h"
#include <pugixml.hpp>

 void System::readPipeLine(std::string fileName)
{
	pipeline.clear();
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());
	if (result) {
		// Cargado correctamente, podemos analizar su contenido ...
		pugi::xml_node pipelineTree = doc.child("pipeline");
		for (pugi::xml_node stepNode = pipelineTree.child("step");
			stepNode;
			stepNode = stepNode.next_sibling("step"))
		{
			pipelineNode p;
			auto inputNode = stepNode.child("input");
			if (inputNode)
				for (auto input : inputNode.children())
					p.input[input.name()] = input.text().as_string();
			auto outputNode = stepNode.child("output");
			if (outputNode)
				for (auto output : outputNode.children())
					p.output[output.name()] = output.text().as_string();
			pipeline.push_back(p);
		}
	}
}

 void System::initSystem(enum backend_e backend) {
	FactoryEngine::setSelectedGraphicsBackend(backend);
	FactoryEngine::setSelectedInputBackend(backend);

	render = FactoryEngine::getNewRender();
	//iniciar eventos
	inputManager = FactoryEngine::getNewInputManager();
	inputManager->init();
}

 void System::setActiveCamera(Camera* cam)
{
	activeCamera = cam;
}

 void System::addCamera(int pipelineStep, Camera* c) {

	cameras[pipelineStep].push_back(c);
	uniqueEntities[c] = c;
}

 void System::addLight(int pipelineStep, Light* l)
{
	lights[pipelineStep].push_back(l);
	uniqueEntities[l] = l;
}

 void System::addEmitter(int pipelineStep, Emitter* l)
{
	emitters[pipelineStep].push_back(l);
	uniqueEntities[l] = l;

}

 void System::addObject(int pipelineStep, Object3D* obj) {
	

	 objectList[pipelineStep].push_back(obj);
	render->setupObject(obj);
	uniqueEntities[obj] = obj;

}
  void System::destroySystem()
 {
	 delete render;
	 //for (auto& obj : objectList)
	 //	delete obj;
 }
 

 void System::mainLoop()
{
	float newTime = static_cast<float>(glfwGetTime());
	float deltaTime = 0;
	float lastTime = newTime;

	while (!render->isClosed())	//bucle de renderizado
	{	//actualizar eventos

		//actualizar tiempo
		newTime = static_cast<float>(glfwGetTime());
		deltaTime = newTime - lastTime;
		lastTime = newTime;
		//mover objetos únicos
		for (auto& obj : uniqueEntities)
		{
			obj.second->step(deltaTime);

		}

		//por cada step
		stepNumber = 0;
		for (auto step : pipeline)
		{
			setActiveCamera(cameras[stepNumber][0]);

			activeViewMatrix = activeCamera->getViewMatrix();
			activeProjectionMatrix = activeCamera->getProjectionMatrix();
			//ordenar objetos del mundo y del emisor
			orderedObjectList.clear();
			
			for (auto& obj : objectList[stepNumber])
			{
				float dist = glm::distance(activeCamera->pos, obj->pos);
				dist += ((float)(std::rand() % 100000)) / 100000.0f;
				orderedObjectList[dist] = obj;
			}
			for (auto& e : emitters[stepNumber])
			{
				for (auto& p : e->particleList)
				{
					orderedObjectList[p.first] = p.second;
				}
			}

			//enlazar objetos entrada/salida
			for (auto& bufferName : step.input) {
				auto readBackBuffer =
					render->getBuffer(bufferName.second);
				for (auto& obj : orderedObjectList) {
					for (auto& mesh : obj.second->meshList)
					{
						mesh->mat->
							textures[bufferName.second] = readBackBuffer;
					}
				}
			}
			//activar buffers de salida en render
			for (auto& bufferName : step.output) {
				render->
					setOutBuffer(bufferName.first, bufferName.second);
			}


			//preparar buffer en render

			//dibujar
			render->setupFrameBuffer();
			render->drawObjects(orderedObjectList);

			stepNumber++;
		}
		inputManager->refresh();
		//cambiar buffers
		render->swapBuffers();
	}
}

 collision_t System::getCollisions(collisionRay_t ray)
 {
	 //por cada objeto, comprobar si choca con nuestro rayo de luz
	 collision_t coll, newColl;
	 coll.distance = FLOAT_MAX;
	 for (auto& obit : uniqueEntities)
		 
	 {
		 auto ob = obit.second;
		 //por cada malla del objeto
		 if(ob->entitySubType==OBJECT3D)
		 {
			 for (auto& mesh : ((Object3D*)ob)->meshList)
			 {
				 //obtendremos un árbol binario de búsqueda de colisiones (un std::map)
				 //las colisiones están ordenadas por distancia, la primera posición es la más cercana
				 //al origen del rayo

				 //Cada malla lleva asociada una jerarquía de volúmenes envolventes
				 //pedimos esa jerarquía, y pedimos todas las colisiones que haya con el rayo
				 newColl = mesh->getAABB()->getCollisions(ray, 0);
				 newColl.mesh = mesh;
				 newColl.entity = ob;
				 //y guardamos la colision más cercana
				 if (newColl.distance < coll.distance)
				 {
					 coll = newColl;
				 }
				 }
			 //}
		 }
	 }
	 return coll;

 }


