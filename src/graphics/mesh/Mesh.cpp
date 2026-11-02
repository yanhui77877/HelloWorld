#include <Mesh.h>

#include <cstddef>
#include <limits>
#include <stdexcept>

Mesh::Mesh(const std::vector<Vertex>& vertices)
    : vertexBuffer(GL_ARRAY_BUFFER),
      vertexCount(0)
{
    if (vertices.empty() || vertices.size()%3 != 0)
        throw std::runtime_error("Mesh requires complete triangles");

    if (vertices.size() > static_cast<std::size_t>((std::numeric_limits<GLsizei>::max)()))
        throw std::runtime_error("Mesh has too many vertices");

    vertexCount = static_cast<GLsizei>(vertices.size());

    vertexArray.bind();
    vertexBuffer.upload(vertices.size()*sizeof(Vertex), vertices.data());
    vertexArray.setAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position));
    vertexArray.setAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, u));
    vertexArray.setAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    VertexArray::unbind();
}

void
Mesh::draw() const
{
    vertexArray.bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    VertexArray::unbind();
}
