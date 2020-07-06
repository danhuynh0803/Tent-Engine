#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include <stdint.h>
#include "Renderer/Buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetLayout(const BufferLayout& _layout) override { layout = _layout; }
    virtual const BufferLayout& GetLayout() const override { return layout; }

private:
    uint32_t renderID;
    BufferLayout layout;
};


class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t _count);
    virtual ~OpenGLIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual uint32_t GetCount() const override;

private:
    uint32_t renderID;
    uint32_t count;
};

class OpenGLUniformBuffer : public UniformBuffer
{
public:
    OpenGLUniformBuffer();
    virtual ~OpenGLUniformBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetLayout(const BufferLayout& _layout, uint32_t blockIndex) override;

private:
    uint32_t renderID;
    BufferLayout layout;
};

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer(uint32_t width, uint32_t height);
    virtual ~OpenGLFrameBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

private:
    uint32_t renderID;
    uint32_t textureID;
    //SharedPtr<Texture> texture;
};

#endif // OPENGL_BUFFER_H
