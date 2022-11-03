#include "Particles/ParticleInstance.hpp"

#include "ECS/GameObject.hpp"
#include "Particles/ParticleEmitter.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "EngineContext.hpp"

namespace SpaceUtils
{
	Vector3 GetRandomDirInCone(Matrix4 rotationMatrix, float angle)
	{
		float roll = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * Maths::PI2;
		float upAngle = (angle * 0.5f - static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * angle) * Maths::DEGTORAD;

		Vector3 forward = TransformData::Forward(rotationMatrix);
		Quaternion rot = Quaternion::FromAxisAngle(forward, roll) * Quaternion::FromAxisAngle(TransformData::Up(rotationMatrix), upAngle);

		return rot * forward;
	}

	Vector3 GetRandomPointInCone(Vector3 position, Matrix4 rotationMatrix, float angle, float distance = 1.0f, float minDistance = 0.0f)
	{
		float randDistance = minDistance + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (distance - minDistance);

		return position + GetRandomDirInCone(rotationMatrix, angle) * randDistance;
	}

	Vector3 GetRandomDirInSphere()
	{
		Vector3 randomDir = Vector3(
			1.0f - (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 2.0f,
			1.0f - (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 2.0f,
			1.0f - (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 2.0f
		).SafeNormalized();

		return randomDir;
	}

	Vector3 GetRandomPointInSphere(Vector3 center, float radius = 1.0f, float minRadius = 0.0f)
	{
		float randDistance = minRadius + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (radius - minRadius);

		return center + GetRandomDirInSphere() * randDistance;
	}
}


ParticleInstance::ParticleInstance(const ParticleEmitter& emitter)
{
}


ParticleInstance::~ParticleInstance() 
{
}


void ParticleInstance::Update(const ParticleEmitter& emitter, float tick)
{
	//	Update life & ages values
	//	-------------------------

	m_life	-= tick;
	m_age		 = m_parameters.lifeStart.GetValue() - m_life;
	m_ageRatio = m_age / m_parameters.lifeStart.GetValue();


	m_parameters.Update(m_ageRatio);

	color = Color4::Lerp(m_parameters.startColor, m_parameters.endColor, m_ageRatio);
	emissiveColor = Color3::Lerp(m_parameters.startEmissiveColor, m_parameters.endEmissiveColor, m_ageRatio);

	emissiveStrength = m_parameters.emissiveStrength.GetValue();

	float billboardRotSpeed = m_parameters.billboardRotationSpeed.GetValue();
	billboardRotation = Maths::Sin((m_seed + m_age * billboardRotSpeed)) * Maths::PI;
	
	//	Update shader values
	//	--------------------

	diffuseUV.xy  += emitter.diffuseTexture.offsetSpeed * tick;
	emissiveUV.xy += emitter.emissiveTexture.offsetSpeed * tick;
	BlendUV_01.xy += emitter.blendTexture01.offsetSpeed * tick;
	BlendUV_02.xy += emitter.blendTexture02.offsetSpeed * tick;
	BlendUV_03.xy += emitter.blendTexture03.offsetSpeed * tick;
	BlendUV_04.xy += emitter.blendTexture04.offsetSpeed * tick;

	//	Update Transforms
	//	-----------------

	m_transform.scale = Vector3::One * m_parameters.sizeScale.GetValueFromCurve(m_ageRatio);

	UpdateTransformMatrix();

	//	If particle is allowed to respawn on death then respawn
	if (!IsAlive() && emitter.allowParticleRespawnOnDeath) Spawn(emitter);
}


void ParticleInstance::FixedUpdate(const ParticleEmitter& emitter, float fixedTick)
{
	//	Compute Forces
	//	--------------

	Vector3 force = m_sumForces;
	m_sumForces = Vector3::Zero;

	//	If Gravity enabled then apply gravity
	if (emitter.useGravity)
	{
		Vector3 weight = Vector3(0.0f, -9.81f, 0.0f) * m_parameters.mass.GetValue();
		force += weight;
	}

	//	Apply air drag
	force -= force * m_parameters.airDragging.GetValueFromCurve(m_ageRatio);

	//	Update acceleration, velocity then position
	//	-------------------------------------------

	Vector3 acceleration = force / m_parameters.mass.GetValue();

	m_velocity += acceleration * fixedTick;
	m_transform.position += m_velocity * fixedTick;
}


void ParticleInstance::Spawn(const ParticleEmitter& emitter)
{
	//	Reset variables
	//	---------------

	m_seed = rand() % 1000 / 10.f;

	m_ageRatio = m_age = 0.0f;
	m_velocity = m_sumForces = Vector3::Zero;

	//	Parameters (re)initializations
	//	--------------------------

	m_parameters = emitter.parameters;
	diffuseUV  = { emitter.diffuseTexture.offset, emitter.diffuseTexture.tiling };
	emissiveUV = { emitter.emissiveTexture.offset, emitter.emissiveTexture.tiling };
	BlendUV_01 = { emitter.blendTexture01.offset, emitter.blendTexture01.tiling };
	BlendUV_02 = { emitter.blendTexture02.offset, emitter.blendTexture02.tiling };
	BlendUV_03 = { emitter.blendTexture03.offset, emitter.blendTexture03.tiling };
	BlendUV_04 = { emitter.blendTexture04.offset, emitter.blendTexture04.tiling };

	m_parameters.Initalize();

	//	Settings datas from m_parameters
	//	------------------------------

	m_life = m_parameters.lifeStart.GetValue();
	color = m_parameters.startColor;
	emissiveColor = m_parameters.startEmissiveColor;
	emissiveStrength = m_parameters.emissiveStrength.GetValue();

	Vector3 pos = Vector3::Zero;
	Matrix4 rot = Matrix4::Identity;
	if (emitter.worldSpace && emitter.gameObject)
	{
		pos = emitter.gameObject->transform.Position();
		rot = Quaternion::ToMatrix(emitter.gameObject->transform.LocalRotation());
	}

	//	Initialization of datas

	Vector3 burst;
	//	Sphere shape random spawn
	if (emitter.burstShape == ParticleEmitter::SpawnShape::Sphere)
	{
		burst = SpaceUtils::GetRandomDirInSphere() * m_parameters.burstForce.GetValue();
	}
	//	Conse shape random spawn
	else if (emitter.burstShape == ParticleEmitter::SpawnShape::Cone)
	{
		burst = SpaceUtils::GetRandomDirInCone(rot, emitter.burstDirectionConeAngle) * m_parameters.burstForce.GetValue();
	}

	m_sumForces += burst;

	//	Transform Initialization
	//	------------------------

	//	Set used space

	m_parentMatrix = nullptr;

	if (!emitter.worldSpace)
	{
		if (emitter.gameObject)
		{
			//	If using a local space then attach a parent transform matrix
			m_parentMatrix = &emitter.gameObject->transform.GetWorldMatrix();
		}
	}

	//	Set spawn position depending on the chosen spawn shape
	//	Sphere shape random spawn
	if (emitter.spawnShape == ParticleEmitter::SpawnShape::Sphere)
	{
		m_transform.position = SpaceUtils::GetRandomPointInSphere(pos, emitter.spawnRange);
	}
	//	Conse shape random spawn
	else if (emitter.spawnShape == ParticleEmitter::SpawnShape::Cone)
	{
		m_transform.position = SpaceUtils::GetRandomPointInCone(pos, rot, emitter.spawnConeAngle, emitter.spawnRange);
	}

	//	Compute rotations

	if (!emitter.billboard)
	{
		m_transform.rotation = Quaternion::FromEulerRad({ rand() % 180 * Maths::DEGTORAD,rand() % 180 * Maths::DEGTORAD ,rand() % 180 * Maths::DEGTORAD });
	}
	m_transform.scale = Vector3::One * m_parameters.sizeScale.GetValueFromCurve(0.0f);

	//	Then, when all transform values are set, compute the transform matrix
	UpdateTransformMatrix();
}

bool ParticleInstance::IsAlive() const
{
	return m_life > 0;
}

void ParticleInstance::UpdateTransformMatrix()
{
	m_transform.ComputeLocalTRS();
	globalTransformModel = m_parentMatrix ? *m_parentMatrix * m_transform.localMatrix : m_transform.localMatrix;
}


Vector3	ParticleInstance::GetPosition() const
{
	return TransformData::Translation(globalTransformModel);
}

Quaternion ParticleInstance::GetRotation() const
{
	return TransformData::Rotation(globalTransformModel);
}

Vector3 ParticleInstance::GetScale() const
{
	return TransformData::Scaling(globalTransformModel);
}
