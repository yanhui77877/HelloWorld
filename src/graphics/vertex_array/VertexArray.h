#pragma once

#include <cstddef>
#include <glad/glad.h>

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind() const;
    static void unbind();

    void setAttribute(
        GLuint index,
        GLint componentCount,
        GLenum type,
        GLboolean normalized,
        GLsizei stride,
        std::size_t offset
    ) const;

private:
    GLuint id;
};
