
MATH_INLINE Vector3 Matrix3::Multiply(const Matrix3& left, const Vector3& right)
{
	Vector3 vec = Vector3::Zero;

	for (int c = 0; c < 3; ++c)
		for (int r = 0; r < 3; ++r)
			vec[c] += left.columns[c][r] * right[r];

	return vec;
}

MATH_INLINE Vector3 Matrix3::operator*(const Vector3& rhs) const
{
	return Multiply(*this, rhs);
}

MATH_INLINE Matrix3 Matrix3::operator*(float rhs) const
{
	Matrix3 out = *this;

	for (int i = 0; i < 9; i++)
	{
		out.elements[i] *= rhs;
	}

	return out;
}
