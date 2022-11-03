#pragma once

#include "Maths/Vector4.hpp"

#include "Generated/Matrix4.rfkh.h"

struct Matrix3;

/**
* Mat4 class definition
*/
struct HY_STRUCT() ENGINE_API Matrix4
{
#pragma region _____PARAMETERS_____

	union
	{
		float elements[16]{ 0.f };
		float columns[4][4];
	};

#pragma endregion

#pragma region _____STATICS_____

	/**
	* Get the identity matrix (no translation, no rotation, one scale)
	* @return Identity matrix
	**/
	static Matrix4 const Identity;

	/**
	* @brief Transform a matrix4 into a matrix3
	* @param mat4 : matrix4 to transform
	**/
	static Matrix3 ToMatrix3(const Matrix4& mat4);

	/**
	* @return Translation matrix from vector
	* @param T : Translation vector
	**/
	static Matrix4 Translate(const Vector3 & translation);

	/**
	* @return Scale matrix from vector
	* @param S : Scale vector
	**/
	static Matrix4 Scale(const Vector3 & scale);

	/**
	* @return Rotation matrix in the X-axis
	* @param AngleRadians : Angle rotation (in radians)
	**/
	static Matrix4 RotateX(float angleRadians);

	/**
	* @return Rotation matrix in the Y-axis
	* @param AngleRadians : Angle rotation (in radians)
	**/
	static Matrix4 RotateY(float angleRadians);

	/**
	* @return Rotation matrix in the Z-axis
	* @param AngleRadians : Angle rotation (in radians)
	**/
	static Matrix4 RotateZ(float angleRadians);

	/**
	* @return Addition of two matrix
	* @param left : Left matrix
	* @param right : Right matrix
	**/
	static Matrix4 Add(const Matrix4 & left, const Matrix4 & right);

	/**
	* @return Substraction of two matrix
	* @param left : Left matrix
	* @param right : Right matrix
	**/
	static Matrix4 Subtract(const Matrix4 & left, const Matrix4 & right);

	/**
	* @return Multiplication of two matrix
	* @param left : Left matrix
	* @param right : Right matrix
	**/
	static Matrix4 Multiply(const Matrix4 & left, const Matrix4 & right);


	/**
	* @return Multiplication of a matrix and a vector
	* @param left : Left matrix
	* @param right : Right vector
	**/
	static Vector4 Multiply(const Matrix4& left, const Vector4& right);

	/**
	* @return Multiplication of a vector and a matrix
	* @param left : Left vector
	* @param right : Right matrix
	**/
	static Vector4 Multiply(const Vector4& left, const Matrix4& right);

	/**
	* Transpose the given matrix
	* @return Transpose matrix (diagnoal mirror)
	* @param M : Matrix to transpose
	**/
	static Matrix4 Transpose(const Matrix4 & matrix);

	/**
	* Inverse the given matrix
	* @return Inverse matrix
	* @param M : Matrix to inverse
	**/
	static Matrix4 Inverse(const Matrix4 & matrix);

	/**
	* Compute the orthographic matrix
	* @return Orthographic matrix
	* @param Left : Left distance
	* @param Right : Right distance
	* @param Bottom : Bottom distance
	* @param Top : Top distance
	* @param Near : Frustum start distance
	* @param Far : Frustum end distance
	**/
	static Matrix4 Orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far);
	
	/**
	* Compute the frustum matrix
	* @return Frustum matrix
	* @param Left : Left distance
	* @param Right : Right distance
	* @param Bottom : Bottom distance
	* @param Top : Top distance
	* @param Near : Frustum start distance
	* @param Far : Frustum end distance
	**/
	static Matrix4 Frustum(float left, float right, float bottom, float top, float near, float far);


	/**
	* Compute the perspective matrix from camera parameters
	* @return Perspective matrix
	* @param FovY   : Camera FOV factor
	* @param Aspect : Screen Ratio
	* @param Near   : Frustum start distance
	* @param Far    : Frustum end distance
	**/
	static Matrix4 Perspective(float fovY, float aspect, float near, float far);

	/**
	* LookAt matrix
	* @param Eye : Position to look from
	* @param At : Position to look at
	* @param Up : Up vector of the looker
	**/
	static Matrix4 LookAt(const Vector3 & eye, const Vector3 & at, const Vector3 & up);

#pragma endregion

#pragma region _____OPERATORS_____

	/**
	* Multiply operand on matrix
	*/
	Matrix4 operator*(const Matrix4 & rhs) const;

	/**
	* Multiply operand with float
	*/
	Matrix4 operator*(float rhs) const;

	/**
	* Multiply operand with Vector4
	*/
	Vector4 operator*(const Vector4& rhs) const;

	/**
	* Multiply and set operand on matrix
	*/
	Matrix4& operator*=(const Matrix4 & rhs);

	/**
	* Add operand on matrix
	*/
	Matrix4 operator+(const Matrix4 & rhs) const;

	/**
	* Add and set operand on matrix
	*/
	Matrix4& operator+=(const Matrix4 & rhs);

	/**
	* Subtract operand on matrix
	*/
	Matrix4 operator-(const Matrix4 & rhs) const;

	/**
	* Subtract and set operand on matrix
	*/
	Matrix4& operator-=(const Matrix4 & rhs);

#pragma endregion

	Matrix4_GENERATED
};

#include "Matrix4.inl"

File_Matrix4_GENERATED