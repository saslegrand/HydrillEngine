#include "ECS/GameObject.hpp"

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"

#include "Tools/DrawDebug.hpp"

#include "Generated/BoxCollider.rfks.h"

rfk::UniquePtr<BoxCollider> BoxCollider::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<BoxCollider>(GO, uid);
}

BoxCollider::BoxCollider(GameObject& gameObject, const HYGUID& uid)
	: Collider(gameObject, staticGetArchetype().getName(), uid)
{
	m_geometry = PxBoxGeometry(m_halfDimensions.x, m_halfDimensions.y, m_halfDimensions.z);
	SetHalfDimensions(m_halfDimensions);

	//EngineContext::Instance().physicsSystem->Register(*this);
}

void BoxCollider::PrepareGeometry()
{
	SetHalfDimensions(m_halfDimensions);
}

void BoxCollider::PrepareForSimulation()
{
	Collider::PrepareForSimulation();
}

const PxGeometry& BoxCollider::GetGeometryShape() const
{
	return m_geometry;
}

const Vector3& BoxCollider::GetHalfDimensions() const
{
	return m_halfDimensions;
}

void BoxCollider::SetHalfDimensions(const Vector3& dimensions)
{
	Vector3 scale = gameObject.transform.Scale();
	m_halfDimensions = dimensions;
	m_geometry.halfExtents = { m_halfDimensions.x * scale.x,
							   m_halfDimensions.y * scale.y,
							   m_halfDimensions.z * scale.z };
}

void BoxCollider::DrawDebug()
{
	DrawDebug::DrawBox(gameObject.transform.GetWorldMatrix() * Matrix4::Translate(m_center),
						m_halfDimensions.x,
						m_halfDimensions.y,
						m_halfDimensions.z,
						3.0f, Color3::LightGreen);
}