#include "Resources/Resource/Shader.hpp"

#include <fstream>
#include <glad/gl.h>

#include "Core/Logger.hpp"
#include "Tools/StringHelper.hpp"


Shader::Shader(Shader&& sMove) noexcept
{
    ID = sMove.ID;

    sMove.ID = 0;
}

Shader& Shader::operator=(Shader&& sMove) noexcept
{
    this->~Shader();

    ID = sMove.ID;
    sMove.ID = 0;

    return *this;
}

Shader::~Shader()
{
    // Delete generated shader
    glDeleteShader(ID);
}

bool Shader::Create(const std::string& filePath, std::string& shaderStr)
{
    std::ifstream fileStream(filePath);
    if (fileStream.fail())
    {
        Logger::Error("Shader - File " + filePath + " cannot be open or not exists");
        fileStream.close();
        return false;
    }

    const std::string extension = StringHelper::GetFileExtensionFromPath(filePath);
    if (extension.compare(".vert") == 0)
        ID = glCreateShader(GL_VERTEX_SHADER);
    else if (extension.compare(".frag") == 0)
        ID = glCreateShader(GL_FRAGMENT_SHADER);
    else if (extension.compare(".geom") == 0)
        ID = glCreateShader(GL_GEOMETRY_SHADER);
    else
    {
        Logger::Error("Shader - File " + filePath + " extension is not valid or not supported");
        fileStream.close();
        return false;
    }

    // Copy file into a string
    std::string line;
    while (std::getline(fileStream, line))
        shaderStr += line + '\n';

    fileStream.close();

    return true;
}


void Shader::Generate(const char* content, unsigned int type)
{
    ID = glCreateShader(type);

    glShaderSource(ID, 1, &content, NULL);
    // Compile the shader
    glCompileShader(ID);

    // Check if the compilation has succeeded
    GLint success;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        const GLsizei infoSize = 256;
        GLchar infoLog[infoSize];
        glGetShaderInfoLog(ID, infoSize, NULL, infoLog);
        Logger::Error("Shader - Filepath.ext TODO shader compilation failed" + std::string(infoLog));

    };
}
