#pragma once

#include <glad/gl.h>

#include "Renderer/RenderUtils.hpp"


#pragma region ImageScreenShader
static const char* imageScreenVertexShader = R"GLSL(
#version 450 core

//  Attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

//  Varyings
out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
})GLSL";

static const char* imageScreenFragmentShader = R"GLSL(
#version 450 core

layout (location = 0) out vec3 oColor;

//  Uniforms
uniform sampler2D uScreenTexture;

//  Varyings
in vec2 vUV;

void main()
{
	oColor = texture(uScreenTexture, vUV).rgb;
})GLSL";

#pragma endregion

void RenderUtils::Initialize()
{
	m_imageScreenShader.Generate(imageScreenVertexShader, imageScreenFragmentShader);

	glUniform1i(glGetUniformLocation(m_imageScreenShader.GetID(), "uScreenTexture"), 0);

	screenRect.Bind();

	{
		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		GLuint quadVBO;

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	screenRect.Unbind();
}

void RenderUtils::DrawImageScreen(unsigned int image)
{
	screenRect.Bind();
	glBindTextureUnit(0, image);
	m_imageScreenShader.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_imageScreenShader.Unbind();
	glBindTextureUnit(0, 0);
	screenRect.Unbind();

}

void RenderUtils::DrawRect()
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}