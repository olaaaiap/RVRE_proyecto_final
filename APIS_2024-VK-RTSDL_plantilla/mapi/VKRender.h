#pragma once
#include "Render.h"
#include "VKBufferMemory.h"

class VKRender :public Render {
	//Extensiones que necesitaremos activar en nuestro motor gráfico
	//- SwapChain: Alojamiento de framebuffers que se dibujarán

	std::vector<const char*> deviceExtensions = {
	 VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	//número máximo de imágenes renderizadas "a la vez"



	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	typedef struct bufferObject_t
	{
		VKBufferMemory* vertexBuffer;
		VKBufferMemory* indexBuffer;
	}bufferObject_t;

	std::map<int, bufferObject_t> boList;

	std::vector<Object3D*>	objList;

	GLFWwindow* window;

public:



	VKRender();
	void setupObject(Object3D* obj);
	void drawObject(Object3D* obj);
	void drawObjects(map<float,Object3D*>& objs);
	bool isClosed();
	void swapBuffers();
	void setupFrameBuffer();
	Texture* getBuffer(std::string bufferName);
	void setOutBuffer(std::string type, std::string bufferName);


	void initWindow();
	//crear instancia de  vulkan
	void createInstance();
	//crear surface (framebuffers para dibujado)
	void createSurface();
	//elegir una GPU "física"
	void pickDevice();
	//Crear la estructura de GPU "lógica"
	void createLogicalDevice();
	//imágenes que guardarán el render
	//representan los framebuffers, encolarán "imágenes" con datos 
	//de render que se presentarán en pantalla
	void createSwapChain();
	//estructuras para dibujar esas imágenes ("ventanas")
	void createImageViews();
	void createRenderPass();
	//crear framebuffers
	void createFramebuffers();
	void createSemaphores();
	//createDescriptorPool();
	void createCommandPool();
	//buffers para la lista de comandos
	void createCommandBuffers();



	std::vector<const char*> getRequiredExtensions();
	bool isDeviceSuitable(VkPhysicalDevice device);
	struct QueueFamilyIndices selectQueue();
	VKRender::SwapChainSupportDetails querySwapChainSupport();
	void drawFrame(map<float, Object3D*>& objs);
	void recordCommandBuffers(map<float, Object3D*>& objs, int currentFrame, int imageIndex);
	void addCommands(int cbId, Mesh* mesh);
	GLFWwindow* getWindow()
	{
		return window;
	}

};