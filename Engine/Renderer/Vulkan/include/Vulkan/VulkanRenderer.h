#pragma once

#include <vulkan/vulkan.hpp>

class VulkanRenderer
{
public:
    static vk::CommandBuffer& BeginScene();
    static void EndScene(vk::CommandBuffer& cmdBuffer);
    static void DrawIndexed(vk::CommandBuffer& commandBuffer, vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);
};
