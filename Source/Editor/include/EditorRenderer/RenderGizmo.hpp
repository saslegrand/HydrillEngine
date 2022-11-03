#pragma once

#include <vector>

#include <Renderer/RenderPipeline.hpp>
#include <Renderer/Primitives/VertexArray.hpp>
#include <Renderer/Primitives/ShaderProgram.hpp>
#include <Types/Color.hpp>
#include <Maths/Matrix4.hpp>

class Transform;
class Camera;

const Color4 guizmoColor = Color4(Color3::LightGray, 0.75f);

class RenderGizmo : public RenderPipeline
{

//	Variables

private:

	unsigned int m_lightGizmoTexture = 0;
	unsigned int m_cameraGizmoTexture = 0;
	unsigned int m_particleGizmoTexture = 0;
	unsigned int m_soundEmitterGizmoTexture = 0;

	VertexArray m_rectVAO;

	ShaderProgram m_shader;
	Matrix4 m_billboardRotationMatrix;

public:
	
	Camera* selectedCamera = nullptr;

//	Functions

private:

	/**
	@brief Draw a sprite gizmo, given a transform, a texture and an optional color

	@param transform : transform of the object we want to render gizmo
	@param texture : icon texture ID of the gizmo
	@param color : color of the gizmo (it is recommanded to use a white image if you use color)
	*/
	void DrawGizmoSprite(const Transform& transform, unsigned int texture, const Color4& color = guizmoColor);


public:

	/**
	@brief Initialize shader and rect vao
	*/
	void Initialize() override;

	/**
	@brief Redner all components with their corresponding gizmo icons
	*/
	void Render() override;
};