#include <Shader.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace
{
    std::string readSource(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open shader file: " + path);

        std::string source{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        if (file.bad())
            throw std::runtime_error("Cannot read shader file: " + path);

        if (source.size() >= 3 &&
            static_cast<unsigned char>(source[0]) == 0xEF &&
            static_cast<unsigned char>(source[1]) == 0xBB &&
            static_cast<unsigned char>(source[2]) == 0xBF)
        {
            source.erase(0, 3);
        }

        return source;
    }

    GLuint compileStage(GLenum type, const std::string& path)
    {
        std::string source = readSource(path);
        const char* text = source.c_str();
        GLuint shader = glCreateShader(type);
        if (shader == 0)
            throw std::runtime_error("Cannot create shader: " + path);

        glShaderSource(shader, 1, &text, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_TRUE)
            return shader;

        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, &log[0]);
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed: " + path + "\n" + log);
    }
}

Shader::Shader(
    const std::string& vertexPath,
    const std::string& fragmentPath,
    const std::string& geometryPath
)
    : id(0),
      vertexPath(vertexPath),
      fragmentPath(fragmentPath),
      geometryPath(geometryPath)
{
    if (!reload())
        throw std::runtime_error("Initial shader program creation failed.");
}

Shader::~Shader()
{
    if (id != 0)
        glDeleteProgram(id);
}

Shader::Shader(Shader&& other) noexcept
    : id(other.id),
      vertexPath(std::move(other.vertexPath)),
      fragmentPath(std::move(other.fragmentPath)),
      geometryPath(std::move(other.geometryPath))
{
    other.id = 0;
}

Shader& 
Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (id != 0)
            glDeleteProgram(id);

        id = other.id;
        vertexPath = std::move(other.vertexPath);
        fragmentPath = std::move(other.fragmentPath);
        geometryPath = std::move(other.geometryPath);
        other.id = 0;
    }

    return *this;
}

void 
Shader::use() const
{
    glUseProgram(id);
}

void 
Shader::setVec3(const char* name, const Vec3& vector) const
{
    use();
    GLint location = glGetUniformLocation(id, name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void 
Shader::setMat4(const char* name, const Mat4& matrix) const
{
    use();
    GLint location = glGetUniformLocation(id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.m);
}

void 
Shader::setInt(const char* name, int value) const
{
    use();
    GLint location = glGetUniformLocation(id, name);
    glUniform1i(location, value);
}

bool 
Shader::reload()
{
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint geometry = 0;
    GLuint candidate = 0;

    try
    {
        vertex = compileStage(GL_VERTEX_SHADER, vertexPath);
        fragment = compileStage(GL_FRAGMENT_SHADER, fragmentPath);

        if (!geometryPath.empty())
            geometry = compileStage(GL_GEOMETRY_SHADER, geometryPath);

        candidate = glCreateProgram();
        if (candidate == 0)
            throw std::runtime_error("Cannot create shader program.");

        glAttachShader(candidate, vertex);
        glAttachShader(candidate, fragment);
        if (geometry != 0)
            glAttachShader(candidate, geometry);
        glLinkProgram(candidate);

        GLint success = GL_FALSE;
        glGetProgramiv(candidate, GL_LINK_STATUS, &success);
        if (success != GL_TRUE)
        {
            GLint length = 0;
            glGetProgramiv(candidate, GL_INFO_LOG_LENGTH, &length);
            std::string log(length > 0 ? length : 1, '\0');
            glGetProgramInfoLog(candidate, static_cast<GLsizei>(log.size()), nullptr, &log[0]);
            throw std::runtime_error("Shader program link failed:\n" + log);
        }
    }
    catch (const std::exception& error)
    {
        if (candidate != 0)
            glDeleteProgram(candidate);
        if (vertex != 0)
            glDeleteShader(vertex);
        if (fragment != 0)
            glDeleteShader(fragment);
        if (geometry != 0)
            glDeleteShader(geometry);

        std::cerr << error.what() << std::endl;
        return false;
    }

    glDetachShader(candidate, vertex);
    glDetachShader(candidate, fragment);
    if (geometry != 0)
        glDetachShader(candidate, geometry);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry != 0)
        glDeleteShader(geometry);

    GLuint previous = id;
    id = candidate;
    use();
    if (previous != 0)
        glDeleteProgram(previous);

    std::cout << "Shader loaded: " << vertexPath
              << " + " << fragmentPath << std::endl;
    return true;
}
