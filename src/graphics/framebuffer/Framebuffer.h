#pragma once

#include <glad/glad.h>

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    void resize(int width, int height);
    void bind() const;
    static void bindDefault();
    void bindColorTexture(GLuint unit = 0) const;

private:
    GLuint id = 0;
    GLuint colorTexture = 0;
    GLuint depthBuffer = 0;
    int width = 0;
    int height = 0;
};
