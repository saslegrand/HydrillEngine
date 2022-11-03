#include "Renderer/Primitives/ShaderProgram.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"

ShaderProgram::ShaderProgram()
{
	m_ID = glCreateProgram();
}

ShaderProgram::ShaderProgram(ShaderProgram&& cpy) noexcept
{
	m_ID = cpy.m_ID;
	vertex = std::move(cpy.vertex);
	fragment = std::move(cpy.fragment);
	m_uniforms = cpy.m_uniforms;
	m_name = cpy.m_name;

	cpy.m_ID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& cpy) noexcept
{
	this->~ShaderProgram();

	m_ID = cpy.m_ID;
	vertex = std::move(cpy.vertex);
	fragment = std::move(cpy.fragment);
	m_uniforms = cpy.m_uniforms;
	m_name = cpy.m_name;

	cpy.m_ID = 0;

	return *this;
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_ID);
}

void ShaderProgram::Generate(const char* vertexShader, const char* fragmentShader, const char* geomertyShader)
{
	// TODO: Get shader from ID
	vertex.Generate(vertexShader, GL_VERTEX_SHADER);
	fragment.Generate(fragmentShader, GL_FRAGMENT_SHADER);
	if(geomertyShader) geometry.Generate(geomertyShader, GL_GEOMETRY_SHADER);


	// Attach shaders to the program
	glAttachShader(m_ID, vertex.ID);
	glAttachShader(m_ID, fragment.ID);
	if (geomertyShader) glAttachShader(m_ID, geometry.ID);

	// Link the program
	glLinkProgram(m_ID);

	// Check for link error(s)
	GLint success;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		const GLsizei infoSize = 256;
		GLchar infoLog[infoSize];
		glGetProgramInfoLog(m_ID, infoSize, NULL, infoLog);
		Logger::Warning("ShaderProgram - " + m_name + " linking failed: " + std::string(infoLog));

		return;
	}

	CreateUniforms();
}

void ShaderProgram::Bind() const
{
	glUseProgram(m_ID);
}

void ShaderProgram::Unbind() const
{
	glUseProgram(0);
}

void ShaderProgram::CreateUniforms()
{
	// Get the active uniforms count
	GLint uniformCount;
	glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &uniformCount);

	// Loop over the active uniforms
	const GLsizei bufSize = 256;
	for (GLint i = 0; i < uniformCount; i++)
	{
		GLsizei nameLength;
		GLint size;             // Size of the variable
		GLenum type;            // Type of the variable
		GLchar uniName[bufSize];

		// Get the current uniform informations
		glGetActiveUniform(m_ID, i, bufSize, &nameLength, &size, &type, uniName);

		// Get the uniform location and check if valid
		GLint location = glGetUniformLocation(m_ID, uniName);

		if (location < 0) continue;

		// Add the new uniform to the program
		m_uniforms[uniName] = { location, type };
	}
}

void ShaderProgram::SendUniform(const std::string& uniformName, const void* data, int count, bool transpose) const
{
	// Check if the uniform exists
	auto found = m_uniforms.find(uniformName);
	if (found == m_uniforms.end())
	{
		Logger::Error("ShaderProgram - Uniform named " + uniformName + " does not exist");
		return;
	}

	// Send the uniform from its type
	const GLUniform& uniform = found->second;
	switch (uniform.type)
	{
	case GL_INT:
	case GL_BOOL:
		glUniform1iv(uniform.location, count, (GLint*)data);
		break;

	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE:
		glUniform1iv(uniform.location, count, (GLint*)data);
		break;

	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		glUniform2iv(uniform.location, count, (GLint*)data);
		break;

	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		glUniform3iv(uniform.location, count, (GLint*)data);
		break;

	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		glUniform4iv(uniform.location, count, (GLint*)data);
		break;

	case GL_FLOAT:
		glUniform1fv(uniform.location, count, (GLfloat*)data);
		break;

	case GL_FLOAT_VEC2:
		glUniform2fv(uniform.location, count, (GLfloat*)data);
		break;

	case GL_FLOAT_VEC3:
		glUniform3fv(uniform.location, count, (GLfloat*)data);
		break;

	case GL_FLOAT_VEC4:
		glUniform4fv(uniform.location, count, (GLfloat*)data);
		break;

	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(uniform.location, count, transpose, (GLfloat*)data);
		break;

	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(uniform.location, count, transpose, (GLfloat*)data);
		break;

	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(uniform.location, count, transpose, (GLfloat*)data);
		break;

	default:
		// If the type is not supported, log this message
		Logger::Error("ShaderProgram - Uniform type " + std::to_string(uniform.type) + " not supported");
		break;
	}
}

int ShaderProgram::GetLocationFromUniformName(const std::string& uniformName)
{
	// Check if the uniform exists
	auto found = m_uniforms.find(uniformName);
	if (found == m_uniforms.end())
	{
		Logger::Error("ShaderProgram - Uniform named " + uniformName + " does not exist");
		return -1;
	}

	return found->second.location;
}

const std::string& ShaderProgram::GetName() const
{
	return m_name;
}