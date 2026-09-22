#include "VKRender.h"
#include "vulkanContext.h"
#include "VKBufferMemory.h"
#include "System.h"
#include "VKTexture.h"
#include "VKProgram.h"

VKRender::VKRender() {



	initWindow();
	//crear instancia de  vulkan
	createInstance();
	//crear surface (framebuffers para dibujado)
	createSurface();
	//elegir una GPU "física"
	pickDevice();
	//Crear la estructura de GPU "lógica"
	createLogicalDevice();
	//imágenes que guardarán el render
	//representan los framebuffers, encolarán "imágenes" con datos 
	//de render que se presentarán en pantalla
	createSwapChain();
	//estructuras para dibujar esas imágenes ("ventanas")
	createImageViews();

	vulkanContext::depthBuffer = new VKDepthText(vulkanContext::swapChainExtent.width, vulkanContext::swapChainExtent.height);
	
	//atar nuestros framebuffers a "renders", hasta ahora solo son arrays de datos
	createRenderPass();
	//crear framebuffers
	createFramebuffers();
	createSemaphores();
	//createDescriptorPool();
	createCommandPool();
	//buffers para la lista de comandos
	createCommandBuffers();
}

void VKRender::setupObject(Object3D* obj)
{
	objList.push_back(obj);
	auto meshes = obj->meshList;
	for (auto mesh : meshes) {
		VKProgram* prg = (VKProgram*)mesh->mat->program;
		if (!prg->inited)
		{
			prg->use();

			prg->setVertexPos(sizeof(vertex_t), (void*)offsetof(vertex_t, pos), 4, GL_FLOAT);
			prg->setVertexColor(sizeof(vertex_t), (void*)offsetof(vertex_t, color), 4, GL_FLOAT);
			prg->setVertexNormal(sizeof(vertex_t), (void*)offsetof(vertex_t, normal), 4, GL_FLOAT);
			prg->setVertexCoordText(sizeof(vertex_t), (void*)offsetof(vertex_t, coordText), 2, GL_FLOAT);
			prg->setVertexTangent(sizeof(vertex_t), (void*)offsetof(vertex_t, tangent), 4, GL_FLOAT);
			prg->setVertexBonesIdx(sizeof(vertex_t), (void*)offsetof(vertex_t, boneIdx), 4, GL_FLOAT);
			prg->setVertexBonesWeights(sizeof(vertex_t), (void*)offsetof(vertex_t, boneWeights), 4, GL_FLOAT);


			//crear un pipeline:
		//- Definición de primitivas
		//- Transformación de vértices (vert. shader)
		//- Unir vértices y crear nuevos vértices "internos" (teselación shader)
		//- Crear nueva geometría (geometry shader)
		//- Rasterizado (3D->2D + depth buffer)
		//- Texturado/iluminación/sombreado (fragment shader)
		//- Mezclado de color entre nuevo objeto y fondo (Blending)
			prg->createGraphicsPipeline();
			//Cola/memoria de comandos a enviar cuando se renderice, con sus datos
			//más adelante, creamos los buffers de vértices
			//prg->createVertexBuffer(sizeof(vertex_t) * obj->getMesh()->getVertList().size());
			//prg->createIndexBuffer(sizeof(vertex_t) * obj->getMesh()->getVertList().size());
			prg->createUniformBuffers();
			//setup de texturas
			if (mesh->mat->textures.size() > 0)
				prg->addImageBuffer(((VKTexture*)mesh->mat->textures["textureColor"]));
			prg->createDescriptorPool();

			prg->createDescriptorSets();
		}
		prg->inited = true;

		//iniciar bufferObjects y.. 
		bufferObject_t bo;
		bo.vertexBuffer = new VKVertexBufferObject(sizeof(vertex_t) * mesh->vertexList.size(), "vertexBuffer");
		bo.indexBuffer = new VKIndexBufferObject(sizeof(unsigned int) * mesh->idList.size(), "indexBuffer");
		//copiar datos a memoria de GPU
		bo.vertexBuffer->copyDataToBuffer(sizeof(vertex_t) * mesh->vertexList.size(), mesh->vertexList.data());
		bo.indexBuffer->copyDataToBuffer(sizeof(unsigned int) * mesh->idList.size(), mesh->idList.data());

		boList[mesh->id] = bo;
	}

}
void VKRender::drawObject(Object3D* obj){

	map<float, Object3D*> objs;
	objs[0] = obj;
	drawObjects(objs);

}
void VKRender::drawObjects(map<float, Object3D*>& objs)
{
	drawFrame(objs);
}
bool VKRender::isClosed() {
	return 	 glfwWindowShouldClose(window);
}
void VKRender::swapBuffers(){
	//TODO, soportar pipeline de render

}
void VKRender::setupFrameBuffer(){
}
Texture* VKRender::getBuffer(std::string bufferName) { 
	//TODO, soportar pipeline de render

	return nullptr; 
}
void VKRender::setOutBuffer(std::string type, std::string bufferName){
	//TODO, soportar pipeline de render
}

void VKRender::initWindow()
{
	//no admitimos redimensionamiento de ventanas
	int res=glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	vulkanContext::window = glfwCreateWindow(640, 480, "Vulkan", nullptr, nullptr);
	this->window = vulkanContext::window;
}

//Seleccionar las extensiones necesarias para glfw
std::vector<const char*> VKRender::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	return extensions;
}

void VKRender::createInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "APIS3D Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "APIS3D Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	//necesitamos reservar espacio para nuestra instancia, con la estructura de información
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//vamos a usar GLFW, tenemos que activar sus extensiones 
	std::vector<const char*> glfwExtensions;
	glfwExtensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = (uint32_t)glfwExtensions.size();
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();
	//sin capas de validación (gestión de errores, nos lo saltamos)
	//más info: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
	createInfo.enabledLayerCount = 0;
	//por último, hacer la petición de instancia
	if (vkCreateInstance(&createInfo, nullptr, &vulkanContext::instance) != VK_SUCCESS) {
		std::cout << "failed to create instance!";
		exit(0);
	}
	//ojo, habrá que borrarlo. Añadir código de destrucción de instancias 
}


void VKRender::createSurface()
{
	//se lo pedimos a glfw
	if (glfwCreateWindowSurface(vulkanContext::instance, vulkanContext::window, nullptr, &vulkanContext::surface) != VK_SUCCESS) {
		std::cout << "failed to create window surface!\n";
	}
}




//evaluamos si una GPU tiene capacidad para nuestra aplicación
bool VKRender::isDeviceSuitable(VkPhysicalDevice device) {
	//primero pedir las propiedades estándar de la GPU
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//y propiedades extra
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	//sólo checkeamos que sea una GPU "real" y que tenga soporte para shaders
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;

}


void VKRender::pickDevice()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	//contar cuantas GPUs tenemos
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanContext::instance, &deviceCount, nullptr);
	//debe haber al menos una
	if (deviceCount == 0) {
		std::cout << "failed to find GPUs with Vulkan support!\n";
		exit(0);
	}

	//pedimos la lista de GPUs
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanContext::instance, &deviceCount, devices.data());
	//y evaluamos si nos sirve:
	for (auto device : devices)
	{
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}
		//opcional, crear un "ranking" de tarjetas según sus capacidades y elegir la más potente
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		std::cout << "failed to find a suitable GPU!\n";
		exit(0);
	}
	else {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		std::cout << "Selected GPU: " << deviceProperties.deviceName << "\n";
		vulkanContext::physicalDevice = physicalDevice;
	}

}




void VKRender::createLogicalDevice()
{
	//hay que encontrar una queue compatible:
	vulkanContext::queue = selectQueue();
	//
	std::set<uint32_t> uniqueQueueFamilies = { vulkanContext::queue.graphicsFamily.value(), vulkanContext::queue.presentFamily.value() };


	//Lista de prioridades para las colas creadas anteriormente. Solo 1 cola por tipo, 100% de prioridad
	//ojo, punteros de stack
	float queuePriority = 1.0f;

	//estructura para crear información de queues.
	//pueden ser distintas las de presentación y gráficos, se 
	//añaden una a una

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		//solo una cola, aquí se añadirán los buffers de comandos para dibujado
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//features, se dejan a valores por defecto
	VkPhysicalDeviceFeatures deviceFeatures{};

	//información del device que estamos creando. Agrupamos las colas creadas y sus caracerísticas
	VkDeviceCreateInfo createInfo{};
	//datos de queue
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (int)queueCreateInfos.size();
	//características soportadas (default)
	createInfo.pEnabledFeatures = &deviceFeatures;

	//OJO: AÑADIR LAS EXTENSIONES A DEVICE_EXTENSION
	createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	//capas de validaciones, no estamos usando
	createInfo.enabledLayerCount = 0;


	//y ya podemos crear el device
	if (vkCreateDevice(vulkanContext::physicalDevice, &createInfo, nullptr, &vulkanContext::device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}
	//OJO, destruirlo al acabar el programa

	//y almacenamos la cola seleccionada	
	vkGetDeviceQueue(vulkanContext::device, vulkanContext::queue.graphicsFamily.value(), 0, &vulkanContext::graphicsQueue);
	vkGetDeviceQueue(vulkanContext::device, vulkanContext::queue.presentFamily.value(), 0, &vulkanContext::presentQueue);


}





VKRender::SwapChainSupportDetails VKRender::querySwapChainSupport() {
	//necesitamos una gpu con capacidad para :
			//cadenas de intercambio de imágenes, mínimos y máximos
			//tipos de color soportado
			//modos de presentación

	SwapChainSupportDetails details;
	//pedimos sus características
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanContext::physicalDevice, vulkanContext::surface, &details.capabilities);
	//pedir formatos de presentación
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext::physicalDevice, vulkanContext::surface, &formatCount, nullptr);

	if (formatCount != 0) {
		//si hay varios formatos, pedir todos
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext::physicalDevice, vulkanContext::surface, &formatCount, details.formats.data());
	}

	//modos de presentación
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanContext::physicalDevice, vulkanContext::surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanContext::physicalDevice, vulkanContext::surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}



void VKRender::createSwapChain()
{
	//pedir características
	SwapChainSupportDetails details = querySwapChainSupport();
	//seleccionar formato
	bool selectedFormat = false;
	VkSurfaceFormatKHR format;
	for (const auto& availableFormat : details.formats) {
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = availableFormat;
			selectedFormat = true;
		}
	}
	//si no hay ninguno de nuestro gusto, coger el primero
	if (!selectedFormat) format = details.formats[0];

	bool selectedPresentationMode = false;
	VkPresentModeKHR presentationMode;
	//seleccionar formatos de ventana, modos de dibujado
	//VK_PRESENT_MODE_MAILBOX_KHR vs VK_PRESENT_MODE_FIFO_KHR
	// FIFO: Obligatorio, forzará al mínimo número de imágenes en swapChain (más ligero, más lento)
	// MAILBOX: Intentará crear más del mínimo de imágenes en swapChain (gasta más cpu, más rápido)
	//buscamos si está la opción "MAILBOX"

	for (const auto& availablePresentation : details.presentModes) {
		if (availablePresentation == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentationMode = availablePresentation;
			selectedPresentationMode = true;
		}
	}
	//si no hay ninguno de nuestro gusto, coger el obligatorio
	if (!selectedPresentationMode) presentationMode = VK_PRESENT_MODE_FIFO_KHR;

	//tamaño de ventana/framebuffer
	int width, height;
	glfwGetFramebufferSize(vulkanContext::window, &width, &height);

	//sacamos tamaños de imagen2D, framebuffers

	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};

	actualExtent.width = std::clamp(actualExtent.width, details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height);

	//cuantas imágenes por swap chain, mínimo+1 (evita pararse mucho tiempo, el máximo debe ser mayor)
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;//seleccionamos el máximo si nos hemos pasado
	}
	//creamos estructura con información de la cadena de intercambios
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vulkanContext::surface;

	//cuantas imágenes vamos a intercambiar
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = actualExtent;
	//cuantas capas tiene cada imagen. 
	//- imágenes 2D: 1
	//- imágenes esteroscópicas: 2.. 

	createInfo.imageArrayLayers = 1;
	//estamos usando renders de imágenes en color
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//podríamos hacerlo concurrente si tuviéramos varias colas de render(queues).
	QueueFamilyIndices indices = vulkanContext::queue;// selectQueue();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;//las imágenes se comparten entre colas
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //una sola cola para cada imagen
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	//transformaciones extra en nuestros framebuffers antes de mostrarlos?
	//podemos investigar qué opciones hay en la variable capabilities.supportedTransforms
	//tipos de transformaciones en VkSurfaceTransformFlagBitsKHR
	createInfo.preTransform = details.capabilities.currentTransform;

	//activar el canal alpha para transparencias
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//añadir el modo de presentación
	createInfo.presentMode = presentationMode;
	//Sólo tiene sentido en entornos multiventana. Se ocultarán los píxeles que solapen con otras ventanas.
	createInfo.clipped = VK_TRUE;

	//las swapchains deberán cambiar cada vez que se redimensione/cambie el formato de 
	//imagen (tamaños de ventana, resoluciones, modos de color...)
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	//y por fin, creamos el swapchain	
	if (vkCreateSwapchainKHR(vulkanContext::device, &createInfo, nullptr, &vulkanContext::swapChain) != VK_SUCCESS) {
		std::cout << "failed to create swap chain!\n";
	}

	//y sacamos los framebuffers generados para uso futuro
	vkGetSwapchainImagesKHR(vulkanContext::device, vulkanContext::swapChain, &imageCount, nullptr);
	vulkanContext::swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanContext::device, vulkanContext::swapChain, &imageCount, vulkanContext::swapChainImages.data());

	//nos guardamos sus propiedades
	vulkanContext::swapChainImageFormat = format.format;
	vulkanContext::swapChainExtent = actualExtent;

}



void VKRender::createImageViews()
{
	//habrá tantas vistas como imágenes a mostrar
	vulkanContext::swapChainImageViews.resize(vulkanContext::swapChainImages.size());
	//por cada imagen
	int i = 0;
	for (auto swapChainImage : vulkanContext::swapChainImages) {

		//estructuras de información para las vistas de imágenes
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImage;
		//vistas para imágenes 2d
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = vulkanContext::swapChainImageFormat;
		//reordenado de canales. En caso de querer imágenes monocromo o intercambio de colores
		//se deja por defecto
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		//formato y uso de la imagen
		//imagen de colores 
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//no usaremos Mipmaps

		createInfo.subresourceRange.baseMipLevel = 0;


		//capas para imágenes, solo tenemos una capa
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		//crear la vista de imagen
		if (vkCreateImageView(vulkanContext::device, &createInfo, nullptr, &(vulkanContext::swapChainImageViews[i])) != VK_SUCCESS) {
			std::cout << "failed to create image views!\n";
		}
		i++;
	}

	//OJO: destruir al final del programa
}


void VKRender::createRenderPass()
{

	VkAttachmentDescription colorAttachment{};
	//atamos una sola imagen de color
	colorAttachment.format = vulkanContext::swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//formas de acceso
	//al inicio se limpia
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//y al final se colocan en el array de la imagen, para poder ser leídos
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	//al inicio no sabemos qué uso tienen las imágenes
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//al final, son imágenes para poder ser accedidas más adelante con el formato del swapchain
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//formas para "atar" el color, usaremos el layout de lectura/escritura óptimo "sólo de color"
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//buffers de profundidad
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = vulkanContext::depthBuffer->getBufferMemory()->getDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



	//vamos a tener varias operaciones de postproceso?
	//en nuestro caso, solo una operación de "proceso"
	VkSubpassDescription subpass{};
	//lo conectamos al sistema de gráficos "básico"
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//salida del color
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	//añadir el buffer de profundidad
	subpass.pDepthStencilAttachment = &depthAttachmentRef;



	//hay que avisar cómo se copian los datos entre cada "subpass". Tenemos uno sólo, hay que configurarlo
	VkSubpassDependency dependency{};
	//copias del anterior al actual
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	//copiamos los buffers de color y profundidad
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	//Cuando esté listo

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	//por fin, creamos el render
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	//hay una dependencia
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(vulkanContext::device, &renderPassInfo, nullptr, &vulkanContext::renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VKRender::createFramebuffers()
{
	//swapChainFramebuffers.resize(swapChainImageViews.size());
	vulkanContext::frameBuffers.resize(vulkanContext::swapChainImages.size());

	for (size_t i = 0; i < vulkanContext::frameBuffers.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			vulkanContext::swapChainImageViews[i],
			vulkanContext::depthBuffer->getImageView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = vulkanContext::renderPass;
		framebufferInfo.attachmentCount = (int32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = vulkanContext::swapChainExtent.width;
		framebufferInfo.height = vulkanContext::swapChainExtent.height;
		framebufferInfo.layers = 1;
		//destruirlos al final
		if (vkCreateFramebuffer(vulkanContext::device, &framebufferInfo, nullptr, &vulkanContext::frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

}

void VKRender::createSemaphores()
{
	int numImages = (int)vulkanContext::swapChainImages.size();

	vulkanContext::imagesInFlight.resize(numImages);
	memset(vulkanContext::imagesInFlight.data(), 0, sizeof(VkFence) * vulkanContext::swapChainImages.size());
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vulkanContext::imageAvailableSemaphores.resize(numImages);
	vulkanContext::renderFinishedSemaphores.resize(numImages);
	vulkanContext::inFlightFences.resize(numImages);

	for (size_t i = 0; i < numImages; i++) {
		if (vkCreateSemaphore(vulkanContext::device, &semaphoreInfo, nullptr, &vulkanContext::imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vulkanContext::device, &semaphoreInfo, nullptr, &vulkanContext::renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(vulkanContext::device, &fenceInfo, nullptr, &vulkanContext::inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

}


void VKRender::createCommandPool()
{

	//añadir doble/triple buffer
	//https://vkguide.dev/docs/chapter-4/double_buffering/
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vulkanContext::queue.graphicsFamily.value();
	poolInfo.flags = 0;

	//VK_COMMAND_POOL_CREATE_TRANSIENT_BIT -> cambiaremos los comandos en cada pasada de render (actualización dinámica)
	//VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT -> resetear cada buffer de comandos por separado. Sólo tenemos uno, no hace falta
	if (vkCreateCommandPool(vulkanContext::device, &poolInfo, nullptr, &vulkanContext::commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}


}



void VKRender::createCommandBuffers()
{
	vulkanContext::commandBuffers.resize(vulkanContext::frameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkanContext::commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //Los comandos enviados no se comparte, sólo se llaman desde este commandbuffer
	allocInfo.commandBufferCount = (uint32_t)vulkanContext::commandBuffers.size();

	//reservamos
	if (vkAllocateCommandBuffers(vulkanContext::device, &allocInfo, vulkanContext::commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}


struct QueueFamilyIndices VKRender::selectQueue()
{

	QueueFamilyIndices queue;
	//pedimos todas las colas para mandar datos
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext::physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext::physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		//Seleccionamos colas gráficas "y" con presentación 
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vulkanContext::physicalDevice, i, vulkanContext::surface, &presentSupport);
		if (presentSupport)
			queue.presentFamily = i;

		if (queue.isComplete()) break;
		i++;
	}

	return queue;
}




void VKRender::addCommands(int cbId, Mesh* mesh) {

	//prgData_t prgData = prgDataList[mesh->getMaterial()->shaderPrg->getProgramId()];
	VKProgram* prg = (VKProgram*)mesh->mat->program;
	vkCmdBindPipeline(vulkanContext::commandBuffers[cbId], VK_PIPELINE_BIND_POINT_GRAPHICS, prg->graphicsPipeline);
	//y mandamos el comando, de momento vacío, pero seleccionamos 3 vértices a dibujar, en la instancia por defecto, 
	//cuando tengamos los vertexbuffer, se añaden
	VkViewport viewport{};
	viewport.x = 0.0f;
	// [POI] When using a negative viewport height, the origin needs to be adjusted too
	viewport.y = (float)vulkanContext::swapChainExtent.height;
	viewport.width = (float)vulkanContext::swapChainExtent.width;
	// [POI] Flip the sign of the viewport's height
	viewport.height = -(float)vulkanContext::swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vulkanContext::commandBuffers[cbId], 0, 1, &viewport);
	//push_constants
	VkBuffer vertexBuffers[] = { boList[mesh->id].vertexBuffer->getBufferID() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(vulkanContext::commandBuffers[cbId], 0, 1, vertexBuffers, offsets);
	//por último, los buffer de índices
	vkCmdBindIndexBuffer(vulkanContext::commandBuffers[cbId], boList[mesh->id].indexBuffer->getBufferID(), 0, VK_INDEX_TYPE_UINT32);
	//y nuevo, los descriptores de uniforms
	vkCmdBindDescriptorSets(vulkanContext::commandBuffers[cbId], VK_PIPELINE_BIND_POINT_GRAPHICS, prg->pipelineLayout, 0, 1, &prg->descriptorSets[cbId], 0, nullptr);
	vkCmdDrawIndexed(vulkanContext::commandBuffers[cbId], static_cast<uint32_t>(mesh->idList.size()), 1, 0, 0, 0);

}



void VKRender::recordCommandBuffers(map<float, Object3D*>& objs, int currentFrame, int imageIndex)
{


	//hay que prepararlos para poder grabar comandos
//	for (size_t i = 0; i < vulkanContext::commandBuffers.size(); i++) {
		//inicializamos cada buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//flags
	//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
	//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT : This is a secondary command buffer that will be entirely within a single render pass.
	//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : The command buffer can be resubmitted while it is also already pending execution.

	if (vkBeginCommandBuffer(vulkanContext::commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	//conectamos cada buffer con el render y framebuffer que se usará
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vulkanContext::renderPass;
	renderPassInfo.framebuffer = vulkanContext::frameBuffers[imageIndex];
	//x,y,w,h
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vulkanContext::swapChainExtent;

	//valores por defecto de fondo y profundidad (glClear buffers)
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	//Y esto serían los comandos que se guardarán
	//"GL_BEGIN"
	vkCmdBeginRenderPass(vulkanContext::commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	//y mandamos el comando, de momento vacío, pero seleccionamos 3 vértices a dibujar, en la instancia por defecto, 
	//cuando tengamos los vertexbuffer, se añaden
	//Material::prepareGlobals();
	for (auto it : objs)
	{
		System::activeModelMatrix = it.second->computeModelMatrix();
		System::activeObject = it.second;

		auto meshes = it.second->meshList;
		for (auto mesh : meshes)
		{
			mesh->mat->prepare();
			addCommands(currentFrame, mesh);
		}
	}

	vkCmdEndRenderPass(vulkanContext::commandBuffers[currentFrame]);

	if (vkEndCommandBuffer(vulkanContext::commandBuffers[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}


void VKRender::drawFrame(map<float, Object3D*>& objs)
{
	//esperamos a que la imagen anterior acabe
	int currentFrame = vulkanContext::currentFrame;
	vkWaitForFences(vulkanContext::device, 1, &vulkanContext::inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//capturamos la nueva
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vulkanContext::device, vulkanContext::swapChain, UINT64_MAX, vulkanContext::imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);


	//si estaba en uso, esperamos a que acabe la anterior
	if (vulkanContext::imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(vulkanContext::device, 1, &vulkanContext::imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	//y asignamos
	vulkanContext::imagesInFlight[imageIndex] = vulkanContext::inFlightFences[currentFrame];


	//VKProgram::updateSharedUniformBuffer(currentFrame, vkc);
	//Material::prepareGlobals();

	recordCommandBuffers(objs, currentFrame, imageIndex);
	//reseteamos la variable, se va a dibujar el actual
	vkResetFences(vulkanContext::device, 1, &vulkanContext::inFlightFences[currentFrame]);

	//cuando está lista, se envía
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//sincronización con los semáforos
	VkSemaphore waitSemaphores[] = { vulkanContext::imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	//buffer de instrucciones a usar
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanContext::commandBuffers[currentFrame];
	//sincronización para avisar que se acabó de ejecutar
	VkSemaphore signalSemaphores[] = { vulkanContext::renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//y se envía a la cola de ejecución
	if (vkQueueSubmit(vulkanContext::graphicsQueue, 1, &submitInfo, vulkanContext::inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}


	//cuando está lista, hay que mostrarla
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	//cuando estén los semáforos abiertos
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	//seleccionamos las imágenes que se usarán de la cadena de intercambios
	VkSwapchainKHR swapChains[] = { vulkanContext::swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(vulkanContext::presentQueue, &presentInfo);

	vulkanContext::currentFrame = (currentFrame + 1) % vulkanContext::swapChainImages.size();

}

