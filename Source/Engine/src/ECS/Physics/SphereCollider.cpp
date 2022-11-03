#include "ECS/GameObject.hpp"

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"

#include "Tools/DrawDebug.hpp"

#include "Generated/SphereCollider.rfks.h"

rfk::UniquePtr<SphereCollider> SphereCollider::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<SphereCollider>(GO, uid);
}

SphereCollider::SphereCollider(GameObject& gameObject, const HYGUID& uid)
	: Collider(gameObject, staticGetArchetype().getName(), uid)
{
	m_geometry = PxSphereGeometry(m_radius);
	//EngineContext::Instance().physicsSystem->Register(*this);
}

void SphereCollider::PrepareGeometry()
{
	SetRadius(m_radius);
}

void SphereCollider::PrepareForSimulation()
{
	Collider::PrepareForSimulation();
}

const PxGeometry& SphereCollider::GetGeometryShape() const
{
	return m_geometry;
}

float SphereCollider::GetRadius() const
{
	return m_radius;
}

void SphereCollider::SetRadius(float value)
{
	if (value < 0)
		return; //TODO : Log that radius can't be negative

	Vector3 scale = gameObject.transform.Scale();
	m_radius = value;
	m_geometry.radius = m_radius * Maths::Max(scale.x, Maths::Max(scale.y, scale.z));
}

void SphereCollider::DrawDebug()
{
	DrawDebug::DrawSphere(gameObject.transform.GetWorldMatrix() * Matrix4::Translate(m_center),
							m_radius,
							3.0f, Color3::LightGreen);
}