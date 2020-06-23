#include <GLFW/glfw3.h>

#include "Renderer/GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Log.h"

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
{
    // TODO create context based on which API is being used
    //switch (Renderer::GetAPI())
    //{
    //    case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
    //}

    return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
}