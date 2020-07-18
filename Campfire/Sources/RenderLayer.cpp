#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer2D.h"

#include "Scene/Scene.h"

#include "RenderLayer.h"

Camera camera(1600, 900, 0.1f, 100.0f);

SharedPtr<Shader> postprocessShader;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    texture2D = Texture2D::Create("../Assets/Textures/awesomeface.png");

    ubo = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    ubo->SetLayout(uboLayout, 0);

    colorFB = FrameBuffer::Create(1600, 900, 16);

    postprocessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
    if (Input::GetMod(MOD_SHIFT) && Input::GetKeyDown(KEY_R))
    {
        ShaderManager::ReloadShaders();
    }

    camera.OnUpdate(dt);

    // Set UBO data
    ubo->Bind();
    uint32_t index = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetProjMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewProjMatrix()));
    ubo->Unbind();

    //colorFB->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec3 scale = glm::vec3(0.5f);
    int x = 50;
    int y = 50;
    Timer timer("Quad draw calls");
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(i-x/2, j-x/2, -5.0f));
            transform = glm::scale(transform, scale);
            glm::vec3 tint = glm::vec3((float)i/x, (float)j/y, 1.0f);
            //Renderer2D::DrawQuad(transform, texture2D, glm::vec4(tint, 1.0f));
            Renderer2D::SubmitQuad(transform, texture2D, glm::vec4(tint, 1.0f));
        }
    }
    Renderer2D::DrawBatch();

    //colorFB->Unbind();

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //Renderer2D::DrawPostProcessQuad(postprocessShader, colorFB->GetColorAttachmentID());

    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFB->GetRenderID());
    //glDrawBuffer(GL_BACK);
    //glBlitFramebuffer(0, 0, 1600, 900, 0, 0, 1600, 900, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RenderLayer::OnImGuiRender()
{
}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
