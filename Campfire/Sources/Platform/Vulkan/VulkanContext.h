#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/GraphicsContext.h"

#include <vector>

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    vk::UniqueInstance CreateInstance();
    vk::PhysicalDevice GetPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

private:
    // TODO change to uniqueInstance?
    vk::UniqueInstance instance;

    std::vector<vk::PhysicalDevice> physicalDevices;
    vk::PhysicalDevice physicalDevice;

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;

    vk::UniqueDevice device;

    vk::UniqueCommandPool commandPool;
    vk::UniqueCommandBuffer commandBuffer;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;

    //vk::PhysicalDeviceProperties deviceProperties;
    //vk::PhysicalDeviceFeatures deviceFeatures;
    //vk::PhysicalDeviceMemoryProperties deviceMemoryProperties;
    //vks::Version version;
    //vks::Version driverVersion;

    GLFWwindow* windowHandle;
};
