#include <VertexArray.h>

VertexArray::VertexArray()
    : id(0)
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
    if (id != 0)
        glDeleteVertexArrays(1, &id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : id(other.id)
{
    other.id = 0;
}

VertexArray& 
VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        if (id != 0)
            glDeleteVertexArrays(1, &id);

        id = other.id;
        other.id = 0;
    }

    return *this;
}

void VertexArray::bind() const
{
    glBindVertexArray(id);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::setAttribute(
    GLuint index,
    GLint componentCount,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    std::size_t offset
) const
{
    bind();
    glVertexAttribPointer(
        index, componentCount, type, normalized, stride,
        reinterpret_cast<const void*>(offset)
    );
    glEnableVertexAttribArray(index);
}
