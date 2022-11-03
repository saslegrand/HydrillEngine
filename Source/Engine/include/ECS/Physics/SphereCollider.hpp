#pragma once

#include <physx/geometry/PxSphereGeometry.h>
using namespace physx;

#include "ECS/Physics/Collider.hpp"

#include "Generated/SphereCollider.rfkh.h"

class GameObject;

/**
@brief Sphere Collider Component
*/
class HY_CLASS() ENGINE_API SphereCollider : public Collider
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<SphereCollider> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	// Disable compiler's warning C4251 when exporting the class member PxSphereGeometry
	// PxSphereGeometry is from physx but only used inside the DLL
#pragma warning(disable:4251)
	PxSphereGeometry m_geometry;
#pragma warning(default:4251)

	HY_FIELD() float m_radius = 0.5f;

public:
	
	SphereCollider(GameObject& gameObject, const HYGUID& uid);
	SphereCollider(SphereCollider const& cam) = delete;
	SphereCollider(SphereCollider&& cam) = default;

	SphereCollider& operator=(SphereCollider const& cam) = delete;
	SphereCollider& operator=(SphereCollider && cam) = default;

protected:
	virtual void PrepareGeometry() override;
	virtual void PrepareForSimulation() override;

public:
	const PxGeometry& GetGeometryShape() const override;

	float GetRadius() const;
	void SetRadius(float value);

	void DrawDebug() override;

	SphereCollider_GENERATED
};

File_SphereCollider_GENERATED