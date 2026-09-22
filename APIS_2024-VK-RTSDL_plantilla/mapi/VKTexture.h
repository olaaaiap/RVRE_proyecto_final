#pragma once

#include "Texture.h"
#include "VKBufferMemory.h"
class VKTexture : public Texture {
public:
	GLuint glTextId = -1;
	VKBufferMemory* buffer = nullptr;
	VkImageView imageView;
	VkSampler imageSamplerId;


	VKTexture();
	VKTexture(std::string fileName) ;
	VKTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
		std::string dback, std::string  dfront);
	virtual void bind(int idx);

	static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VKBufferMemory* getBufferMemory() { return buffer; }
	VkImageView getImageView() { return imageView; }
	VkSampler getImageSampler() { return imageSamplerId; }
	void createTextureSampler();

};


class VKDepthText :public VKTexture {
protected:

public:

	VKDepthText(int w, int h);

};