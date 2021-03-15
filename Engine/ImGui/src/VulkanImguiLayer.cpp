#include "Core/Application.h"
#include "ImGui/VulkanImGuiLayer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

VulkanImGuiLayer::VulkanImGuiLayer()
    : Layer("VulkanImGuiLayer")
{
}

void VulkanImGuiLayer::OnImGuiRender()
{
}

void VulkanImGuiLayer::OnAttach()
{
    // Initialize imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style;
    ImGui::StyleColorsDark();

    // Configuration flags
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    Application& app = Application::Get();
    // Setup Platform/Renderer bindings
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

    ImGui_ImplGlfw_InitForVulkan(window, true);

    auto deviceImpl = VulkanContext::Get()->GetDevice();
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = VulkanContext::GetInstance();
    initInfo.PhysicalDevice = deviceImpl->GetVulkanPhysicalDevice();
    initInfo.Device = deviceImpl->GetVulkanDevice();
    initInfo.QueueFamily = deviceImpl->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS);
    initInfo.Queue = deviceImpl->GetGraphicsQueue();
    initInfo.PipelineCache = nullptr; // TODO pass my pipeline here after I convert to pipelineCache

    auto swapchainImpl = VulkanContext::Get()->GetSwapChain();
    vk::DescriptorPoolCreateInfo poolInfo{};
    uint32_t imageCount = static_cast<uint32_t>(swapchainImpl->GetImages().size());
    poolInfo.maxSets = imageCount;
    //poolInfo.poolSizeCount = 11;
    //poolInfo.pPoolSizes = poolSizes;

    //vk::UniqueDescriptorPool descriptorPool = device.createDescriptorPoolUnique(poolInfo);
    //initInfo.DescriptorPool = descriptorPool.get();
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = imageCount;
    initInfo.CheckVkResultFn = nullptr;


    //ImGui_ImplVulkan_Init(&initInfo, pipelineImpl->GetImGuiRenderPass());
}

void VulkanImGuiLayer::OnEvent(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    // Disable for now since viewport needs access to mouse events
    //e.handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    e.handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}

void VulkanImGuiLayer::OnDetach()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Beginning of frame
void VulkanImGuiLayer::Begin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::ShowDemoWindow();
}

// End of frame
void VulkanImGuiLayer::End()
{
    ImGui::Render();
    auto cmdBuffer = vkUtil::BeginSingleTimeCommands();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
    vkUtil::EndSingleTimeCommands(cmdBuffer);

    /*
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    */
}
