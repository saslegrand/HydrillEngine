#include "ECS/Systems/CameraSystem.hpp"

#include "EngineContext.hpp"

#include "ECS/CameraComponent.hpp"
#include "ECS/GameObject.hpp"

#include "Core/Logger.hpp"

void CameraSystem::SetActiveCamera(CameraComponent& camera)
{
	CameraSystem& system = SystemManager::GetCameraSystem();

	// Find the the camera
	auto it = std::find_if(system.m_cameras.begin(), system.m_cameras.end(),
		[&camera](std::unique_ptr<CameraComponent>& _camera) { return _camera.get() == &camera; });

	if (it == system.m_cameras.end())
	{
		Logger::Error("CameraSystem - The camera you tried to set active is not registered");
		return;
	}

	system.m_activeCamera = &camera;
}

CameraComponent* CameraSystem::GetActiveCameraComponent()
{
	return SystemManager::GetCameraSystem().m_activeCamera;
}

const std::vector<std::unique_ptr<CameraComponent>>& CameraSystem::GetAllCameraComponents()
{
	return SystemManager::GetCameraSystem().m_cameras;
}

Camera* CameraSystem::GetActiveCamera()
{
	CameraComponent* camComp = SystemManager::GetCameraSystem().m_activeCamera;
	return camComp != nullptr ? &camComp->camera : nullptr;
}

void CameraSystem::Update()
{
	if (m_activeCamera == nullptr)
		return;

	if (m_activeCamera->IsActive())
		m_activeCamera->UpdateCamera();
}

CameraComponent* CameraSystem::AddCameraInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	CameraComponent& cam = *m_cameras.emplace_back(archetype.makeUniqueInstance<CameraComponent>(owner, id));

	if (m_activeCamera == nullptr)
		m_activeCamera = &cam;

	return &cam;
}

void CameraSystem::RemoveCameraInstance(CameraComponent& camera)
{
	// Find the the camera
	auto it = std::find_if(m_cameras.begin(), m_cameras.end(),
		[&camera](std::unique_ptr<CameraComponent>& _camera) { return _camera.get() == &camera; });

	if (it == m_cameras.end())
	{
		// Should not happer
		Logger::Error("CameraSystem - The camera component you tried to remove not exists");
		return;
	}

	if (it->get() == m_activeCamera)
		m_activeCamera = m_cameras.size() != 1 ? m_cameras[0].get() : nullptr;

	// Remove the camera
	m_cameras.erase(it);
}