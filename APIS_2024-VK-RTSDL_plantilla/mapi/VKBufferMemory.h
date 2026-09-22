#pragma once
#include "common.h"
#include "VulkanContext.h"

class VKBufferMemory {
protected:
	VkBuffer bufferId = {};
	VkImage textureImageId = {};
	VkDeviceMemory memory = {};

	VkBuffer staggingBufferId = {};
	VkDeviceMemory staggingMemory = {};
	VkFormat depthFormat = VK_FORMAT_UNDEFINED;

public:
	std::string bufferName="";
	VkDeviceSize size=0;
	//bufferTypes_e type;
	unsigned int width = 0;
	unsigned int height = 0;

	VKBufferMemory(std::string bufferName):bufferName(bufferName) {};
	static unsigned int  findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static void createBuffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& bid, VkDeviceMemory& mem);
	static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	static VkCommandBuffer  beginSingleTimeCommands();
	static void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	virtual void copyDataToBuffer(VkDeviceSize size, void* data)=0;
	virtual VkBuffer getBufferID() { return bufferId; }
	virtual VkImage getTextureBufferID() { return textureImageId; }
	virtual VkFormat getDepthFormat() { return depthFormat; }

};


class VKVertexBufferObject : public VKBufferMemory {

public:
	VKVertexBufferObject(std::string bufferName) :VKBufferMemory(bufferName) {};
	VKVertexBufferObject(VkDeviceSize size, std::string bufferName);
	void copyDataToBuffer(VkDeviceSize size, void* data) override;
};

class VKIndexBufferObject : public VKVertexBufferObject {

public:
	VKIndexBufferObject(std::string bufferName) :VKVertexBufferObject(bufferName) {};
	VKIndexBufferObject(VkDeviceSize size, std::string bufferName);
};


class VKUniformBufferObject : public VKBufferMemory {

public:
	VKUniformBufferObject(std::string bufferName) :VKBufferMemory(bufferName) {};

	VKUniformBufferObject(VkDeviceSize size, std::string bufferName);
	void copyDataToBuffer(VkDeviceSize size, void* data) override;

};

class VKImageBufferObject : public VKBufferMemory {

public:


	VKImageBufferObject(std::string bufferName) :VKBufferMemory(bufferName) {};

	VKImageBufferObject(int w, int h, std::string bufferName);
	void copyDataToBuffer(VkDeviceSize size, void* data) override;
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

};

class VKDepthBufferObject : public VKBufferMemory {

public:

	VKDepthBufferObject(std::string bufferName) :VKBufferMemory(bufferName) { depthFormat = VK_FORMAT_UNDEFINED; };

	VKDepthBufferObject(int w, int h, std::string bufferName);
	void copyDataToBuffer(VkDeviceSize size, void* data) override;
	static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	static VkFormat findDepthFormat();
};