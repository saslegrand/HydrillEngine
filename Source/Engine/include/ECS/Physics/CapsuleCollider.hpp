#pragma once

#include <physx/geometry/PxCapsuleGeometry.h>
using namespace physx;

#include "ECS/Physics/Collider.hpp"

#include "Generated/CapsuleCollider.rfkh.h"

class GameObject;

/**
@brief Capsule Collider Component
*/
class HY_CLASS() ENGINE_API CapsuleCollider : public Collider
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<CapsuleCollider> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	// Disable compiler's warning C4251 when exporting the class member PxCapsuleGeometry
	// PxCapsuleGeometry is from physx but only used inside the DLL
#pragma warning(disable:4251)
	PxCapsuleGeometry m_geometry;
#pragma warning(default:4251)

	HY_FIELD() float m_radius = 0.5f;
	HY_FIELD() float m_halfHeight = 0.5f;

public:
	
	CapsuleCollider(GameObject& gameObject, const HYGUID& uid);
	CapsuleCollider(CapsuleCollider const& cam) = delete;
	CapsuleCollider(CapsuleCollider&& cam) = default;

	CapsuleCollider& operator=(CapsuleCollider const& cam) = delete;
	CapsuleCollider& operator=(CapsuleCollider && cam) = default;

protected:
	virtual void PrepareGeometry() override;
	virtual void PrepareForSimulation() override;

public:
	const PxGeometry& GetGeometryShape() const override;

	float GetRadius() const;
	void SetRadius(float value);
	float GetHalfHeight() const;
	void SetHalfHeight(float value);

	void DrawDebug() override;

	CapsuleCollider_GENERATED
};

File_CapsuleCollider_GENERATED
