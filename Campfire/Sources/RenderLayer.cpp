#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "RenderLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "Core/Input.h"
#include "Core/Time.h"


unsigned int triangleWidth = 100;
unsigned int triangleHeight = 100;
float red, green;

glm::vec3 pos;
glm::vec4 color;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    pos = glm::vec3(0.0);
    color = glm::vec4(1.0f);

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
    //glm::vec3 scale = glm::vec3(0.1f);
    //model = glm::scale(model, scale);

    shader->Bind();
    shader->SetFloat("time", static_cast<float>(glfwGetTime()));

    texture->Bind();
    Renderer::Draw(shader, vertexArray);
}

void RenderLayer::OnUpdate()
{
    // Testing input controller
    if (Input::GetKeyDown(KEY_UP))
    {
        pos.y += 0.1f;
    }
    if (Input::GetKeyUp(KEY_DOWN))
    {
        pos.y -= 0.1f;
    }

    DrawTriangles();
}

void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{
}
