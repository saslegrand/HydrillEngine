#pragma once

#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"

class RenderUtils
{

//	Variables

private:

	ShaderProgram m_imageScreenShader;

public:

	VertexArray   screenRect;

//	Functions

public:

	void Initialize();
	void DrawImageScreen(unsigned int image);
	void DrawRect();
};