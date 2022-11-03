#include <Maths/Vector3.hpp>
#include "Maths/Matrix3.hpp"
#include "ECS/TransformData.hpp"
#include "Renderer/RenderSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Core/Time.hpp"
#include "EngineContext.hpp"
#include "Maths/Maths.hpp"

#include "Generated/Camera.rfks.h"


Camera::Camera(const float in_FOV, const float in_near, const float in_far, const float in_aspect, const Vector3& in_position, const Vector3& in_rotation)
{
	m_FOV = in_FOV;
	m_near = in_near;
	m_far = in_far;
	m_aspect = in_aspect;
	m_position = in_position;

	ComputeProjectionMatrix();
	ComputeViewMatrix(in_position, Quaternion::FromEulerRad(in_rotation * Maths::DEGTORAD));
}


void Camera::Update()
{
	ComputeProjectionMatrix();
}


void Camera::UpdateExposure()
{
	const GraphicsSettings& param = SystemManager::GetRenderSystem().graphicsSettings;

	if (!param.autoExposure)
	{
		m_exposure = param.constantExposure;
		return;
	}

	//	Compute exposure

	const float step = param.exposureSpeed * EngineContext::Instance().time->GetUnscaledDeltaTime();
	float brightness = Maths::Max(m_brightness * param.exposureMultiplier, 0.001f);
	const float target = Maths::Clamp(0.5f / brightness, param.minExposure, param.maxExposure);

	m_exposure = Maths::Lerp(m_exposure, target, step);
	m_exposure = Maths::Clamp(m_exposure, param.minExposure, param.maxExposure);
}

void Camera::SetTransform(const Vector3& position, const Quaternion& rotation)
{
	m_position = position;
	m_rotation = rotation;
	ComputeViewMatrix(position, rotation);
}

void Camera::ComputeProjectionMatrix()
{
	m_projMatrix = Matrix4::Perspective(m_FOV * Maths::DEGTORAD, m_aspect, m_near, m_far);
	m_recomputeProjMatrix = false;
}


void Camera::ComputeViewMatrix(const Vector3& in_position, const Quaternion& in_rotation)
{
	m_viewMatrix  = Quaternion::ToMatrix(Quaternion::Invert(in_rotation));
	m_viewMatrix *= Matrix4::Translate(-in_position);
}


float Camera::GetFOV() const
{ 
	return m_FOV; 
}

float Camera::GetNear() const
{ 
	return m_near; 
}

float Camera::GetFar() const
{ 
	return m_far; 
}

float Camera::GetAspect() const
{ 
	return m_aspect; 
}

Matrix4 Camera::GetProjectionMatrix() const
{
	return m_projMatrix;
}

Matrix4 Camera::GetViewMatrix() const
{ 
	return m_viewMatrix;
}

Matrix4 Camera::GetTransformMatrix() const
{
	return Matrix4::Translate(m_position) * Quaternion::ToMatrix(m_rotation);
}

Vector3 Camera::GetPosition() const
{
	return m_position;
}

Quaternion Camera::GetRotation() const
{
	return m_rotation;
}

float Camera::GetCurrentExposure() const
{
	return m_exposure;
}

void Camera::SetFOV(const float FOV) 
{
	m_recomputeProjMatrix = true;
	m_FOV = FOV;
}

void Camera::SetNear(const float near)
{
	m_recomputeProjMatrix = true;
	m_near = near;
}

void Camera::SetFar(const float far)
{
	m_recomputeProjMatrix = true;
	m_far = far;
}

void Camera::SetAspect(const float aspect)
{
	m_recomputeProjMatrix = true;
	m_aspect = aspect;
}

void Camera::SetAspect(const float width, const float height)
{
	m_recomputeProjMatrix = true;
	m_aspect = width / height; 
}

void Camera::SetAspect(const Vector2 m_dimensions)
{
	m_recomputeProjMatrix = true;
	m_aspect = m_dimensions.x / m_dimensions.y;
}


void Camera::SetCurrentBrightness(float brightness)
{
	m_brightness = brightness;
}


Quaternion Camera::GetBillboardRotation()  const
{	
	return Quaternion::FromMatrix(TransformData::RotationMatrix(m_viewMatrix));
}