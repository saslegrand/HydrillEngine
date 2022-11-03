#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"

class Skybox;

struct ENGINE_API GLCubeMap : public GLPrimitive
{
	GLCubeMap();
	GLCubeMap(const GLCubeMap& cpy) = delete;
	GLCubeMap(GLCubeMap&& cpy) noexcept;

	GLCubeMap& operator=(const GLCubeMap& cpy) = delete;
	GLCubeMap& operator=(GLCubeMap&& cpy) noexcept;

	~GLCubeMap();

	void Bind() const;
	void BindUnit(int unit) const;
	void Unbind() const;
	void UnbindUnit(int unit) const;

	void Generate(Skybox& skybox);
};