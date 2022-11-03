#pragma once

#include <unordered_map>

#include "Renderer/Primitives/GLPrimitive.hpp"
#include "Resources/Resource/Shader.hpp"

struct GLUniform
{
	int			 location;
	unsigned int type;
};

struct ENGINE_API ShaderProgram : public GLPrimitive
{
private:
	// Disable std warning
#pragma warning(disable:4251)
	std::string m_name;

	std::unordered_map<std::string, GLUniform> m_uniforms;
#pragma warning(default:4251)

	Shader fragment;
	Shader vertex;
	Shader geometry;

	void CreateUniforms();

public:
	ShaderProgram();
	ShaderProgram(const ShaderProgram& cpy) = delete;
	ShaderProgram(ShaderProgram&& cpy) noexcept;

	ShaderProgram& operator=(const ShaderProgram& cpy) = delete;
	ShaderProgram& operator=(ShaderProgram&& cpy) noexcept;

	~ShaderProgram();

	void Generate(const char* vertexShader, const char* fragmentShader, const char* geomertyShader = nullptr);
	void Bind() const;
	void Unbind() const;
	void SendUniform(const std::string& uniformName, const void* data, int count = 1, bool transpose = false) const;

	int GetLocationFromUniformName(const std::string& uniformName);

	const std::string& GetName() const;
};