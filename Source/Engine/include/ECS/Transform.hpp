#pragma once

#include "ECS/TransformData.hpp"
#include "Maths/Quaternion.hpp"
#include "EngineDLL.hpp"

class GameObject;

enum class EDirtyFlags : short
{
	None = 0,

	Local = 1 << 0,
	World = 1 << 1
};

/**
@brief Component to place the object in the world
*/
class Transform
{
private:
	TransformData data;

	/**
	@brief Tell if the transform needs to be updated
	*/

	int m_dirtyFlag = 0;

public:
	/**
	@brief Owner of the transform
	*/
	GameObject& gameObject;

public:
	/**
	@brief Constructor object from TransformData struct

	@param go : Owner
	@param transformData : Data to set
	*/
	Transform(GameObject& go, const TransformData& transformData);

	/**
	@brief Constructor object from TransformData struct

	@param go : Owner
	@param position : localPostion of the object
	@param rotation : localRotation of the object
	@param scale : localScale of the object
	*/
	Transform(GameObject& go, const Vector3& position = Vector3::Zero, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);

	Transform(Transform const& cam) = delete;
	Transform(Transform&& cam) = default;

	Transform& operator=(Transform const& cam) = delete;
	Transform& operator=(Transform && cam) = default;

private:
	/**
	@brief Update the transform local matrix
	*/
	void ComputeLocalModelMatrix();

	void ResetDirtyFlag();

public:
	/**
	@brief Update the global matrix (equal to local matrix)
	*/
	void ComputeModelMatrices();

	/**
	@brief Update the global matrix from parent matrix

	@param parentMatrix : Matrix of the parent
	*/
	void ComputeModelMatrices(const Matrix4& parentMatrix);


	// -------
	// GETTERS

	/**
	@brief Get the current world position of the transform

	@return Vector3 : Current world position
	*/
	ENGINE_API Vector3 Position() const;

	/**
	@brief Get the current local position of the transform

	@return Vector3& : Current local position
	*/
	ENGINE_API const Vector3& LocalPosition() const;


	/**
	@brief Get the current world rotation of the transform

	@return Quaternion : Current world rotation
	*/
	ENGINE_API Quaternion Rotation() const;

	/**
	@brief Get the current local rotation of the transform

	@return Quaternion& : Current local rotation
	*/
	ENGINE_API Quaternion LocalRotation() const;


	/**
	@brief Get the current world rotation of the transform in euler angles

	@return Vector3 : Current world rotation in euler angles
	*/
	ENGINE_API Vector3 EulerAngles() const;

	/**
	@brief Get the current local rotation of the transform in euler angles

	@return Vector3 : Current local rotation in euler angles
	*/
	ENGINE_API Vector3 LocalEulerAngles() const;


	/**
	@brief Get the current world scale of the transform

	@return Vector3 : Current world scale
	*/
	ENGINE_API Vector3 Scale() const;

	/**
	@brief Get the current local scale of the transform

	@return Vector3& : Current local scale
	*/
	ENGINE_API const Vector3& LocalScale() const;


	// -------
	// SETTERS

	/**
	@brief Set a new position  in world space (set dirty)

	@param newPosition : The new position to set
	*/
	ENGINE_API void SetPosition(const Vector3& newPosition);
	//ENGINE_API void SetPosition(float x, float y, float z);

	/**
	@brief Set a new position  in local space (set dirty)

	@param newPosition : The new position to set
	*/
	ENGINE_API void SetLocalPosition(const Vector3& newPosition);
	//ENGINE_API void SetLocalPosition(float x, float y, float z);


	/**
	@brief Set a new rotation in world space (set dirty)

	@param newRotation : The new rotation to set in quaternion
	*/
	ENGINE_API void SetRotation(const Quaternion& newRotation);

	/**
	@brief Set a new rotation in local space (set dirty)

	@param newRotation : The new rotation to set in quaternion
	*/
	ENGINE_API void SetLocalRotation(const Quaternion& newRotation);


	/**
	@brief Set a new rotation in world space with euler angles (set dirty)

	@param newRotation : The new rotation to set in quaternion
	*/
	ENGINE_API void SetEulerAngles(const Vector3& newRotation);
	//ENGINE_API void SetEulerAngles(float x, float y, float z);
	
	/**
	@brief Set a new rotation in local space with euler angles (set dirty)

	@param newRotation : The new rotation to set in quaternion
	*/
	ENGINE_API void SetLocalEulerAngles(const Vector3& newRotation);
	//ENGINE_API void SetLocalEulerAngles(float x, float y, float z);

	/**
	@brief Set a new scale in world space (set dirty)

	@param newScale : The new scale to set
	*/
	ENGINE_API void SetScale(const Vector3& newScale);
	//ENGINE_API void SetScale(float x, float y, float z);

	/**
	@brief Set a new scale in local space (set dirty)

	@param newScale : The new scale to set
	*/
	ENGINE_API void SetLocalScale(const Vector3& newScale);
	//ENGINE_API void SetLocalScale(float x, float y, float z);


	// ----------
	// WORLD AXIS

	/**
	@brief Get the current forward vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Forward() const;
	ENGINE_API Vector3 LocalForward() const;

	/**
	@brief Get the current back vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Back() const;
	ENGINE_API Vector3 LocalBack() const;

	/**
	@brief Get the current left vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Left() const;
	ENGINE_API Vector3 LocalLeft() const;

	/**
	@brief Get the current right vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Right() const;
	ENGINE_API Vector3 LocalRight() const;

	/**
	@brief Get the current up vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Up() const;
	ENGINE_API Vector3 LocalUp() const;

	/**
	@brief Get the current down vector of the transform

	@return Vector3& : normalized Directional vector
	*/
	ENGINE_API Vector3 Down() const;
	ENGINE_API Vector3 LocalDown() const;



	/**
	@brief Transforms a direction from local space to world space

	@param direction : const Vector3& - local space direction
	@return Vector3 : world spaced direction
	*/
	Vector3 TransformDirection(const Vector3& direction);
	//Vector3 TransformDirection(float x, float y, float z);

	/**
	@brief Transforms a position from local space to world space

	@param point : const Vector3& - local space point
	@return Vector3& : world spaced position
	*/
	Vector3 TransformPoint(const Vector3& point);
	//Vector3 TransformPoint(float x, float y, float z);

	/**
	@brief Transforms a vector from local space to world space

	@param vector : const Vector3& - local space vector
	@return Vector3& : world spaced vector
	*/
	Vector3 TransformVector(const Vector3& vector);
	//Vector3 TransformVector(float x, float y, float z);

	/**
	@brief Transforms a direction from world space to local space

	@param direction : const Vector3& - world space direction
	@return Vector3& : local spaced direction
	*/
	Vector3 InverseTransformDirection(const Vector3& direction);
	//Vector3 InverseTransformDirection(float x, float y, float z);

	/**
	@brief Transforms a position from world space to local space

	@param position : const Vector3& - world space position
	@return Vector3& : local spaced position
	*/
	Vector3 InverseTransformPoint(const Vector3& point);
	//Vector3 InverseTransformPoint(float x, float y, float z);

	/**
	@brief Transforms a vector from world space to local space

	@param vector : const Vector3& - world space vector
	@return Vector3& : local spaced vector
	*/
	Vector3 InverseTransformVector(const Vector3& vector);
	//Vector3 InverseTransformVector(float x, float y, float z);
	


	/**
	@brief Get the current global matrix of the transform

	@return Matrix& : Current global matrix
	*/
	Matrix4& GetWorldMatrix();

	/**
	@brief Get the current local matrix of the transform

	@return Matrix& : Current local matrix
	*/
	Matrix4& GetLocalMatrix();


	/**
	@brief Get the dirty state of the transform

	@return bool : true if dirty, false otherwise
	*/
	bool IsDirty() const;

	/**
	@brief Set the dirty flag from given flag, the other defined flags are preserved

	@param flag : Flag to set
	*/
	void SetDirty(EDirtyFlags flag);
};

#include "ECS/Transform.inl"