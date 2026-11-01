#pragma once

#include <glad/glad.h>
#include <GraphicsMath.h>
#include <string>

class Shader
{
public:
    Shader(
        const std::string& vertexPath,
        const std::string& fragmentPath,
        const std::string& geometryPath = {}
    );
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void 
    use() const;

    void 
    setVec3(const char* name, const Vec3& vector) const;

    void 
    setMat4(const char* name, const Mat4& matrix) const;

    void 
    setInt(const char* name, int value) const;

    bool 
    reload();

private:
    GLuint id;
    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;
};
