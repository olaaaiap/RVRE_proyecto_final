#define GLAD_GL_IMPLEMENTATION
#include "mapi/common.h"
#include "mapi/System.h"
#include "demos.h"


int main(int argc, char** argv)
{
	System::initSystem(backend_e::RT_SDL);
	System::readPipeLine("data/configVK.pipeline");
	//////////////////
	//DEMOS OPENGL
	// 
	//asianTown();
	//fengShui();
	//particles();
	//skybox1GL();
	//shadowMap();
	//asianTownShadowMap();
	//animation();
	
	//////////////////
	//DEMO VULKAN
	//
	//VulkanDemo();

	//////////////////
	//DEMOS RAYTRACING
	//
	skybox1RT();
	//cornellBox1();//esferas metal/cristal
	//cornellBox2();//tetera cristal
	//cornellBox3();//conejo cristal
	System::mainLoop();
	System::destroySystem();
	return 0;

}