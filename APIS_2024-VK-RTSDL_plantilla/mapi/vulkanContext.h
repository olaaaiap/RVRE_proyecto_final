#pragma once
#include "common.h"

typedef struct QueueFamilyIndices {
	std::optional<std::uint32_t> graphicsFamily;
	std::optional<std::uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
}QueueFamilyIndices;

class VKDepthText;

class vulkanContext
{
public:

	static inline GLFWwindow* window;
	static  inline VkInstance instance;
	static  inline VkSurfaceKHR surface; // una surface para dibujar "framebuffer"
	static  inline VkPhysicalDevice physicalDevice; // una GPU "física
	// 
	static  inline VkDevice device;	//datos de GPU "lógica" 
	static  inline VkQueue graphicsQueue;//cola de instrucciones
	static  inline VkQueue presentQueue;    //y cola para enseñar resultados
	//
	static  inline QueueFamilyIndices queue;
	static  inline VkSwapchainKHR swapChain;
	//buffers
	static  inline std::vector<VkImage>  swapChainImages;
	static  inline VkFormat swapChainImageFormat;//tamaños y colores
	static  inline VkExtent2D swapChainExtent;
	//
	static  inline std::vector < VkImageView> swapChainImageViews;
	//
	static  inline VkRenderPass renderPass;

	//variables para usar/actualizar uniforms
//tenemos que describir la estructura uniform


	static  inline std::vector<VkFramebuffer> frameBuffers;

	static  inline VkCommandPool commandPool;

	static  inline std::vector < VkCommandBuffer> commandBuffers;

	static  inline std::vector<VkSemaphore> imageAvailableSemaphores;
	static  inline std::vector<VkSemaphore> renderFinishedSemaphores;
	static  inline std::vector<VkFence> inFlightFences;
	static  inline std::vector<VkFence> imagesInFlight;
	static  inline int currentFrame = 0;

	static  inline VKDepthText* depthBuffer;

};


