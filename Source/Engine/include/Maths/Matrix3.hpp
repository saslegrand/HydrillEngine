#pragma once

#include "Maths/Maths.hpp"
#include "Vector3.hpp"

#include "Generated/Matrix3.rfkh.h"

struct Matrix4;

/**
* Mat3 class definition
*/
struct HY_STRUCT() ENGINE_API Matrix3
{
#pragma region _____PARAMETERS_____
	union
	{
		float elements[9]{ 0.f };
		float columns[3][3];
	};
#pragma endregion

#pragma region _____STATICS_____
	/**
	* Get the identity matrix (no translation, no rotation, one scale)
	* @return Identity matrix
	**/
	static Matrix3 const Identity;

	/**
	* @brief Transform a matrix3 into a matrix4
	* @param mat3 : matrix3 to transform
	**/
	static Matrix4 ToMatrix4(const Matrix3& mat3);

	/**
	* @return Multiplication of a matrix and a vector
	* @param left : Left matrix
	* @param right : Right vector
	**/
	static Vector3 Multiply(const Matrix3& left, const Vector3& right);

#pragma endregion

#pragma region _____OPERATORS_____

	/**
	* Multiply operand with Vector3
	*/
	Vector3 operator*(const Vector3& rhs) const;

	/**
	* Multiply operand with float
	*/
	Matrix3 operator*(float rhs) const;

#pragma endregion

	Matrix3_GENERATED
};

#include "Maths/Matrix3.inl"

File_Matrix3_GENERATED