#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Base.h"
#include "Core/Layer.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Renderer/Framebuffer.h"

class RenderLayer : public Layer
{
public:
    RenderLayer();
    ~RenderLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

private:
    SharedPtr<UniformBuffer> ubo;
    SharedPtr<Framebuffer> colorFB;
};

#endif // RENDER_LAYER_H
