#include <Texture2D.h>
#include <stb_image.h>

#include <memory>
#include <stdexcept>
#include <utility>

Texture2D::Texture2D(const std::string& path, 
                     bool flipVertically)
    : id(0), 
      width(0), 
      height(0)
{
    int channelsInFile = 0;
    stbi_set_flip_vertically_on_load(flipVertically);

    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> pixels{
        stbi_load(path.c_str(), &width, &height, &channelsInFile, STBI_rgb_alpha),
        &stbi_image_free
    };

    if (!pixels)
    {
        const char* reason = stbi_failure_reason();
        throw std::runtime_error(
            "Cannot load texture: " + path + "\n" +
            (reason != nullptr ? reason : "Unknown image error")
        );
    }

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if (width > maxTextureSize || height > maxTextureSize)
        throw std::runtime_error("Image exceeds GL_MAX_TEXTURE_SIZE: " + path);

    glGenTextures(1, &id);
    if (id == 0)
        throw std::runtime_error("Cannot create texture: " + path);

    bind();
    setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    setWrapping(GL_REPEAT, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels.get()
    );
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture2D::~Texture2D()
{
    if (id != 0)
        glDeleteTextures(1, &id);
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : id(std::exchange(other.id, 0)),
      width(std::exchange(other.width, 0)),
      height(std::exchange(other.height, 0))
{}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this != &other)
    {
        if (id != 0)
            glDeleteTextures(1, &id);

        id = std::exchange(other.id, 0);
        width = std::exchange(other.width, 0);
        height = std::exchange(other.height, 0);
    }

    return *this;
}

void Texture2D::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::setFiltering(GLenum minFilter, GLenum magFilter) const
{
    if (magFilter != GL_NEAREST && magFilter != GL_LINEAR)
        throw std::invalid_argument("Texture magnification requires GL_NEAREST or GL_LINEAR.");

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void 
Texture2D::setWrapping(GLenum wrapS, GLenum wrapT) const
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

int 
Texture2D::getWidth() const
{
    return width;
}

int 
Texture2D::getHeight() const
{
    return height;
}
