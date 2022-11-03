#pragma once

#include <physx/geometry/PxBoxGeometry.h>
using namespace physx;

#include "ECS/Physics/Collider.hpp"
#include "Maths/Vector3.hpp"

#include "Generated/BoxCollider.rfkh.h"

class GameObject;

/**
@brief Box Collider Component
*/
class HY_CLASS() ENGINE_API BoxCollider : public Collider
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<BoxCollider> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	// Disable compiler's warning C4251 when exporting the class member PxBoxGeometry
	// PxBoxGeometry is from physx but only used inside the DLL
#pragma warning(disable:4251)
	PxBoxGeometry m_geometry;
#pragma warning(default:4251)

	HY_FIELD() Vector3 m_halfDimensions = { 0.5f, 0.5f, 0.5f };

public:
	
	BoxCollider(GameObject& gameObject, const HYGUID& uid);
	BoxCollider(BoxCollider const& cam) = delete;
	BoxCollider(BoxCollider&& cam) = default;

	BoxCollider& operator=(BoxCollider const& cam) = delete;
	BoxCollider& operator=(BoxCollider && cam) = default;

protected:
	virtual void PrepareGeometry() override;
	virtual void PrepareForSimulation() override;

public:
	const PxGeometry& GetGeometryShape() const override;

	const Vector3& GetHalfDimensions() const;
	void SetHalfDimensions(const Vector3& dimensions);

	void DrawDebug() override;

	BoxCollider_GENERATED
};

File_BoxCollider_GENERATED
