#pragma once

#include "Maths/Vector2.hpp"

#include "Generated/Vector3.rfkh.h"

/**
* Vector3 class definition
*/
struct HY_STRUCT() ENGINE_API Vector3
{
	Vector3(float x = 0.f, float y = 0.f, float z = 0.f);
	Vector3(const Vector2& vec, float z = 0.f);

#pragma region _____PARAMETERS_____

	union
	{
		float elements[3];
		struct {
			float x;
			float y;
			float z;
		};
		Vector2 xy;
	};

#pragma endregion


#pragma region _____STATICS_____

	/** Definition of world vector up : (0, 1, 0) */
	static Vector3 const Up;

	/** Definition of world vector down : (0, -1, 0) */
	static Vector3 const Down;

	/** Definition of world vector right : (1, 0, 0) */
	static Vector3 const Right;

	/** Definition of world vector left : (-1, 0, 0) */
	static Vector3 const Left;

	/** Definition of world vector forward : (0, 0, 1) */
	static Vector3 const Forward;

	/** Definition of world vector back : (0, 0, -1) */
	static Vector3 const Back;

	/** Definition of world vector zero : (0, 0, 0) */
	static Vector3 const Zero;

	/** Definition of world vector one : (1, 1, 1) */
	static Vector3 const One;

	/**
	* Give the dot product between two vectors
	* @param lhs : first vector
	* @param rhs : second vector
	**/
	static float DotProduct(const Vector3& lhs, const Vector3& rhs);

	/**
	* Give the cross product between two vectors
	* @param lhs : first vector
	* @param rhs : second vector
	**/
	static Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs);

	/**
	* Clamp vector values between min and max
	* @param vector : target
	* @param min	: min value
	* @param max	: max value
	**/
	static Vector3 Clamp(const Vector3& vector, const float min, const float max);

	/**
	* Clamp vector values between vectors min and max
	* @param vector : target
	* @param min	: min value
	* @param max	: max value
	**/
	static Vector3 Clamp(const Vector3& vector, const Vector3& min, const Vector3& max);

	/**
	* Return the angle in degrees between two vectors (not signed)
	* @param lhs : first vector
	* @param rhs : second vector
	* @param normalize : norm entry vectors
	**/
	static float AngleBetween(const Vector3& lhs, const Vector3& rhs, bool normalize = false);

	/**
	* Return a copy of the vector normalized
	* @param vector : target
	**/
	static Vector3 Normalize(const Vector3& vector);

#pragma endregion


#pragma region _____FUNCTIONS_____

	/**
	* Check if the vector members are equals to another vector members with a tolerance
	* @param vector : target values
	* @param tolerance
	**/
	bool Equals(const Vector3& vector, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector members are equals to a value with a tolerance
	* @param value : target value
	* @param tolerance
	**/
	bool Equals(float value, float tolerance = Maths::SmallFloat) const;

	/**
	* Check if the vector is null (0, 0)
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
	Vector3 GetAbs() const;

	/**
	* Give the sign of member values
	**/
	Vector3 GetSign() const;

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
	Vector3 SafeNormalized() const;

#pragma endregion


#pragma region _____OPERATORS_____

	/**
	* Check if the vectors are equal
	*/
	bool operator==(const Vector3& vector) const;

	/**
	* Check weither vectors are not equal
	*/
	bool operator!=(const Vector3& vector) const;

	/**
	* Check if the vector member values are all equal to float
	*/
	bool operator==(float InF) const;

	/**
	* Check if the vector member values are not equal to float
	*/
	bool operator!=(float InF) const;



	/**
	* Adds two vectors
	*/
	Vector3 operator+(const Vector3& vector) const;
	/**
	* Adds two vectors and set left operand
	*/
	Vector3 operator+=(const Vector3& vector);

	/**
	* Adds float bias to vector member values
	*/
	Vector3 operator+(float bias) const;

	/**
	* Adds float bias to vector member values and set left 
	*/
	Vector3 operator+=(float bias);


	/**
	* Substract two vectors
	*/
	Vector3 operator-(const Vector3& vector) const;

	/**
	* Substract two vectors and set left operand
	*/
	Vector3 operator-=(const Vector3& vector);

	/**
	* Substract float bias to vector member values
	*/
	Vector3 operator-(float bias) const;

	/**
	* Substract float bias to vector member values and set left operand
	*/
	Vector3 operator-=(float bias);

	/**
	* Negate the vector
	*/
	Vector3 operator-() const;


	/**
	* Multiply operand two vectors by member values
	* #param rhs : multiplier
	*/
	Vector3 operator*(const Vector3& vector) const;

	/**
	* Multiply and set operand two vectors by member values
	* #param rhs : multiplier
	*/
	Vector3 operator*=(const Vector3& vector);

	/**
	* Multiply operand vector by a float
	* #param rhs : multiplier
	*/
	Vector3 operator*(float rhs) const;

	/**
	* Multiply and set operand vector by a float
	* #param rhs : multiplier
	*/
	Vector3 operator*=(float rhs);

	/**
	* Divide operand vector by a vector
	* #param rhs : multiplier
	*/
	Vector3 operator/(const Vector3& vector) const;

	/**
	* Divide operand vector by a float
	* #param rhs : multiplier
	*/
	Vector3 operator/(float rhs) const;

	/**
	* Divide and set operand vector by a float
	* #param rhs : multiplier
	*/
	Vector3 operator/=(float rhs);


	/**
	* Give operand vector member reference from index
	* @param index
	*/
	float& operator[](int32 index);

	/**
	* Give operand vector member from index
	* @param index
	*/
	float operator[](int32 index) const;

#pragma endregion

	Vector3_GENERATED
};

#include "Vector3.inl"

File_Vector3_GENERATED