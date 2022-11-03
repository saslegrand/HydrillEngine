
MATH_INLINE Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z) {}

MATH_INLINE Vector3::Vector3(const Vector2& vec, float z)
	: x(vec.x), y(vec.y), z(z) {}

// Static functions definitions
#pragma region STATIC_FUNCTIONS

MATH_INLINE float Vector3::DotProduct(const Vector3& lhs, const Vector3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

MATH_INLINE Vector3 Vector3::CrossProduct(const Vector3& lhs, const Vector3& rhs)
{
	return
	{
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x
	};
}

MATH_INLINE Vector3 Vector3::Clamp(const Vector3& vector, const float min, const float max)
{
	return
	{
		Maths::Clamp(vector.x, min, max),
		Maths::Clamp(vector.y, min, max),
		Maths::Clamp(vector.z, min, max)
	};
}

MATH_INLINE Vector3 Vector3::Clamp(const Vector3& vector, const Vector3& min, const Vector3& max)
{
	return
	{
		Maths::Clamp(vector.x, min.x, max.x),
		Maths::Clamp(vector.y, min.y, max.y),
		Maths::Clamp(vector.z, min.z, max.z)
	};
}

MATH_INLINE float Vector3::AngleBetween(const Vector3& lhs, const Vector3& rhs, bool normalize)
{
	Vector3 _lhs = lhs, _rhs = rhs;

	if (normalize)
	{
		_lhs.Normalize();
		_rhs.Normalize();
	}

	return Maths::Acos(Vector3::DotProduct(_lhs, _rhs)) * (_lhs.x > _rhs.x ? -1.f : 1.f) * Maths::RADTODEG;
}

MATH_INLINE Vector3 Vector3::Normalize(const Vector3& vector)
{
	return vector.SafeNormalized();
}

#pragma endregion

#pragma region FUNCTIONS

MATH_INLINE bool Vector3::Equals(const Vector3& vector, float tolerance) const
{
	return Maths::Equal(this->x, vector.x, tolerance)
		&& Maths::Equal(this->y, vector.y, tolerance)
		&& Maths::Equal(this->z, vector.z, tolerance);
}

MATH_INLINE bool Vector3::Equals(float value, float tolerance) const
{
	bool isValid = true;

	isValid &= Maths::Equal(this->x, value, tolerance);
	isValid &= Maths::Equal(this->y, value, tolerance);
	isValid &= Maths::Equal(this->z, value, tolerance);

	return isValid;
}

MATH_INLINE bool Vector3::IsZeroExactly() const
{
	return (x == 0.f && y == 0.f && z == 0.f);
}

MATH_INLINE bool Vector3::IsZeroAlmost(float tolerance)
{
	return Equals(0.f, tolerance);
}

MATH_INLINE float Vector3::GetMax() const
{
	return Maths::Max(x, Maths::Max(y, z));
}

MATH_INLINE float Vector3::GetMin() const
{
	return Maths::Min(x, Maths::Min(y, z));
}

MATH_INLINE float Vector3::GetAbsMax() const
{
	return Maths::Max(Maths::Abs(x), Maths::Max(Maths::Abs(y), Maths::Abs(z)));
}

MATH_INLINE float Vector3::GetAbsMin() const
{
	return Maths::Min(Maths::Abs(x), Maths::Min(Maths::Abs(y), Maths::Abs(z)));
}

MATH_INLINE Vector3 Vector3::GetAbs() const
{
	return { Maths::Abs(x), Maths::Abs(y), Maths::Abs(z) };
}

MATH_INLINE Vector3 Vector3::GetSign() const
{
	return { Maths::Sign(x), Maths::Sign(y), Maths::Sign(z) };
}

MATH_INLINE float Vector3::Magnitude() const
{
	return Maths::Sqrt(SquaredMagnitude());
}

MATH_INLINE float Vector3::SquaredMagnitude() const
{
	return Vector3::DotProduct(*this, *this);
}

MATH_INLINE void Vector3::Normalize()
{
	*this = SafeNormalized();
}

MATH_INLINE Vector3 Vector3::SafeNormalized() const
{
	float length = Magnitude();

	if (length == 0.f)
		return *this;

	return *this / length;
}

#pragma endregion


// Operators definitions
#pragma region OPERATORS

MATH_INLINE bool Vector3::operator==(const Vector3& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z);
}

MATH_INLINE bool Vector3::operator!=(const Vector3& vector) const
{
	return (x != vector.x || y != vector.y || z != vector.z);
}

MATH_INLINE bool Vector3::operator==(float value) const
{
	return (x == value && y == value && z == value);
}

MATH_INLINE bool Vector3::operator!=(float value) const
{
	return (x != value || y != value || z != value);
}


MATH_INLINE Vector3 Vector3::operator+(const Vector3& vector) const
{
	return { x + vector.x, y + vector.y, z + vector.z };
}

MATH_INLINE Vector3 Vector3::operator+=(const Vector3& vector)
{
	return *this = *this + vector;
}

MATH_INLINE Vector3 Vector3::operator+(float bias) const
{
	return { x + bias, y + bias, z + bias };
}

MATH_INLINE Vector3 Vector3::operator+=(float bias)
{
	return *this = *this + bias;
}


MATH_INLINE Vector3 Vector3::operator-() const
{
	return { -x, -y, -z };
}


MATH_INLINE Vector3 Vector3::operator-(const Vector3& vector) const
{
	return { x - vector.x, y - vector.y, z - vector.z };
}

MATH_INLINE Vector3 Vector3::operator-=(const Vector3& vector)
{
	return *this = *this - vector;
}

MATH_INLINE Vector3 Vector3::operator-(float bias) const
{
	return { x - bias, y - bias, z - bias };
}

MATH_INLINE Vector3 Vector3::operator-=(float bias)
{
	return *this = *this - bias;
}


MATH_INLINE Vector3 Vector3::operator*(const Vector3& vector) const
{
	return { x * vector.x, y * vector.y, z * vector.z };
}

MATH_INLINE Vector3 Vector3::operator*=(const Vector3& vector)
{
	return *this = *this * vector;
}

MATH_INLINE Vector3 Vector3::operator*(float value) const
{
	return { x * value, y * value, z * value };
}

MATH_INLINE Vector3 Vector3::operator*=(float rhs)
{
	return *this = *this * rhs;
}

MATH_INLINE Vector3 Vector3::operator/(const Vector3& vector) const
{
	Vector3 vect = *this;
	if (!Maths::Equal<float>(vector.x, 0)) vect.x /= vector.x;
	if (!Maths::Equal<float>(vector.y, 0)) vect.y /= vector.y;
	if (!Maths::Equal<float>(vector.z, 0)) vect.z /= vector.z;
	return vect;
}

MATH_INLINE Vector3 Vector3::operator/(float rhs) const
{
	if (rhs == 0.f)
		return Vector3(0.f, 0.f, 0.f);

	float quot = 1.f / rhs;

	return Vector3(*this * quot);
}

MATH_INLINE Vector3 Vector3::operator/=(float rhs)
{
	return *this = *this / rhs;
}


MATH_INLINE float& Vector3::operator[](int32 index)
{
	if (index < 0 || index > 2)
		return x;

	return (&x)[index];
}

MATH_INLINE float Vector3::operator[](int32 index) const
{
	if (index < 0 || index > 2)
		return x;

	return (&x)[index];
}

#pragma endregion