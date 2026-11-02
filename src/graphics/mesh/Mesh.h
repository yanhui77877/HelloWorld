#pragma once

#include <Buffer.h>
#include <VertexArray.h>
#include <Vertex.h>

#include <vector>

class Mesh
{
public:
    explicit Mesh(const std::vector<Vertex>& vertices);

    void draw() const;

private:
    Buffer vertexBuffer;
    VertexArray vertexArray;
    GLsizei vertexCount;
};
