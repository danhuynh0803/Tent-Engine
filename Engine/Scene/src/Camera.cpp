#include <map>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/ResourceManager.h"
#include "Scene/Camera.h"
#include "Renderer/Renderer.h"

static const std::map<std::string, ClearFlag> str2ClearFlagMap
{
    { "Skybox", ClearFlag::SKYBOX },
    { "Color", ClearFlag::COLOR },
    { "Depth", ClearFlag::DEPTH },
    { "None", ClearFlag::NONE },
};

static const std::map<ClearFlag, std::string> ClearFlag2StrMap
{
    { ClearFlag::SKYBOX, "Skybox" },
    { ClearFlag::COLOR, "Color" },
    { ClearFlag::DEPTH, "Depth"},
    { ClearFlag::NONE,  "None"},
};

Camera::Camera()
    : width(1600.0f), height(900.0f), nearPlane(0.1f), farPlane(100.0f)
{
    SetProjection(width, height);
    Init();
}

Camera::Camera(float w, float h, float nearplane, float farplane)
    : width(w), height(h), nearPlane(nearplane), farPlane(farplane)
{
    SetProjection(w, h);
    Init();
}

void Camera::Init()
{
    shader = ShaderManager::Create("line", SHADERS + "/line.vert", SHADERS + "/line.frag");

    /*
       Frustum vertices:

               4--------------------7
              -|                 -  |
            -  |               -    |
          -    |            -       |
        -      |         -          |
       0---------------3            |
       |       5-------|------------6
       |     -         |          -
       |   -           |      -
       | -             |  -
       1---------------2

    */

    // Create VAO for frustum
    uint32_t indices[] =
    {
        // near
        0, 1,   1, 2,   2, 3,   3, 0,
        // far
        4, 5,   5, 6,   6, 7,   7, 4,
        // connection
        0, 4,   1, 5,   2, 6,   3, 7
    };

    /*
    vertexArray = VertexArray::Create();
    // pos, color, and 8 vertices for frustum
    vertexBuffer = VertexBuffer::Create(3 * 3 * 8 * sizeof(float));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT3, "aColor" }
    };
    vertexBuffer->SetLayout(layout);
    vertexArray->AddVertexBuffer(vertexBuffer);

    SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

    vertexArray->SetIndexBuffer(indexBuffer);

    vertexBuffer->Unbind();
    vertexArray->Unbind();
    */
}

void Camera::DrawFrustum(glm::mat4 transform)
{
    glm::vec3 camPos = glm::vec3(transform[0][3], transform[1][3], transform[2][3]);
    glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 nearCenter = camPos - camFront * nearPlane;
    glm::vec3 farCenter  = camPos - camFront * farPlane;

    float aspectRatio = width / height;
    float nearHeight, farHeight;

    if (isPerspective)
    {
        float fovRadians = glm::radians(vFov);
        nearHeight = 2 * glm::tan(fovRadians / 2) * nearPlane;
        farHeight  = 2 * glm::tan(fovRadians / 2) * farPlane;
    }
    else
    {
        nearHeight = 2 * size;
        farHeight  = 2 * size;
    }

    float nearWidth = nearHeight * aspectRatio;
    float farWidth  = farHeight  * aspectRatio;

    float halfNearHeight = nearHeight * 0.5f;
    float halfNearWidth  = nearWidth  * 0.5f;
    float halfFarHeight  = farHeight  * 0.5f;
    float halfFarWidth   = farWidth   * 0.5f;

    //glm::vec3 nearTL, nearTR, nearBL, nearBR;
    //glm::vec3 farTL, farTR, farBL, farBR;

    glm::vec3 nearTL = nearCenter + camUp * halfNearHeight - camRight * halfNearWidth;
    glm::vec3 nearTR = nearCenter + camUp * halfNearHeight + camRight * halfNearWidth;
    glm::vec3 nearBL = nearCenter - camUp * halfNearHeight - camRight * halfNearWidth;
    glm::vec3 nearBR = nearCenter - camUp * halfNearHeight + camRight * halfNearWidth;

    glm::vec3 farTL  = farCenter + camUp * halfFarHeight - camRight * halfFarWidth;
    glm::vec3 farTR  = farCenter + camUp * halfFarHeight + camRight * halfFarWidth;
    glm::vec3 farBL  = farCenter - camUp * halfFarHeight - camRight * halfFarWidth;
    glm::vec3 farBR  = farCenter - camUp * halfFarHeight + camRight * halfFarWidth;

    glm::vec3 color = glm::vec3(1.0f);
    // Vertex data
    GLfloat vertices[] =
    {
        // near
        nearTL.x, nearTL.y, -nearPlane, color.r, color.g, color.b,
        nearBL.x, nearBL.y, -nearPlane, color.r, color.g, color.b,
        nearBR.x, nearBR.y, -nearPlane, color.r, color.g, color.b,
        nearTR.x, nearTR.y, -nearPlane, color.r, color.g, color.b,

        // Far
        farTL.x, farTL.y, -farPlane, color.r, color.g, color.b,
        farBL.x, farBL.y, -farPlane, color.r, color.g, color.b,
        farBR.x, farBR.y, -farPlane, color.r, color.g, color.b,
        farTR.x, farTR.y, -farPlane, color.r, color.g, color.b
    };

    vertexBuffer->SetData(vertices, sizeof(vertices));

    Renderer::DrawLines(shader, vertexArray, transform);
}

void Camera::RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up)
{
    pos = position;
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& euler)
{
    float pitch = euler.x;
    float yaw = -90.0f - euler.y;

    // Calculate the orthonormal basis for the game camera based
    // on it's transform
    glm::vec3 front, up, right;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, front));

    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::SetClearFlag(const std::string currentFlag)
{
    if (str2ClearFlagMap.find(currentFlag) != str2ClearFlagMap.end())
    {
        clearFlag = static_cast<ClearFlag>(str2ClearFlagMap.at(currentFlag));
    }
}

const std::string& Camera::GetClearFlag()
{
    if (ClearFlag2StrMap.find(clearFlag) != ClearFlag2StrMap.end())
    {
        return ClearFlag2StrMap.at(clearFlag);
    }
}

void Camera::SetProjection()
{
    if (isPerspective)
        SetPerspectiveProjection();
    else
        SetOrthographicProjection();

    // Always keep UI projMatrix in ortho
    uiProjMatrix = glm::ortho(
        0.0f,
        width,
        0.0f,
        height
    );
}

void Camera::SetProjection(uint32_t newWidth, uint32_t newHeight)
{
    width = newWidth;
    height = newHeight;

    if (isPerspective)
        SetPerspectiveProjection();
    else
        SetOrthographicProjection();

    // Always keep UI projMatrix in ortho
    uiProjMatrix = glm::ortho(
        0.0f,
        (float)newWidth,
        0.0f,
        (float)newHeight
    );
}

void Camera::SetPerspectiveProjection()
{
    projMatrix =
        glm::perspective(
            glm::radians(vFov), // actually the vertical FOV
            (width / height), // aspect ratio
            nearPlane,
            farPlane
        )
    ;
}

void Camera::SetOrthographicProjection()
{
    float aspectRatio = (width / height);
    projMatrix =
        glm::ortho(
            -0.5f * aspectRatio * size, // left
             0.5f * aspectRatio * size, // right
            -0.5f * size, // bottom
             0.5f * size, // top
             // Note: setting it to nearPlane
             // seems to mess up ImGuizmo, not sure why
            -farPlane, // near
             farPlane  // far
        )
    ;
}
