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

    // Logical device
    static vk::UniqueDevice device;

private:
    // Instance
    vk::UniqueInstance CreateInstance();
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    // PhysicalDevice
    vk::PhysicalDevice GetPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);

    // SwapChain
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    void SetupSwapChain();

    // Logical device
    vk::UniqueDevice CreateLogicalDevice();

    // Graphics pipeline
    vk::UniquePipeline CreateGraphicsPipeline();

    // Framebuffers
    void CreateFramebuffers();

    // Command pool/buffer
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    // Semaphores
    vk::UniqueSemaphore CreateSemaphore();

    vk::UniqueFence CreateFence();

private:
    // Instance
    vk::UniqueInstance instance;

    // PhysicalDevice
    std::vector<vk::PhysicalDevice> physicalDevices;
    vk::PhysicalDevice physicalDevice;

    // SwapChain
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;

    // Queues
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;


    // Graphics pipeline
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
    vk::UniquePipeline graphicsPipeline;

    // Framebuffers
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    // Command pool/buffer
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Semaphores
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;

    GLFWwindow* windowHandle;
};
