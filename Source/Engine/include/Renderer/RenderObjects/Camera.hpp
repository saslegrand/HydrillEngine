#pragma once
#include<EngineDLL.hpp>

#include "Maths/Matrix4.hpp"
#include "Maths/Quaternion.hpp"
#include "Core/BaseObject.hpp"
#include "Tools/RFKProperties.hpp"

#include "Generated/Camera.rfkh.h"

class HY_CLASS() ENGINE_API Camera : public BaseObject
{
//	Constructors
//	------------

public:

	Camera(const float FOV = 60.f, const float near = 0.1f, const float far = 5000.f, const float aspect = 1920.f / 1080.f, const Vector3& position = { 0.f,0.f,0.f }, const Vector3& rotation = { 0.f,0.f,0.f });

//	Variables
//	---------

private:
	
	HY_FIELD(POnModify("ComputeProjectionMatrix")) float m_FOV;
	HY_FIELD(POnModify("ComputeProjectionMatrix")) float m_near;
	HY_FIELD(POnModify("ComputeProjectionMatrix")) float m_far;

	float m_aspect;

	Matrix4 m_projMatrix;
	Matrix4 m_viewMatrix;

	bool m_recomputeProjMatrix = false;
	Vector3 m_position;
	Quaternion m_rotation;
	Quaternion m_billboardRotation;

	float m_exposure = 1.0f;
	float m_brightness = 0.0f;

//	Functions
//	---------

public:

	void UpdateExposure();

	//	Get and Set functions

	/**
	@brief Get FOV value of the camera
	*/
	float GetFOV()	  const;

	/**
	@brief Get Near value of the camera
	*/
	float GetNear()	  const;

	/**
	@brief Get Far value of the camera
	*/
	float GetFar()	  const;

	/**
	@brief Get Aspect value of the camera
	*/
	float GetAspect() const;

	/**
	@brief Get Projection matrix of the camera
	*/
	Matrix4 GetProjectionMatrix() const;

	/**
	@brief Get View matrix of the camera
	*/
	Matrix4 GetViewMatrix() const;

	/**
	* @brief Get Transform matrix of the camera
	*/
	Matrix4 GetTransformMatrix() const;

	/**
	@brief Get camera position
	*/
	Vector3 GetPosition() const;

	/**
	@brief Get camera rotation
	*/
	Quaternion GetRotation() const;
	
	/**
	@brief Get current camera exposure value
	*/
	float GetCurrentExposure() const;
	
	/**
	@brief Set/Change FOV value of the camera

	@param FOV : new FOV value
	*/
	void SetFOV(const float FOV);

	/**
	@brief Set/Change Near value of the camera

	@param near : new near value
	*/
	void SetNear(const float near);

	/**
	@brief Set/Change Far value of the camera

	@param far : new far value
	*/
	void SetFar(const float far);

	/**
	@brief Set/Change Aspect value of the camera

	@param aspect : new aspect value
	*/
	void SetAspect(const float aspect);

	/**
	@brief Set/Change Aspect value of the camera from a width and a height

	@param width : width to compute the aspect from
	@param height : height to compute the aspect from
	*/
	void SetAspect(const float width, const float height);


	/**
	@brief Set/Change Aspect value of the camera

	@param dimensions : vector 2d to compute aspect from
	*/
	void SetAspect(const Vector2 m_dimensions);

	/**
	@brief Set brightness average value of what the camera sees

	@param brightness : new brightness value
	*/
	void SetCurrentBrightness(float brightness);

	//	Matrix Compute functions

	/**
	@brief Update camera 
	*/
	void Update();

	/**
	@brief Update camera transform

	@param position : new position of the camera
	@param rotation : new rotation of the camera
	*/
	void SetTransform(const Vector3& position, const Quaternion& in_rotation);


	/**
	@brief Get billboard rotation from for this camera
	*/
	Quaternion GetBillboardRotation() const;

private:
	

	/**
	@brief Compute the View matrix of the camera

	@param position : position of the camera
	@param rotation : rotation of the camera
	*/
	void ComputeViewMatrix(const Vector3& position, const Quaternion& in_rotation);

	/**
	@brief Compute the Projection matrix of the camera
	*/
	HY_METHOD()
	void ComputeProjectionMatrix();

	Camera_GENERATED
};

File_Camera_GENERATED
