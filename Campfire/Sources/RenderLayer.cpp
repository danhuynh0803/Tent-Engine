#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Timer.h"
#include "RenderLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Camera.h"

unsigned int triangleWidth = 100;
unsigned int triangleHeight = 100;
float red, green;

glm::vec3 pos;
glm::vec4 color;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

Camera camera(1600, 900, 0.1f, 100.0f);

void RenderLayer::OnAttach()
{
    glEnable(GL_DEPTH_TEST);

    pos = glm::vec3(0.0);
    color = glm::vec4(1.0f);

    Renderer::SetAPI(RendererAPI::OpenGL);

    shader = Shader::Create("triangle", "../Campfire/Shaders/tri.vert", "../Campfire/Shaders/tri.frag");
//    GLfloat vertices[] =
//    {
//       -1.0f,  1.0f,  0.0f,     0.0f, 1.0f,
//       -1.0f, -1.0f,  0.0f,     0.0f, 0.0f,
//        1.0f, -1.0f,  0.0f,     1.0f, 0.0f,
//        1.0f,  1.0f,  0.0f,     1.0f, 1.0f
//    };

        GLfloat vertices[] = {
            // Position           // UV         // Normals
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0, 0, -1,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 0, -1,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, 0, 1,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0, 0, 1,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1, 0, 0,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1, 0, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0, -1, 0,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, -1, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 1, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0, 1, 0,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0
        };

    vertexArray = VertexArray::Create();
    vertexArray->Bind();

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "inPos"},
        { ShaderDataType::FLOAT2, "inUV"},
        { ShaderDataType::FLOAT3, "inNormal"}
    };
    buffer->SetLayout(layout);

    GLuint indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

    vertexArray->AddVertexBuffer(buffer);
    vertexArray->SetIndexBuffer(indexBuffer);

    buffer->Unbind();
    vertexArray->Unbind();

    texture = Texture::Create("../Assets/Textures/uv.png");
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::DrawTriangles()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    //glm::vec3 scale = glm::vec3(0.1f);
    //model = glm::scale(model, scale);

    shader->Bind();
    shader->SetFloat("time", static_cast<float>(glfwGetTime()));
    shader->SetMat4("model", model);
    shader->SetFloat4("color", color);
    shader->SetMat4("viewProjMatrix", camera.GetViewProjMatrix());

    // Create test triangle
    vertexArray->Bind();
    texture->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void RenderLayer::OnUpdate()
{
    camera.OnUpdate();

    // Testing input controller
    if (Input::GetKeyDown(KEY_UP))
    {
        pos.y += 0.1f;
    }
    else if (Input::GetKeyDown(KEY_DOWN))
    {
        pos.y -= 0.1f;
    }

    if (Input::GetKeyDown(KEY_RIGHT))
    {
        pos.x += 0.1f;
    }
    else if (Input::GetKeyDown(KEY_LEFT))
    {
        pos.x -= 0.1f;
    }

    DrawTriangles();
}

void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{

}
