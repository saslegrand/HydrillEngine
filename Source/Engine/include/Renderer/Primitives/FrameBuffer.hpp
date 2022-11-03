#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"
#include "Maths/Vector2.hpp"

#include <glad/gl.h>

struct ENGINE_API FrameBuffer : public GLPrimitive
{
public:

	enum class InternalFormat
	{
		RGB = 0x881B,
		RGBA = 0x881A
	};

	enum class Format
	{
		RGB = 0x1907,		// Equivalent to GL_RGB
		RGBA = 0x1908		// Equivalent to GL_RGBA
	};

private:

	unsigned int m_intermediateCaptureFrameBuffer = 0;
	unsigned int m_multiSampleTextureBuffer = 0;
	unsigned int m_resultTextureBuffer = 0;
	unsigned int m_captureRenderBuffer = 0;
	unsigned int m_textureInternalFormat = 0;
	unsigned int m_textureFormat = 0;

	int m_previousFB = 0;

	Vector2 m_dimensions = { 1920.f, 1080.f };
	bool    m_resized = true;

public:

	FrameBuffer(InternalFormat internalTextureFormat = InternalFormat::RGB, Format textureFormat = Format::RGB);
	FrameBuffer(const FrameBuffer& cpy) = delete;
	FrameBuffer(FrameBuffer&& cpy) noexcept;

	FrameBuffer& operator=(const FrameBuffer& cpy) = delete;
	FrameBuffer& operator=(FrameBuffer&& cpy) noexcept;

	~FrameBuffer();

	void AttachNewTexture(unsigned int textureID);

	/**
	@brief Create and initialize the framebuffer
	*/
	void Create();

	/**
	@brief Bind framebuffer (Render in framebuffer by calling draw functions between bind and unbind functions)
	*/
	void Bind() const;

	/**
	@brief Must be placed after drawing inside the framebuffer
	*/
	void EndDraw() const;

	/**
	@brief Unbind framebuffer
	*/
	void Unbind() const;

	/**
	* @brief Update the texture and render buffer size if it was resized
	*/
	void Update();

	/**
	* @brief Resize texture and render buffer
	*/
	void ApplySize();

	/**
	* @brief Get Width
	*/
	float GetWidth() const;

	/**
	* @brief Get Height
	*/
	float GetHeight() const;

	/**
	* @brief Get Dimensions
	*/
	Vector2 GetDimensions() const;

	/**
	* @brief Set new dimensions of the buffer
	*
	* @parame width : new width
	* @parame height : new height
	*/
	void SetDimensions(const float width, const float height);

	/**
	* @brief Set new dimensions of the buffer
	*
	* @param dimensions : new dimensions
	*/
	void SetDimensions(const Vector2 dimensions);

	/**
	* @brief Get result of the framebuffer
	*
	* @return image texture ID that was rendered
	*/
	unsigned int GetImage() const;

	/**
	* @brief Was the framebuffer resized ?
	*
	* @return Wether it was resized or not
	*/
	bool WasResized() const;
};
