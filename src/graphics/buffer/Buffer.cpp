#include <Buffer.h>

Buffer::Buffer(GLenum target)
    : id(0), 
      target(target)
{
    glGenBuffers(1, &id);
}

Buffer::~Buffer()
{
    if (id != 0)
        glDeleteBuffers(1, &id);
}

Buffer::Buffer(Buffer&& other) noexcept
    : id(other.id), 
      target(other.target)
{
    other.id = 0;
}

Buffer& 
Buffer::operator=(Buffer&& other) noexcept
{
    if (this != &other)
    {
        if (id != 0)
            glDeleteBuffers(1, &id);

        id = other.id;
        target = other.target;
        other.id = 0;
    }

    return *this;
}

void 
Buffer::bind() const
{
    glBindBuffer(target, id);
}

void 
Buffer::upload(GLsizeiptr size, 
               const void* data, 
               GLenum usage) const
{
    bind();
    glBufferData(target, size, data, usage);
}
