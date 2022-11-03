
constexpr int32_t Matrix_Dimension = 4;

MATH_INLINE Matrix4 Matrix4::Translate(const Vector3& translation)
{
	return
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		translation.x, translation.y, translation.z, 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::Scale(const Vector3& scale)
{
	return
	{
		scale.x, 0.f, 0.f, 0.f,
		0.f, scale.y, 0.f, 0.f,
		0.f, 0.f, scale.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::RotateX(float angleRadians)
{
	float C = Maths::Cos(angleRadians);
	float S = Maths::Sin(angleRadians);
	return
	{
		1.f, 0.f, 0.f, 0.f,
		0.f,   C,   S, 0.f,
		0.f,  -S,   C, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::RotateY(float angleRadians)
{
	float C = Maths::Cos(angleRadians);
	float S = Maths::Sin(angleRadians);

	return
	{
		  C, 0.f,   S, 0.f,
		0.f, 1.f, 0.f, 0.f,
		 -S, 0.f,   C, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::RotateZ(float angleRadians)
{
	float C = Maths::Cos(angleRadians);
	float S = Maths::Sin(angleRadians);
	return
	{
		  C,   S, 0.f, 0.f,
		 -S,   C, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::Add(const Matrix4& left, const Matrix4& right)
{
	Matrix4 mat;

	for (int32_t c = 0; c < Matrix_Dimension; ++c)
		for (int32_t r = 0; r < Matrix_Dimension; ++r)
			mat.columns[c][r] = left.columns[c][r] + right.columns[c][r];

	return mat;
}

MATH_INLINE Matrix4 Matrix4::Subtract(const Matrix4& left, const Matrix4& right)
{
	Matrix4 mat;

	for (int32_t c = 0; c < Matrix_Dimension; ++c)
		for (int32_t r = 0; r < Matrix_Dimension; ++r)
			mat.columns[c][r] = left.columns[c][r] - right.columns[c][r];

	return mat;
}

MATH_INLINE Matrix4 Matrix4::Multiply(const Matrix4& left, const Matrix4& right)
{
	Matrix4 mat;

	for (int32_t c = 0; c < Matrix_Dimension; ++c)
	{
		float* newMatColumn = mat.columns[c];
		const float* rightColumn = right.columns[c];

		for (int32_t r = 0; r < Matrix_Dimension; ++r)
		{
			float& newMatValue = newMatColumn[r];
			for (int32_t k = 0; k < Matrix_Dimension; ++k)
			{
				newMatValue += left.columns[k][r] * rightColumn[k];
			}
		}
	}

	return mat;
}

MATH_INLINE Vector4 Matrix4::Multiply(const Matrix4& left, const Vector4& right)
{
	Vector4 vec = Vector4::Zero;

	for (int32_t c = 0; c < Matrix_Dimension; ++c)
	{
		float& newVecValue = vec[c];
		const float* leftColumn = left.columns[c];

		for (int32_t r = 0; r < Matrix_Dimension; ++r)
		{
			newVecValue += leftColumn[r] * right[r];
		}
	}
		

	return vec;
}

MATH_INLINE Vector4 Matrix4::Multiply(const Vector4& left, const Matrix4& right)
{
	Vector4 vec = Vector4::Zero;

	for (int32_t c = 0; c < 4; ++c)
	{
		float& newValue = vec[c];
		for (int32_t r = 0; r < 4; ++r)
		{
			newValue += right.columns[r][c] * left[r];
		}
	}
		

	return vec;
}

MATH_INLINE Matrix4 Matrix4::Transpose(const Matrix4& matrix)
{
	return {
			matrix.columns[0][0], matrix.columns[1][0], matrix.columns[2][0], matrix.columns[3][0],
			matrix.columns[0][1], matrix.columns[1][1], matrix.columns[2][1], matrix.columns[3][1],
			matrix.columns[0][2], matrix.columns[1][2], matrix.columns[2][2], matrix.columns[3][2],
			matrix.columns[0][3], matrix.columns[1][3], matrix.columns[2][3], matrix.columns[3][3]
	};
}

MATH_INLINE Matrix4 Matrix4::Inverse(const Matrix4& matrix)
{
	Matrix4 R;

	float S[6];
	S[0] = matrix.columns[0][0] * matrix.columns[1][1] - matrix.columns[1][0] * matrix.columns[0][1];
	S[1] = matrix.columns[0][0] * matrix.columns[1][2] - matrix.columns[1][0] * matrix.columns[0][2];
	S[2] = matrix.columns[0][0] * matrix.columns[1][3] - matrix.columns[1][0] * matrix.columns[0][3];
	S[3] = matrix.columns[0][1] * matrix.columns[1][2] - matrix.columns[1][1] * matrix.columns[0][2];
	S[4] = matrix.columns[0][1] * matrix.columns[1][3] - matrix.columns[1][1] * matrix.columns[0][3];
	S[5] = matrix.columns[0][2] * matrix.columns[1][3] - matrix.columns[1][2] * matrix.columns[0][3];

	float C[6];
	C[0] = matrix.columns[2][0] * matrix.columns[3][1] - matrix.columns[3][0] * matrix.columns[2][1];
	C[1] = matrix.columns[2][0] * matrix.columns[3][2] - matrix.columns[3][0] * matrix.columns[2][2];
	C[2] = matrix.columns[2][0] * matrix.columns[3][3] - matrix.columns[3][0] * matrix.columns[2][3];
	C[3] = matrix.columns[2][1] * matrix.columns[3][2] - matrix.columns[3][1] * matrix.columns[2][2];
	C[4] = matrix.columns[2][1] * matrix.columns[3][3] - matrix.columns[3][1] * matrix.columns[2][3];
	C[5] = matrix.columns[2][2] * matrix.columns[3][3] - matrix.columns[3][2] * matrix.columns[2][3];

	float div = (S[0] * C[5] - S[1] * C[4] + S[2] * C[3] + S[3] * C[2] - S[4] * C[1] + S[5] * C[0]);

	// If it is not invertible
	if (Maths::Equal(div, 0.f))
		return matrix;

	float InvDet = 1.0f / div;

	R.columns[0][0] = +(matrix.columns[1][1] * C[5] - matrix.columns[1][2] * C[4] + matrix.columns[1][3] * C[3]) * InvDet;
	R.columns[0][1] = -(matrix.columns[0][1] * C[5] - matrix.columns[0][2] * C[4] + matrix.columns[0][3] * C[3]) * InvDet;
	R.columns[0][2] = +(matrix.columns[3][1] * S[5] - matrix.columns[3][2] * S[4] + matrix.columns[3][3] * S[3]) * InvDet;
	R.columns[0][3] = -(matrix.columns[2][1] * S[5] - matrix.columns[2][2] * S[4] + matrix.columns[2][3] * S[3]) * InvDet;

	R.columns[1][0] = -(matrix.columns[1][0] * C[5] - matrix.columns[1][2] * C[2] + matrix.columns[1][3] * C[1]) * InvDet;
	R.columns[1][1] = +(matrix.columns[0][0] * C[5] - matrix.columns[0][2] * C[2] + matrix.columns[0][3] * C[1]) * InvDet;
	R.columns[1][2] = -(matrix.columns[3][0] * S[5] - matrix.columns[3][2] * S[2] + matrix.columns[3][3] * S[1]) * InvDet;
	R.columns[1][3] = +(matrix.columns[2][0] * S[5] - matrix.columns[2][2] * S[2] + matrix.columns[2][3] * S[1]) * InvDet;

	R.columns[2][0] = +(matrix.columns[1][0] * C[4] - matrix.columns[1][1] * C[2] + matrix.columns[1][3] * C[0]) * InvDet;
	R.columns[2][1] = -(matrix.columns[0][0] * C[4] - matrix.columns[0][1] * C[2] + matrix.columns[0][3] * C[0]) * InvDet;
	R.columns[2][2] = +(matrix.columns[3][0] * S[4] - matrix.columns[3][1] * S[2] + matrix.columns[3][3] * S[0]) * InvDet;
	R.columns[2][3] = -(matrix.columns[2][0] * S[4] - matrix.columns[2][1] * S[2] + matrix.columns[2][3] * S[0]) * InvDet;

	R.columns[3][0] = -(matrix.columns[1][0] * C[3] - matrix.columns[1][1] * C[1] + matrix.columns[1][2] * C[0]) * InvDet;
	R.columns[3][1] = +(matrix.columns[0][0] * C[3] - matrix.columns[0][1] * C[1] + matrix.columns[0][2] * C[0]) * InvDet;
	R.columns[3][2] = -(matrix.columns[3][0] * S[3] - matrix.columns[3][1] * S[1] + matrix.columns[3][2] * S[0]) * InvDet;
	R.columns[3][3] = +(matrix.columns[2][0] * S[3] - matrix.columns[2][1] * S[1] + matrix.columns[2][2] * S[0]) * InvDet;

	return R;
}

MATH_INLINE Matrix4 Matrix4::Frustum(float left, float right, float bottom, float top, float near, float far)
{
	return
	{
		(near * 2.f) / (right - left),   0.f,                              0.f,                               0.f,
		0.f,                             (near * 2.f) / (top - bottom),  0.f,                               0.f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near),      -1.f,
		0.f,                             0.f,                             -(far * near * 2.f) / (far - near), 0.f
	};
}

MATH_INLINE Matrix4 Matrix4::Orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	float rightLeft = 1 / (right - left);
	float topBottom = 1 / (top - bottom);
	float farNear = 1 / (far - near);

	float a = 2 * rightLeft;
	float b = 2 * topBottom;
	float c = -2 * farNear;

	float d = -(right + left) * rightLeft;
	float e = -(top + bottom) * topBottom;
	float f = -(far + near) * farNear;

	//  Return 4x4 matrix
	return {
		a   ,   0.f ,   0.f ,   0.f ,
		0.f ,   b   ,   0.f ,   0.f ,
		0.f ,   0.f ,   c   ,   0.f ,
		d   ,   e   ,   f   ,   1.f ,
	};
}

MATH_INLINE Matrix4 Matrix4::Perspective(float fovY, float aspcet, float near, float far)
{
	float Top = near * tanf(fovY / 2.f);
	float Right = Top * aspcet;
	return Matrix4::Frustum(-Right, Right, -Top, Top, near, far);
}

MATH_INLINE Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
	Vector3 ZAxis = Vector3::Normalize(at - eye);
	Vector3 XAxis = Vector3::Normalize(Vector3::CrossProduct(ZAxis, up));
	Vector3 YAxis = Vector3::CrossProduct(XAxis, ZAxis);

	return {
		XAxis.x, YAxis.x, -ZAxis.x, 0.f,
		XAxis.y, YAxis.y, -ZAxis.y, 0.f,
		XAxis.z, YAxis.z, -ZAxis.z, 0.f,
		-Vector3::DotProduct(XAxis, eye), -Vector3::DotProduct(YAxis, eye), Vector3::DotProduct(ZAxis, eye), 1.f
	};
}

MATH_INLINE Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
	return Multiply(*this, rhs);
}

MATH_INLINE Matrix4 Matrix4::operator*(float rhs) const
{
	Matrix4 out = *this;

	for (int i = 0; i < 16; i++)
	{
		out.elements[i] *= rhs;
	}

	return out;
}

MATH_INLINE Vector4 Matrix4::operator*(const Vector4& rhs) const
{
	return Multiply(*this, rhs);
}

MATH_INLINE Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	return *this = Multiply(*this, rhs);
}

MATH_INLINE Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	return Add(*this, rhs);
}

MATH_INLINE Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
	return *this = Add(*this, rhs);
}

MATH_INLINE Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	return Subtract(*this, rhs);
}

MATH_INLINE Matrix4& Matrix4::operator-=(const Matrix4& rhs)
{
	return *this = Subtract(*this, rhs);
}