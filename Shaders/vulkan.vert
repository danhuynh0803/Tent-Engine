#version 450

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;

// =========================================
layout (location = 0) out vec3 outColor;

// =========================================
layout (binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camera;

layout (binding = 1) uniform Transform
{
    mat4 model;
} transform;

// =========================================
void main()
{
    gl_Position = camera.viewProj * transform.model * vec4(aPos, 1.0);

    outColor = aColor;
}