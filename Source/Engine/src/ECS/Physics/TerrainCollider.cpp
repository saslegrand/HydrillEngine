#include "ECS/GameObject.hpp"

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"

#include "Resources/Resource/Terrain.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Mesh.hpp"

#include "Generated/TerrainCollider.rfks.h"

rfk::UniquePtr<TerrainCollider> TerrainCollider::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<TerrainCollider>(GO, uid);
}

TerrainCollider::TerrainCollider(GameObject& gameObject, const HYGUID& uid)
	: Collider(gameObject, staticGetArchetype().getName(), uid)
{
	gameObject.isStatic = true;
	m_geometry = PxHeightFieldGeometry();
	//EngineContext::Instance().physicsSystem->Register(*this);
}

void TerrainCollider::PrepareGeometry()
{
	if (m_terrain)
	{
		Mesh* heightMapMesh = m_terrain->heightMapMesh;
		if (!heightMapMesh) return;
		if (heightMapMesh->subMeshes.size() != 1) return;
		MeshData& mesh = heightMapMesh->subMeshes[0];
		if (mesh.vertices.empty()) return;

		// Regen terrain from mesh
		PxHeightFieldDesc desc;
		const unsigned int nbRows = m_terrain->rowNb;
		const unsigned int nbColumns = m_terrain->columnNb;

		desc.nbRows = nbRows;
		desc.nbColumns = nbColumns;
		samplesData = (PxU32*)malloc(sizeof(PxU32) * (nbColumns * nbRows)); // PhysX way to do it - Malloc is freed after creation of the shape in PrepareForSimulation() (down below)
		desc.samples.data = samplesData;
		desc.samples.stride = sizeof(PxU32);
		desc.convexEdgeThreshold = 0;
		PxU8* currentByte = (PxU8*)desc.samples.data;

		for (unsigned int i = 0; i < nbRows; i++)
		{
			for (unsigned int j = 0; j < nbColumns; j++)
			{
				PxHeightFieldSample* currentSample = (PxHeightFieldSample*)currentByte;
				currentSample->height = static_cast<PxI16>(mesh.vertices[i * nbColumns + j].position.y);
				currentSample->materialIndex0 = 0;
				currentSample->materialIndex1 = 0;
				currentSample->clearTessFlag();
				currentByte += desc.samples.stride;
			}
		}

		PxHeightField* heightField = SystemManager::GetPhysicsSystem().GetSimulation().CreateHeightField(desc);

		Vector3 scale = gameObject.transform.Scale();
		m_geometry = PxHeightFieldGeometry(heightField, PxMeshGeometryFlags(), scale.y, scale.x, scale.z); // Because we applied scale to mesh, we create it at One scale
	}
}

void TerrainCollider::PrepareForSimulation()
{
	Collider::PrepareForSimulation();

	free(samplesData); // Malloc is freed here
	m_PxShape->userData = this;
	SetMaterial(m_material);
}

const PxGeometry& TerrainCollider::GetGeometryShape() const
{
	return m_geometry;
}

void TerrainCollider::DrawDebug()
{

}