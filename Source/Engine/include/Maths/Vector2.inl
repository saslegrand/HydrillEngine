
MATH_INLINE Vector2::Vector2(float x, float y)
	: x(x), y(y) {}

// Static functions definitions
#pragma region STATIC_FUNCTIONS

MATH_INLINE float Vector2::DotProduct(const Vector2& lhs, const Vector2& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

MATH_INLINE Vector2 Vector2::Clamp(const Vector2& vector, const float min, const float max)
{
	return
	{
		Maths::Clamp(vector.x, min, max),
		Maths::Clamp(vector.y, min, max)
	};
}

MATH_INLINE float Vector2::AngleBetween(const Vector2& lhs, const Vector2& rhs, bool normalize)
{
	Vector2 _lhs = lhs, _rhs = rhs;

	if (normalize)
	{
		_lhs.Normalize();
		_rhs.Normalize();
	}

	return Maths::Acos(Vector2::DotProduct(_lhs, _rhs)) * (_lhs.x > _rhs.x ? -1.f : 1.f) * Maths::RADTODEG;
}

#pragma endregion


#pragma region FUNCTIONS

MATH_INLINE bool Vector2::Equals(const Vector2& vector, float tolerance) const
{
	bool isValid = true;

	isValid &= Maths::Equal(this->x, vector.x, tolerance);
	isValid &= Maths::Equal(this->y, vector.y, tolerance);

	return isValid;
}

MATH_INLINE bool Vector2::Equals(float value, float tolerance) const
{
	bool isValid = true;

	isValid &= Maths::Equal(this->x, value, tolerance);
	isValid &= Maths::Equal(this->y, value, tolerance);

	return isValid;
}

MATH_INLINE bool Vector2::IsZeroExactly() const
{
	return (x == 0.f && y == 0.f);
}

MATH_INLINE bool Vector2::IsZeroAlmost(float tolerance)
{
	return Equals(0.f, tolerance);
}

MATH_INLINE float Vector2::GetMax() const
{
	return Maths::Max(x, y);
}

MATH_INLINE float Vector2::GetMin() const
{
	return Maths::Min(x, y);
}

MATH_INLINE float Vector2::GetAbsMax() const
{
	return Maths::Max(Maths::Abs(x), Maths::Abs(y));
}

MATH_INLINE float Vector2::GetAbsMin() const
{
	return Maths::Min(Maths::Abs(x), Maths::Abs(y));
}

MATH_INLINE Vector2 Vector2::GetAbs() const
{
	return { Maths::Abs(x), Maths::Abs(y) };
}

MATH_INLINE Vector2 Vector2::GetSign() const
{
	return { Maths::Sign(x), Maths::Sign(y) };
}

// Return the length of the vector
MATH_INLINE float Vector2::Magnitude() const
{
	return Maths::Sqrt(SquaredMagnitude());
}

// Return the squared length of the vector
MATH_INLINE float Vector2::SquaredMagnitude() const
{
	return Vector2::DotProduct(*this, *this);
}

// Give a copy of the vector normalized
MATH_INLINE Vector2 Vector2::SafeNormalize()
{
	float length = Magnitude();

	if (length == 0.f)
		return Vector2::Zero;

	return *this / length;
}

// Normalize itself
MATH_INLINE void Vector2::Normalize()
{
	*this = SafeNormalize();
}

#pragma endregion

// Operators definitions
#pragma region OPERATORS
MATH_INLINE bool Vector2::operator==(const Vector2& vector) const
{
	return (x == vector.x && y == vector.y);
}

MATH_INLINE bool Vector2::operator!=(const Vector2& vector) const
{
	return (x != vector.x || y != vector.y);
}


MATH_INLINE Vector2 Vector2::operator+(const Vector2& vector) const
{
	return { x + vector.x, y + vector.y };
}

MATH_INLINE Vector2 Vector2::operator+=(const Vector2& vector)
{
	return *this = *this + vector;
}


MATH_INLINE Vector2 Vector2::operator-() const
{
	return { -x, -y };
}

MATH_INLINE Vector2 Vector2::operator-(const Vector2& vector) const
{
	return { x - vector.x, y - vector.y };
}

MATH_INLINE Vector2 Vector2::operator-=(const Vector2& vector)
{
	return *this = *this - vector;
}


MATH_INLINE Vector2 Vector2::operator*(float rhs) const
{
	return { x * rhs, y * rhs };
}

MATH_INLINE Vector2 Vector2::operator*=(float rhs)
{
	return *this = *this * rhs;
}


MATH_INLINE Vector2 Vector2::operator/(float rhs) const
{
	if (rhs == 0.f)
		return Vector2(0.f, 0.f);

	float quot = 1.f / rhs;

	return Vector2(*this * quot);
}

MATH_INLINE Vector2 Vector2::operator/=(float rhs)
{
	return *this = *this / rhs;
}


MATH_INLINE float& Vector2::operator[](int32 index)
{
	if (index < 0 || index > 1)
		return x;

	return (&x)[index];
}

MATH_INLINE float Vector2::operator[](int32 index) const
{
	if (index < 0 || index > 1)
		return x;

	return (&x)[index];
}

#pragma endregion