#include "ECS/GameObject.hpp"

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"

#include "Tools/DrawDebug.hpp"

#include "Generated/CapsuleCollider.rfks.h"

rfk::UniquePtr<CapsuleCollider> CapsuleCollider::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<CapsuleCollider>(GO, uid);
}

CapsuleCollider::CapsuleCollider(GameObject& gameObject, const HYGUID& uid)
	: Collider(gameObject, staticGetArchetype().getName(), uid)
{
	m_geometry = PxCapsuleGeometry(m_radius, m_halfHeight);
	//EngineContext::Instance().physicsSystem->Register(*this);
}

void CapsuleCollider::PrepareGeometry()
{
	SetRadius(m_radius);
	SetHalfHeight(m_halfHeight);
}

void CapsuleCollider::PrepareForSimulation()
{
	Collider::PrepareForSimulation();

	if (m_PxShape != nullptr)
	{
		PxTransform transform = m_PxShape->getLocalPose();
		PxTransform relativeRot = PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		m_PxShape->setLocalPose(transform * relativeRot);
	}
}

const PxGeometry& CapsuleCollider::GetGeometryShape() const
{
	return m_geometry;
}

float CapsuleCollider::GetRadius() const
{
	return m_radius;
}

void CapsuleCollider::SetRadius(float value)
{
	if (value < 0)
		return; //TODO : Log that radius can't be negative

	Vector3 scale = gameObject.transform.Scale();
	m_radius = value;
	m_geometry.radius = m_radius * Maths::Max(scale.x, scale.z);
}

float CapsuleCollider::GetHalfHeight() const
{
	return m_halfHeight;
}

void CapsuleCollider::SetHalfHeight(float value)
{
	if (value < 0)
		return; //TODO : Log that radius can't be negative

	m_halfHeight = value;
	m_geometry.halfHeight = m_halfHeight * gameObject.transform.Scale().y;
}

void CapsuleCollider::DrawDebug()
{
	DrawDebug::DrawCapsule(gameObject.transform.GetWorldMatrix() * Matrix4::Translate(m_center),
							m_radius,
							m_halfHeight * 2,
							3.0f, Color3::LightGreen);
}