#pragma once

#include <physx/geometry/PxHeightFieldGeometry.h>
using namespace physx;

#include "ECS/Physics/Collider.hpp"

#include "Generated/TerrainCollider.rfkh.h"

class GameObject;
class Terrain;

/**
@brief Terrain Collider Component
*/
class HY_CLASS() ENGINE_API TerrainCollider : public Collider
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<TerrainCollider> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	// Disable compiler's warning C4251 when exporting the class member PxHeightFieldGeometry
	// PxHeightFieldGeometry is from physx but only used inside the DLL
#pragma warning(disable:4251)
	PxHeightFieldGeometry m_geometry;
#pragma warning(default:4251)

	PxU32* samplesData; // Temporary buffer to hold height field data

public:
	HY_FIELD() Terrain* m_terrain;

	TerrainCollider(GameObject& gameObject, const HYGUID& uid);
	TerrainCollider(TerrainCollider const& cam) = delete;
	TerrainCollider(TerrainCollider&& cam) = default;

	TerrainCollider& operator=(TerrainCollider const& cam) = delete;
	TerrainCollider& operator=(TerrainCollider && cam) = default;

public:
	virtual void PrepareGeometry() override;
	virtual void PrepareForSimulation() override;

	const PxGeometry& GetGeometryShape() const override;

	void DrawDebug() override;

	TerrainCollider_GENERATED
};

File_TerrainCollider_GENERATED