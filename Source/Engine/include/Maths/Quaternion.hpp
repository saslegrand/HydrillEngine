#pragma once

#include "Maths/Vector4.hpp"
#include "Maths/Matrix4.hpp"

#include "Generated/Quaternion.rfkh.h"

struct HY_STRUCT() Quaternion
{
#pragma region _____PARAMETERS_____

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

#pragma endregion

#pragma region _____STATICS_____

	/**
	* Return identity quaternion
	**/
	ENGINE_API static Quaternion const Identity;

	/**
	* Adds two quaternions
	**/
	static Quaternion Add(const Quaternion& q1, const Quaternion& q2);

	/**
	* Add float to quaternion member values
	**/
	static Quaternion AddValue(const Quaternion& quat, float value);

	/**
	* Substract two quaternions
	**/
	static Quaternion Substract(const Quaternion& q1, const Quaternion& q2);

	/**
	* Substract float to quaternion member values
	**/
	static Quaternion SubstractValue(const Quaternion& quat, float value);

	/**
	* Multiply two quaternions
	**/
	static Quaternion Multiply(const Quaternion& q1, const Quaternion& q2);

	/**
	* Scale quaternion by float
	**/
	static Quaternion Scale(const Quaternion& quat, float scale);

	/**
	* Divide two quaternions by member values
	**/
	static Quaternion Divide(const Quaternion& q1, const Quaternion& q2);

	/**
	* Compute the dot product of two quaternions
	**/
	static float DotProduct(const Quaternion & q1, const Quaternion & q2);

	/**
	* Give the magnitude/length of the quaternion
	**/
	static float Magnitude(const Quaternion& quat);

	/**
	* Give the squared magnitude/length of the quaternion
	**/
	static float SquaredMagnitude(const Quaternion& quat);

	/**
	* Normalize the quaternion
	**/
	static Quaternion Normalize(const Quaternion& quat);

	/**
	* Invert the quaternion
	**/
	static Quaternion Invert(const Quaternion& quat);

	/**
	* Lerp rotation from q1 to q2
	**/
	static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float percent);

	/**
	* Lerp rotation normalized from q1 to q2
	**/
	static Quaternion NLerp(const Quaternion& q1, const Quaternion& q2, float percent);

	/**
	* Spherically Lerp rotation from q1 to q2
	**/
	static Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float percent);

	/**
	* Create quaternion rotation from vector3 to vector3
	**/
	static Quaternion FromVector3ToVector3(const Vector3& from, const Vector3& to);

	/**
	* Create quaternion rotation from axis and angle
	**/
	static Quaternion FromAxisAngle(const Vector3& axis, float angle);

	/**
	* Create quaternion rotation from a rotation matrix
	**/
	static Quaternion FromMatrix(const Matrix4 & mat);

	/**
	* Return axis and angle from quaternion rotation
	**/
	static void ToAxisAngle(const Quaternion& quat, Vector3& outAxis, float& outAngle);

	/**
	* Create quaternion from euler rotations
	**/
	static Quaternion FromEulerRad(float roll, float pitch, float yaw);

	/**
	* Create quaternion from euler rotation vector
	**/
	static Quaternion FromEulerRad(const Vector3& rotation);

	/**
	* Get euler angles from quaternion rotation
	**/
	static Vector3 ToEulerRad(const Quaternion& quat);

	/**
	* Rotate a vector by a quaternion rotation
	**/
	static Vector3 RotateVectorByQuaternion(const Vector3& vec, const Quaternion& quat);

	/**
	* Get rotation matrix from quaternion
	**/
	static Matrix4 ToMatrix(const Quaternion& quat);


	Quaternion operator*(const Quaternion& rhs) const;
	Vector3 operator*(const Vector3& rhs) const;

#pragma endregion

	Quaternion_GENERATED
};

#include "Quaternion.inl"

File_Quaternion_GENERATED
