#include "VKTexture.h"
#include "stb_image.h"

VKTexture::VKTexture() {}
VKTexture::VKTexture(std::string fileName) {

    int comp;
    unsigned char* data = stbi_load(fileName.c_str(), &res.w, &res.h, &comp, 4);
    this->type = color2D;


    if (data == nullptr)
    {
        std::cout << __FILE__ << ":" << __LINE__ << " ERROR: fichero " << fileName << " no encontrado\n";
    }
    else
    {
        //copiar datos
        pixels[0].resize(res.h * res.w * 4);
        memcpy(pixels[0].data(), data, pixels[0].size());
        stbi_image_free(data);

        if (!buffer)
        {
            buffer = new VKImageBufferObject(res.w, res.h, "texture");
            imageView = createImageView(buffer->getTextureBufferID(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
            createTextureSampler();
        }
        buffer->copyDataToBuffer(res.w * res.h * 4, pixels[0].data());
    }

}
VKTexture::VKTexture(std::string dright, std::string dleft, std::string  ddown, std::string  dup,
	std::string dback, std::string  dfront){

    std::cout << "Vulkan Cubic textures not supported " << endl;


}
void VKTexture::bind(int idx){}


void VKTexture::createTextureSampler() {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vulkanContext::physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    //VK_FILTER_NEAREST = 0,
    //VK_FILTER_LINEAR = 1,
    //VK_FILTER_CUBIC_IMG
    VkFilter filter = VK_FILTER_NEAREST;//elegir filtrado
    //filter = VK_FILTER_LINEAR;

    //VK_SAMPLER_ADDRESS_MODE_REPEAT
    //VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
    //VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    //VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
    //VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
    VkSamplerAddressMode addrMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    //if (this->getRepeat())
    {
        addrMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
    samplerInfo.addressModeU = addrMode;
    samplerInfo.addressModeV = addrMode;
    samplerInfo.addressModeW = addrMode;
    samplerInfo.anisotropyEnable = VK_TRUE;//sí queremos filtros anisotrópicos
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = 0.25;

    if (vkCreateSampler(vulkanContext::device, &samplerInfo, nullptr, &imageSamplerId) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}


VkImageView VKTexture::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; //imágenes 2D "normales"
    viewInfo.format = format; //rgba
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0; //sin mipmaps
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(vulkanContext::device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

VKDepthText::VKDepthText(int w, int h)
{
    res.w = w;
    res.h = h;
    buffer = new VKDepthBufferObject(w, h, "depthBuffer");
    imageView = createImageView(buffer->getTextureBufferID(), buffer->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

}
