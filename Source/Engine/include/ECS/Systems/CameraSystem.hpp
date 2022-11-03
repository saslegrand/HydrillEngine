#pragma once

#include <vector>
#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDll.hpp"

class CameraComponent;
class Camera;
class GameObject;
class HYGUID;

/**
@brief System that handle every cameras in the game
*/
class CameraSystem
{

//	Variables

private:
	std::vector<std::unique_ptr<CameraComponent>> m_cameras;

	CameraComponent* m_activeCamera = nullptr;

public:
	
	Camera* renderingCamera = nullptr;

//	Functions

public:
	/**
	@brief Set a new active camera for the game mode

	@param camera : camera to set active
	*/
	ENGINE_API static void SetActiveCamera(CameraComponent& camera);

	/**
	@brief Get the current active camera component

	@return CameraComponent* : camera component ptr if one currently active, nullptr otherwise
	*/
	ENGINE_API static CameraComponent* GetActiveCameraComponent();

	/**
	@brief Get all camera components registered in the camera system

	@return const std::vector<const CameraComponent*>& : camea components list
	*/
	ENGINE_API static const std::vector<std::unique_ptr<CameraComponent>>& GetAllCameraComponents();

	/**
	@brief Get the current active camera

	@return Camera* : camera ptr if one currently active, nullptr otherwise
	*/
	ENGINE_API static Camera* GetActiveCamera();

	/**
	@brief Update the current active camera
	*/
	void Update();

	/**
	@brief Add a new camera to the system

	@param owner : Owner of the camera
	@param id : Unique identifier of the camera

	@return Behavior* : Created camera, nullptr if not valid
	*/
	CameraComponent* AddCameraInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing camera in the system

	@param camera : camera to remove
	*/
	void RemoveCameraInstance(CameraComponent& camera);
};