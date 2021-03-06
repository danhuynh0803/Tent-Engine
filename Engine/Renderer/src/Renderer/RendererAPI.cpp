#include "Renderer/RendererAPI.h"
#include "OpenGL/OpenGLRendererAPI.h"
#include "Vulkan/VulkanRendererAPI.h"

//RendererAPI::API RendererAPI::sAPI = RendererAPI::API::OpenGL;
RendererAPI::API RendererAPI::sAPI = RendererAPI::API::Vulkan;

UniquePtr<RendererAPI> RendererAPI::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateUniquePtr<OpenGLRendererAPI>();
        //case RendererAPI::API::Vulkan:
        //    return CreateUniquePtr<VulkanRendererAPI>();
    }

    return nullptr;
}
