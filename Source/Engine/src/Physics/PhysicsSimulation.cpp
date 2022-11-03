#include "Physics/PhysicsSimulation.hpp"

#include <assert.h>

#include "Core/Logger.hpp"

#include "Physics/PhysicsActor.hpp"
#include "ECS/Physics/Collider.hpp"
#include "ECS/GameObject.hpp"

#include "Resources/Resource/Terrain.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Maths/Vector3.hpp"

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"

bool IsCCDActive(PxFilterData& filterData)
{
	return filterData.word3 & PxRigidBodyFlag::eENABLE_CCD;
}

PxFilterFlags GetFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{

	// If one is a trigger
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		return PxFilterFlag::eDEFAULT;
	}

	// If not a trigger, then collision
	pairFlags = physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;

	// For contact points info
	pairFlags = physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	
	if (IsCCDActive(filterData0) || IsCCDActive(filterData1))
	{
		// For Continious Collision Dynamic
		pairFlags |= PxPairFlag::eSOLVE_CONTACT;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	}
	// For filtering
	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	//{
	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	//}

	return PxFilterFlag::eDEFAULT;
}

void PhysicsSimulation::Create()
{
	if (m_foundation != nullptr)
		return; // Physics Simulation is already created;

	// Create foundation
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	// Create pvd
	//if (m_visualDebug)
	//{
	//	m_pvd = PxCreatePvd(*m_foundation);
	//	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 100);
	//	m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	//}

	// Create physics
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd);

	PxCookingParams cookingParams = PxCookingParams(PxTolerancesScale());
	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, cookingParams);

	// Create scene parameters
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = GetFilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eABP;

	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	sceneDesc.sceneQueryUpdateMode = PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;

	m_scene = m_physics->createScene(sceneDesc);

	//PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	m_material = m_physics->createMaterial(0.2f, 0.2f, 0.5f);
}

void PhysicsSimulation::Destroy()
{
	PX_RELEASE(m_scene);
	PX_RELEASE(m_dispatcher);
	PX_RELEASE(m_physics);
	PX_RELEASE(m_cooking);
	//if (m_pvd)
	//{
	//	PxPvdTransport* transport = m_pvd->getTransport();
	//	m_pvd->release();
	//	m_pvd = nullptr;
	//	PX_RELEASE(transport);
	//}
	PX_RELEASE(m_foundation);
}


void PhysicsSimulation::StepSimulation(float timeStep)
{
	m_scene->simulate(timeStep);
	m_scene->fetchResults(true);

	//PxU32 nbActiveTransforms;
	//PxActor** activeTransforms = m_scene->getActiveActors(nbActiveTransforms);
	//
	//for (PxU32 i = 0; i < nbActiveTransforms; ++i)
	//{
	//	PhysicsActor* actor = static_cast<PhysicsActor*>(activeTransforms[i]->userData);
	//	actor->GetGameObject().
	//}
}

PxShape* PhysicsSimulation::CreateShape(Collider* collider, PxMaterial* material)
{
	PxFlags<PxShapeFlag::Enum, PxU8> flag;

	if (collider->IsTrigger())
		flag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE;
	else
		flag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE;

	PxShape* shape = m_physics->createShape(collider->GetGeometryShape(), &material, true, flag);

	//PxFilterData filterData;
	//filterData.word0 = PhysicsLayer::Layer1; // TODO : put the layer of the gameobject
	//filterData.word1 = PX_MAX_U32; // TODO : Put the appropriate layer mask collision
	//shape->setQueryFilterData(filterData);

	return shape;
}

PxMaterial* PhysicsSimulation::CreateMaterial(const PhysicsMaterial& material)
{
	return m_physics->createMaterial(material.staticFriction, material.dynamicFriction, material.restitution);
}

PxRigidDynamic* PhysicsSimulation::CreateDynamicActor(const Transform& transform)
{
	Vector3 pos = transform.Position();
	Quaternion rot = transform.Rotation();
	return m_physics->createRigidDynamic(PxTransform(pos.x, pos.y, pos.z, PxQuat{ rot.x, rot.y, rot.z, rot.w }));
}

PxRigidStatic* PhysicsSimulation::CreateStaticActor(const Transform& transform)
{
	Vector3 pos = transform.Position();
	Quaternion rot = transform.Rotation();
	return m_physics->createRigidStatic(PxTransform(pos.x, pos.y, pos.z, PxQuat{ rot.x, rot.y, rot.z, rot.w }));
}

bool PhysicsSimulation::GenerateTerrain(Terrain* terrain)
{
	terrain->heightMapMesh->subMeshes.clear();
	terrain->heightMapMesh->subMeshes.push_back(MeshData());
	MeshData& mesh = terrain->heightMapMesh->subMeshes[0];
	Texture* heightMap = terrain->heightMapTexture;

	if (terrain->LOD < 1) terrain->LOD = 1;

	const int PixelNbRows = heightMap->data.width;
	const int PixelNbColumns = heightMap->data.height;

	terrain->rowNb = PixelNbRows / terrain->LOD;
	terrain->columnNb = PixelNbColumns / terrain->LOD;

	unsigned short* pixel = reinterpret_cast<unsigned short*>(heightMap->data.data);

	mesh.vertices.clear();
	mesh.indices.clear();
	
	for (int k = 0; k < terrain->rowNb * terrain->columnNb; k++)
	{
		mesh.vertices.push_back(Vertex());
	}

	for (int i = 0; i < terrain->rowNb; i++)
	{
		for (int j = 0; j < terrain->columnNb; j++)
		{
			int pi = i * terrain->LOD;
			int pj = j * terrain->LOD;
			float height = pixel[(pi * PixelNbColumns + pj) * heightMap->data.channels] / 256.0f;

			float row = static_cast<float>(i);
			float column = static_cast<float>(j);

			mesh.vertices[i * terrain->columnNb + j].position = { row * terrain->rowScale, height * terrain->heightScale, column * terrain->columnScale };
			mesh.vertices[i * terrain->columnNb + j].uvs.x = (column / static_cast<float>(terrain->columnNb));
			mesh.vertices[i * terrain->columnNb + j].uvs.y = (row / static_cast<float>(terrain->rowNb));

			int sx = pixel[(pi < PixelNbRows - 1 ? pi + 1 : pi) * PixelNbColumns + pj] - pixel[(pi > 0 ? pi - 1 : pi) * PixelNbColumns + pj];
			if (pi == 0 || pi == PixelNbRows - 1)
				sx *= 2;

			int sy = pixel[pi * PixelNbColumns + (pj < PixelNbColumns - 1 ? pj + 1 : pj)] - pixel[pi * PixelNbColumns + (pj > 0 ? pj - 1 : pj)];
			if (pj == 0 || pj == PixelNbColumns- 1)
				sy *= 2;

			//mesh.vertices[i * nbColumns + j].normal = Vector3::Up;
			mesh.vertices[i * terrain->columnNb + j].tangent = Vector3{ 2.0f, 2.0f, (float)sy }.SafeNormalized();
			mesh.vertices[i * terrain->columnNb + j].bitangent = Vector3{2.0f, 2.0f, (float)sx}.SafeNormalized();
			mesh.vertices[i * terrain->columnNb + j].normal = Vector3{ -sx * terrain->rowScale * 0.5f, terrain->heightScale, sy * terrain->columnScale * 0.5f }.SafeNormalized();
		}
	}

	for (int i = 0; i < (terrain->columnNb - 1); i++)
	{
		for (int j = 0; j < (terrain->rowNb - 1); j++)
		{
			// First triangle
			mesh.indices.push_back((i + 1) * terrain->rowNb + j);
			mesh.indices.push_back(i * terrain->rowNb + j);
			mesh.indices.push_back(i * terrain->rowNb + j + 1);

			// Second triangle
			mesh.indices.push_back((i + 1) * terrain->rowNb + j + 1);
			mesh.indices.push_back((i + 1) * terrain->rowNb + j);
			mesh.indices.push_back(i * terrain->rowNb + j + 1);
		}
	}

	return true;
}

PxHeightField* PhysicsSimulation::CreateHeightField(const PxHeightFieldDesc& heightFieldDesc)
{
	return m_cooking->createHeightField(heightFieldDesc, m_physics->getPhysicsInsertionCallback());
}

void PhysicsSimulation::AddActorToSimulation(PxActor& actor)
{
	m_scene->addActor(actor);
}

void PhysicsSimulation::RemoveActorFromSimulation(PxActor& actor)
{
	m_scene->removeActor(actor);
}

bool PhysicsSimulation::Raycast(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask)
{
	//const PxHitFlags outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;
	//PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eANY_HIT);
	//filterData.data.word0 = layerMask;
	return m_scene->raycast({ origin.x, origin.y, origin.z }, { unitDirection.x, unitDirection.y, unitDirection.z }, maxDistance, outHit);// , outputFlags, filterData);
}

void PhysicsSimulation::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	const PxU32 bufferSize = 64;
	PxContactPairPoint contacts[bufferSize];

	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.shapes[0]->userData == nullptr || cp.shapes[1]->userData == nullptr)
			break;

		// Retreive contact points
		PxU32 nbContacts = cp.extractContacts(contacts, bufferSize);
		Vector3 position = Vector3::Zero;
		Vector3 normal = Vector3::Up;
		if (nbContacts > 0)
		{
			PxContactPairPoint closestContact = contacts[0];
			normal = Vector3{ closestContact.normal.x, closestContact.normal.y, closestContact.normal.z };
			position = Vector3{ closestContact.position.x, closestContact.position.y, closestContact.position.z };
		}

		// Retreive both actor
		//PhysicsActor* p1 = static_cast<PhysicsActor*>(pairHeader.actors[0]->userData);
		//PhysicsActor* p2 = static_cast<PhysicsActor*>(pairHeader.actors[1]->userData);

		// Retreive both colliders
		Collider* c1 = nullptr;
		Collider* c2 = nullptr;
		if (cp.shapes[0]->getActor() == pairHeader.actors[0])
		{
			c1 = static_cast<Collider*>(cp.shapes[0]->userData);
			c2 = static_cast<Collider*>(cp.shapes[1]->userData);
		}
		else
		{
			c1 = static_cast<Collider*>(cp.shapes[1]->userData);
			c2 = static_cast<Collider*>(cp.shapes[0]->userData);
		}		

		CollisionInfo collisionInfo1 = { c2, position, normal };
		CollisionInfo collisionInfo2 = { c1, position, -normal };

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			c1->OnCollisionStay.Invoke(collisionInfo1);
			c2->OnCollisionStay.Invoke(collisionInfo2);
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			c1->OnCollisionEnter.Invoke(collisionInfo1);
			c2->OnCollisionEnter.Invoke(collisionInfo2);
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			c1->OnCollisionExit.Invoke(collisionInfo1);
			c2->OnCollisionExit.Invoke(collisionInfo2);
		}
	}
}

void PhysicsSimulation::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		const PxTriggerPair& cp = pairs[i];
		
		if (cp.triggerShape->userData == nullptr || cp.otherShape->userData == nullptr)
			break;

		// Retreive both colliders
		Collider* c1 = static_cast<Collider*>(cp.triggerShape->userData);
		Collider* c2 = static_cast<Collider*>(cp.otherShape->userData);
		
		Vector3 positionTrigger = c1->gameObject.transform.Position();
		Vector3 positionOther = c2->gameObject.transform.Position();
		Vector3 normal = (positionTrigger - positionOther).SafeNormalized();

		CollisionInfo collisionInfo1 = { c2, positionOther, normal };
		
		if (cp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			c1->OnTriggerEnter.Invoke(collisionInfo1);
		}
		else if (cp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			c1->OnTriggerExit.Invoke(collisionInfo1);
		}
	}
}