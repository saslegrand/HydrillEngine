#include "ECS/Transform.hpp"

#include "Maths/Matrix3.hpp"
#include "ECS/GameObject.hpp"

Transform::Transform(GameObject& go, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
	: gameObject(go)
{
	SetLocalPosition(position);
	SetLocalRotation(rotation);
	SetLocalScale(scale);
}

Transform::Transform(GameObject& go, const TransformData& transformData)
	: gameObject(go)
{
	data.position	= transformData.position;
	data.rotation	= transformData.rotation;
	data.scale		= transformData.scale;
}


Vector3 Transform::Position() const
{
	if (gameObject.GetParent())
		return Matrix4::Multiply(Vector4(data.position, 1.0f), gameObject.GetParent()->transform.GetWorldMatrix()).xyz;
	return data.position;
}

Quaternion Transform::Rotation() const
{
	if (gameObject.GetParent())
		return gameObject.GetParent()->transform.Rotation() * data.rotation;
	return data.rotation;
}

Vector3 Transform::Scale() const
{
	if (gameObject.GetParent())
		return gameObject.GetParent()->transform.Scale() * data.scale;
	return data.scale;
}


void Transform::ComputeLocalModelMatrix()
{
	if (m_dirtyFlag & EDirtyFlags::Local)
	{
		// Update local matrix
		data.ComputeLocalTRS();
		ResetDirtyFlag();
	}

	if (m_dirtyFlag & EDirtyFlags::World)
	{
		bool hasParent = gameObject.GetParent();
		if (hasParent)
			data.localMatrix = (Matrix4::Inverse(gameObject.GetParent()->transform.GetWorldMatrix())) * data.worldMatrix;
		else
			data.localMatrix = data.worldMatrix;
		
		data.DecomposeMatrix(data.localMatrix, data.position, data.rotation, data.scale);

		ResetDirtyFlag();
	}
}


void Transform::ComputeModelMatrices()
{
	ComputeLocalModelMatrix();
	data.worldMatrix = data.localMatrix;
}

void Transform::ComputeModelMatrices(const Matrix4& parentMatrix)
{
	ComputeLocalModelMatrix();
	data.worldMatrix = parentMatrix * data.localMatrix;
}


void Transform::SetPosition(const Vector3& newPosition)
{
	if (gameObject.GetParent())
		data.position = Matrix4::Multiply( Vector4(newPosition, 1.0f), Matrix4::Inverse(gameObject.GetParent()->transform.GetWorldMatrix())).xyz;
	else
		data.position = newPosition;

	SetDirty(EDirtyFlags::Local);
}

void Transform::SetRotation(const Quaternion& newRotation)
{
	if (gameObject.GetParent())
		data.rotation = Quaternion::Invert(gameObject.GetParent()->transform.Rotation()) * newRotation;
	else
		data.rotation = newRotation;

	SetDirty(EDirtyFlags::Local);
}


void Transform::SetEulerAngles(const Vector3& newRotation)
{
	SetRotation(Quaternion::FromEulerRad(newRotation * Maths::DEGTORAD));
}

void Transform::SetScale(const Vector3& newScale)
{
	if (newScale.SquaredMagnitude() < Maths::SmallFloat) return;

	if (gameObject.GetParent())
		data.scale = newScale / gameObject.GetParent()->transform.Scale();
	else
		data.scale = newScale;

	SetDirty(EDirtyFlags::Local);
}


Vector3 Transform::TransformDirection(const Vector3& direction)
{
	// For direction, we want only rotation
	//return data.rotation * direction;
	return Vector3::Zero;
}
//Vector3 TransformDirection(float x, float y, float z);


Vector3 Transform::TransformPoint(const Vector3& point)
{
	// For point, we want position & rotation & scale
	//return (data.worldMatrix * Vector4(point, 1.f)).xyz;
	return Vector3::Zero;
}
//Vector3 TransformPoint(float x, float y, float z);


Vector3 Transform::TransformVector(const Vector3& vector)
{
	// For vector, we want only rotation & scale
	//return Matrix4::ToMatrix3(GetWorldMatrix()) * vector;
	return Vector3::Zero;
}
//Vector3 TransformVector(float x, float y, float z);


Vector3 Transform::InverseTransformDirection(const Vector3& direction)
{
	//return data.rotation
	return Vector3::Zero;
}
//Vector3 InverseTransformDirection(float x, float y, float z);


Vector3 Transform::InverseTransformPoint(const Vector3& point)
{
	//return (Matrix4::Inverse(GetWorldMatrix()) * Vector4(point, 1.f)).xyz;
	return Vector3::Zero;
}
//Vector3 InverseTransformPoint(float x, float y, float z);


Vector3 Transform::InverseTransformVector(const Vector3& vector)
{
	//return Matrix4::ToMatrix3(Matrix4::Inverse(GetWorldMatrix())) * vector;
	return Vector3::Zero;
}
//Vector3 InverseTransformVector(float x, float y, float z);