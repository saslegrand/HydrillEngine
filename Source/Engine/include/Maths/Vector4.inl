
MATH_INLINE Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w) {}

MATH_INLINE Vector4::Vector4(const Vector3& vec, float w)
	: x(vec.x), y(vec.y), z(vec.z), w(w) {}

MATH_INLINE Vector4::Vector4(const Vector2& vecXY, const Vector2& vecZW)
	: xy(vecXY), zw(vecZW) {}

// Static functions definitions
#pragma region STATIC_FUNCTIONS

MATH_INLINE float Vector4::DotProduct(const Vector4& lhs, const Vector4& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

MATH_INLINE Vector4 Vector4::Clamp(const Vector4& vector, const float min, const float max)
{
	return
	{
		Maths::Clamp(vector.x, min, max),
		Maths::Clamp(vector.y, min, max),
		Maths::Clamp(vector.z, min, max),
		Maths::Clamp(vector.w, min, max)
	};
}

MATH_INLINE Vector4 Vector4::Clamp(const Vector4& vector, const Vector4& min, const Vector4& max)
{
	return
	{
		Maths::Clamp(vector.x, min.x, max.x),
		Maths::Clamp(vector.y, min.y, max.y),
		Maths::Clamp(vector.z, min.z, max.z),
		Maths::Clamp(vector.w, min.w, max.w)
	};
}

#pragma endregion

#pragma region FUNCTIONS

MATH_INLINE bool Vector4::Equals(const Vector4& vector, float tolerance) const
{
	return Maths::Equal(this->x, vector.x, tolerance)
		&& Maths::Equal(this->y, vector.y, tolerance)
		&& Maths::Equal(this->z, vector.z, tolerance)
		&& Maths::Equal(this->w, vector.w, tolerance);
}

MATH_INLINE bool Vector4::Equals(float value, float tolerance) const
{
	bool isValid = true;

	isValid &= Maths::Equal(this->x, value, tolerance);
	isValid &= Maths::Equal(this->y, value, tolerance);
	isValid &= Maths::Equal(this->z, value, tolerance);
	isValid &= Maths::Equal(this->w, value, tolerance);

	return isValid;
}

MATH_INLINE bool Vector4::IsZeroExactly() const
{
	return (x == 0.f && y == 0.f && z == 0.f && w == 0.f);
}

MATH_INLINE bool Vector4::IsZeroAlmost(float tolerance)
{
	return Equals(0.f, tolerance);
}

MATH_INLINE float Vector4::GetMax() const
{
	return Maths::Max(Maths::Max(x, y), Maths::Max(z, w));
}

MATH_INLINE float Vector4::GetMin() const
{
	return Maths::Min(Maths::Min(x, y), Maths::Min(z, w));
}

MATH_INLINE float Vector4::GetAbsMax() const
{
	return Maths::Max(Maths::Max(Maths::Abs(x), Maths::Abs(y)), Maths::Max(Maths::Abs(z), Maths::Abs(w)));
}

MATH_INLINE float Vector4::GetAbsMin() const
{
	return Maths::Min(Maths::Min(Maths::Abs(x), Maths::Abs(y)), Maths::Min(Maths::Abs(z), Maths::Abs(w)));
}

MATH_INLINE Vector4 Vector4::GetAbs() const
{
	return { Maths::Abs(x), Maths::Abs(y), Maths::Abs(z), Maths::Abs(w) };
}

MATH_INLINE Vector4 Vector4::GetSign() const
{
	return { Maths::Sign(x), Maths::Sign(y), Maths::Sign(z), Maths::Sign(w) };
}

MATH_INLINE float Vector4::Magnitude() const
{
	return Maths::Sqrt(SquaredMagnitude());
}

MATH_INLINE float Vector4::SquaredMagnitude() const
{
	return Vector4::DotProduct(*this, *this);
}

MATH_INLINE void Vector4::Normalize()
{
	*this = SafeNormalize();
}

MATH_INLINE Vector4 Vector4::SafeNormalize()
{
	float length = Magnitude();

	if (length == 0.f)
		return *this;

	return *this / length;
}

#pragma endregion


// Operators definitions
#pragma region OPERATORS

MATH_INLINE bool Vector4::operator==(const Vector4& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
}

MATH_INLINE bool Vector4::operator!=(const Vector4& vector) const
{
	return (x != vector.x || y != vector.y || z != vector.z || w != vector.w);
}

MATH_INLINE bool Vector4::operator==(float value) const
{
	return (x == value && y == value && z == value && w == value);
}

MATH_INLINE bool Vector4::operator!=(float value) const
{
	return (x != value || y != value || z != value || w != value);
}


MATH_INLINE Vector4 Vector4::operator+(const Vector4& vector) const
{
	return { x + vector.x, y + vector.y, z + vector.z, w + vector.w};
}

MATH_INLINE Vector4 Vector4::operator+=(const Vector4& vector)
{
	return *this = *this + vector;
}

MATH_INLINE Vector4 Vector4::operator+(float bias) const
{
	return { x + bias, y + bias, z + bias, w + bias };
}

MATH_INLINE Vector4 Vector4::operator+=(float bias)
{
	return *this = *this + bias;
}


MATH_INLINE Vector4 Vector4::operator-() const
{
	return { -x, -y, -z, -w };
}


MATH_INLINE Vector4 Vector4::operator-(const Vector4& vector) const
{
	return { x - vector.x, y - vector.y, z - vector.z, w - vector.w };
}

MATH_INLINE Vector4 Vector4::operator-=(const Vector4& vector)
{
	return *this = *this - vector;
}

MATH_INLINE Vector4 Vector4::operator-(float bias) const
{
	return { x - bias, y - bias, z - bias, w - bias };
}

MATH_INLINE Vector4 Vector4::operator-=(float bias)
{
	return *this = *this - bias;
}


MATH_INLINE Vector4 Vector4::operator*(const Vector4& vector) const
{
	return { x * vector.x, y * vector.y, z * vector.z, w * vector.w };
}

MATH_INLINE Vector4 Vector4::operator*=(const Vector4& vector)
{
	return *this = *this * vector;
}

MATH_INLINE Vector4 Vector4::operator*(float value) const
{
	return { x * value, y * value, z * value, w * value };
}

MATH_INLINE Vector4 Vector4::operator*=(float rhs)
{
	return *this = *this * rhs;
}


MATH_INLINE Vector4 Vector4::operator/(float rhs) const
{
	if (rhs == 0.f)
		return Vector4(0.f, 0.f, 0.f, 0.f);

	float quot = 1.f / rhs;

	return Vector4(*this * quot);
}

MATH_INLINE Vector4 Vector4::operator/=(float rhs)
{
	return *this = *this / rhs;
}


MATH_INLINE float& Vector4::operator[](int32 index)
{
	if (index < 0 || index > 3)
		return x;

	return (&x)[index];
}

MATH_INLINE float Vector4::operator[](int32 index) const
{
	if (index < 0 || index > 3)
		return x;

	return (&x)[index];
}

#pragma endregion