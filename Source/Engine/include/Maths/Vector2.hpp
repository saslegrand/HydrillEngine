#pragma once

#include "Maths/Maths.hpp"

#include "Generated/Vector2.rfkh.h"

/**
* Vector2 class definition
*/
struct HY_STRUCT() ENGINE_API Vector2
{
	Vector2(float x = 0.f, float y = 0.f);

#pragma region _____PARAMETERS_____

	union
	{
		float elements[2];
		struct {
			float x;
			float y;
		};
	};

#pragma endregion


#pragma region _____STATICS_____

	/** Definition of world vector up : (0, 1) */
	static Vector2 const Up;

	/** Definition of world vector down : (0, -1) */
	static Vector2 const Down;

	/** Definition of world vector left : (-1, 0) */
	static Vector2 const Left;

	/** Definition of world vector right : (1, 0) */
	static Vector2 const Right;

	/** Definition of world vector zero : (0, 0) */
	static Vector2 const Zero;

	/** Definition of world vector one : (1, 1)	*/
	static Vector2 const One;

	/**
	* Give the dot product between two vectors
	* @param lhs : first vector
	* @param rhs : second vector
	**/
	static float DotProduct(const Vector2& lhs, const Vector2& rhs);

	/**
	* Clamp vector values between min and max
	* @param vector : target
	* @param min	: min value
	* @param max	: max value
	**/
	static Vector2 Clamp(const Vector2& vector, const float min, const float max);

	/**
	* Return the signed angle between two vectors
	* @param lhs : first vector
	* @param rhs : second vector
	* @param normalize : norm entry vectors
	**/
	static float AngleBetween(const Vector2& lhs, const Vector2& rhs, bool normalize = false);

#pragma endregion


#pragma region _____FUNCTIONS_____

	/**
	* Check if the vector members are equals to another vector members with a tolerance
	* @param vector : target values
	* @param tolerance
	**/
	bool Equals(const Vector2& vector, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector members are equals to a value with a tolerance
	* @param value : target value
	* @param tolerance
	**/
	bool Equals(float value, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector is vector zero (0, 0)
	**/
	bool IsZeroExactly() const;

	/**
	* Check if the vector is null with a tolerance range(-tolerance, tolerance)
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
	Vector2 GetAbs() const;

	/**
	* Give the sign of member values
	**/
	Vector2 GetSign() const;

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
	Vector2 SafeNormalize();

#pragma endregion


#pragma region _____OPERATORS_____

	/** Check if the vectors are equal */
	bool operator==(const Vector2& vector) const;

	/** Check weither vectors are not equal */
	bool operator!=(const Vector2& vector) const;

	/** Adds two vectors */
	Vector2 operator+(const Vector2& vector) const;

	/** Adds two vectors and set left operand */
	Vector2 operator+=(const Vector2& vector);

	/** Substract two vectors */
	Vector2 operator-(const Vector2& vector) const;

	/** Substract two vectors and set left operand */
	Vector2 operator-=(const Vector2& vector);

	/** Negate the vector */
	Vector2 operator-() const;

	/**
	* Multiply operand vector by a float
	* @param rhs : multiplier
	*/
	Vector2 operator*(float rhs) const;

	/**
	* Multiply and set operand vector by a float
	* @param rhs : multiplier
	*/
	Vector2 operator*=(float rhs);

	/**
	* Divide operand vector by a float
	* @param rhs : multiplier
	*/
	Vector2 operator/(float rhs) const;

	/**
	* Divide and set operand vector by a float
	* @param rhs : multiplier
	*/
	Vector2 operator/=(float rhs);

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

	Vector2_GENERATED
};

#include "Vector2.inl"

File_Vector2_GENERATED