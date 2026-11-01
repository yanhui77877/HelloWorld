#pragma once

#include <glad/glad.h>

class Buffer
{
public:
    explicit Buffer(GLenum target);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    void bind() const;
    void upload(
        GLsizeiptr size,
        const void* data,
        GLenum usage = GL_STATIC_DRAW
    ) const;

private:
    GLuint id;
    GLenum target;
};
