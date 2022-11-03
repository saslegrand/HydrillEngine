#pragma once

#include "Maths/Vector3.hpp"
#include "Maths/Vector2.hpp"

#include "Generated/Vector4.rfkh.h"

/**
* Vector4 class definition
*/
struct HY_STRUCT() ENGINE_API Vector4
{
	Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f);
	Vector4(const Vector2 & vecXY, const Vector2 & vecZW);
	Vector4(const Vector3& vec, float w = 0.f);

#pragma region _____PARAMETERS_____

	union
	{
		float elements[4];
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		struct {
			Vector2 xy;
			Vector2 zw;
		};

		Vector3 xyz;
	};

#pragma endregion


#pragma region _____STATICS_____

	/** Definition of world vector zero : (0, 0, 0, 0) */
	static Vector4 const Zero;

	/** Definition of world vector one : (1, 1, 1, 1) */
	static Vector4 const One;

	/**
	* Give the dot product between two vectors
	* @param lhs : first vector
	* @param rhs : second vector
	**/
	static float DotProduct(const Vector4& lhs, const Vector4& rhs);

	/**
	* Clamp vector values between min and max
	* @param vector : target
	* @param min	: min value
	* @param max	: max value
	**/
	static Vector4 Clamp(const Vector4& vector, const float min, const float max);

	/**
	* Clamp vector values between vectors min and max
	* @param vector : target
	* @param min	: min value
	* @param max	: max value
	**/
	static Vector4 Clamp(const Vector4& vector, const Vector4& min, const Vector4& max);

#pragma endregion


#pragma region _____FUNCTIONS_____

	/**
	* Check if the vector members are equals to another vector members with a tolerance
	* @param vector : target values
	* @param tolerance
	**/
	bool Equals(const Vector4& vector, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector members are equals to a value with a tolerance
	* @param value : target value
	* @param tolerance
	**/
	bool Equals(float value, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector is exactly zero (0, 0, 0, 0)
	**/
	bool IsZeroExactly() const;

	/**
	* Check if the vector is near zero, with a tolerance range(-tolerance, tolerance)
	* @param tolerance
	**/
	bool IsZeroAlmost(float tolerance = Maths::SmallFloat);

	/**
	* Give the max value from the vector member values
	**/
	float GetMax() const;

	/**
	* Give the min value from the vector member values
	**/
	float GetMin() const;

	/**
	* Give the max value from the absolute vector member values
	**/
	float GetAbsMax() const;

	/**
	* Give the min value from the absolute vector member values
	**/
	float GetAbsMin() const;


	/**
	* Give the vector with absolute member values
	**/
	Vector4 GetAbs() const;

	/**
	* Give the sign of member values
	**/
	Vector4 GetSign() const;

	/**
	* Give the magnitude/length of the vector
	**/
	float Magnitude() const;

	/**
	* Give the squared magnitude/length of the vector
	**/
	float SquaredMagnitude() const;

	/**
	* Normalize the vector
	**/
	void Normalize();

	/**
	* Give a copy of the vector normalized
	**/
	Vector4 SafeNormalize();

#pragma endregion


#pragma region _____OPERATORS_____

	/** Check if the vectors are equal */
	bool operator==(const Vector4& vector) const;

	/** Check weither vectors are not equal */
	bool operator!=(const Vector4& vector) const;

	/** Check if the vector member values are all equal to float */
	bool operator==(float InF) const;

	/** Check if the vector member values are not equal to float */
	bool operator!=(float InF) const;



	/** Adds two vectors */
	Vector4 operator+(const Vector4& vector) const;
	/** Adds two vectors and set left operand */
	Vector4 operator+=(const Vector4& vector);

	/** Adds float bias to vector member values */
	Vector4 operator+(float bias) const;
	/** Adds float bias to vector member values and set left operand*/
	Vector4 operator+=(float bias);


	/** Substract two vectors */
	Vector4 operator-(const Vector4& vector) const;

	/** Substract two vectors and set left operand */
	Vector4 operator-=(const Vector4& vector);

	/** Substract float bias to vector member values */
	Vector4 operator-(float bias) const;

	/** Substract float bias to vector member values and set left operand*/
	Vector4 operator-=(float bias);

	/** Negate the vector */
	Vector4 operator-() const;


	/**
	* Multiply operand two vectors by member values
	* #param rhs : multiplier
	*/
	Vector4 operator*(const Vector4& vector) const;

	/**
	* Multiply and set operand two vectors by member values
	* #param rhs : multiplier
	*/
	Vector4 operator*=(const Vector4& vector);

	/**
	* Multiply operand vector by a float
	* #param rhs : multiplier
	*/
	Vector4 operator*(float rhs) const;

	/**
	* Multiply and set operand vector by a float
	* #param rhs : multiplier
	*/
	Vector4 operator*=(float rhs);


	/**
	* Divide operand vector by a float
	* #param rhs : multiplier
	*/
	Vector4 operator/(float rhs) const;

	/**
	* Divide and set operand vector by a float
	* #param rhs : multiplier
	*/
	Vector4 operator/=(float rhs);


	/**
	* Give operand vector member reference from index
	* @param index
	*/
	float& operator[](int32 index);

	/**
	* Give operand vector member from index
	* @param index
	*/
	float  operator[](int32 index) const;

#pragma endregion

	Vector4_GENERATED
};

#include "Vector4.inl"

File_Vector4_GENERATED