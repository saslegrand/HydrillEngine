#pragma once

#include "Maths/Quaternion.hpp"
#include "Maths/Maths.hpp"

/**
@brief Data for the transform component
*/
class TransformData
{
public:
	Matrix4		worldMatrix = Matrix4::Identity;
	Matrix4		localMatrix = Matrix4::Identity;

	Vector3		position = Vector3::Zero;
	Quaternion	rotation = Quaternion::Identity;
	Vector3		scale = Vector3::One;

	TransformData() = default;

	static Vector3 Right(const Matrix4& mat);
	static Vector3 Left(const Matrix4& mat);
	static Vector3 Up(const Matrix4& mat);
	static Vector3 Down(const Matrix4& mat);
	static Vector3 Forward(const Matrix4& mat);
	static Vector3 Back(const Matrix4& mat);

	static Vector3 Translation(const Matrix4& mat);
	static Quaternion Rotation(const Matrix4& mat);
	static Matrix4 RotationMatrix(const Matrix4& mat);
	static Vector3 Scaling(const Matrix4& mat);

	/**
	@brief Calculate the TranslationRotationScale matrix

	@return Matrix4 : TRS Matrix
	*/
	void ComputeLocalTRS();

	/**
	@brief Calculate position/rotation/scale from localMatrix

	@return Matrix4 : TRS Matrix
	*/
	static void DecomposeMatrix(const Matrix4& mat, Vector3& position, Quaternion& rotation, Vector3& scale);
};

#include "ECS/TransformData.inl"