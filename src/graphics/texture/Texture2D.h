#pragma once

#include <glad/glad.h>
#include <string>

class Texture2D
{
    GLuint id;
    int width;
    int height;

public:
    enum class ColorSpace
    {
        Linear,
        SRGB
    };

    explicit Texture2D(const std::string& path, 
                       bool flipVertically = true,
                       ColorSpace colorSpace = ColorSpace::Linear);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    void 
    bind(GLuint unit = 0) const;
    void 
    setFiltering(GLenum minFilter, GLenum magFilter) const;
    void 
    setWrapping(GLenum wrapS, GLenum wrapT) const;

    int getWidth() const;
    int getHeight() const;
};
