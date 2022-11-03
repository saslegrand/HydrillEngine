
MATH_INLINE Quaternion Quaternion::Add(const Quaternion& q1, const Quaternion& q2)
{
	return { q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w };
}

MATH_INLINE Quaternion Quaternion::AddValue(const Quaternion& quat, float value)
{
	return { quat.x + value, quat.y + value, quat.z + value, quat.w + value };
}

MATH_INLINE Quaternion Quaternion::Substract(const Quaternion& q1, const Quaternion& q2)
{
	return { q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w };
}

MATH_INLINE Quaternion Quaternion::SubstractValue(const Quaternion& quat, float value)
{
	return { quat.x - value, quat.y - value, quat.z - value, quat.w - value };
}

MATH_INLINE float Quaternion::DotProduct(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

MATH_INLINE Quaternion Quaternion::Multiply(const Quaternion& q1, const Quaternion& q2)
{
	float qax = q1.x, qay = q1.y, qaz = q1.z, qaw = q1.w;
	float qbx = q2.x, qby = q2.y, qbz = q2.z, qbw = q2.w;
	return
	{
		qax * qbw + qaw * qbx + qay * qbz - qaz * qby,
		qay * qbw + qaw * qby + qaz * qbx - qax * qbz,
		qaz * qbw + qaw * qbz + qax * qby - qay * qbx,
		qaw * qbw - qax * qbx - qay * qby - qaz * qbz
	};
}

MATH_INLINE Quaternion Quaternion::Scale(const Quaternion& quat, float scale)
{
	float qax = quat.x, qay = quat.y, qaz = quat.z, qaw = quat.w;
	return
	{
		qax * scale + qaw * scale + qay * scale - qaz * scale,
		qay * scale + qaw * scale + qaz * scale - qax * scale,
		qaz * scale + qaw * scale + qax * scale - qay * scale,
		qaw * scale - qax * scale - qay * scale - qaz * scale,
	};
}

MATH_INLINE Quaternion Quaternion::Divide(const Quaternion& q1, const Quaternion& q2)
{
	return  { q1.x / q2.x, q1.y / q2.y, q1.z / q2.z, q1.w / q2.w };
}

MATH_INLINE float Quaternion::Magnitude(const Quaternion& quat)
{
	return Vector4{ quat.x, quat.y, quat.z, quat.w }.Magnitude();
}

MATH_INLINE float Quaternion::SquaredMagnitude(const Quaternion& quat)
{
	return Vector4{ quat.x, quat.y, quat.z, quat.w }.SquaredMagnitude();
}

MATH_INLINE Quaternion Quaternion::Normalize(const Quaternion& quat)
{
	float length = Quaternion::Magnitude(quat);
	if (Maths::Equal(length, 0.0f))
		length = 1.0f;

	float ilength = 1.0f / length;

	return
	{
		quat.x * ilength,
		quat.y * ilength,
		quat.z * ilength,
		quat.w * ilength
	};
}

MATH_INLINE Quaternion Quaternion::Invert(const Quaternion& quat)
{
	float lengthSq = Quaternion::SquaredMagnitude(quat);
	if (Maths::Equal(lengthSq, 0.f))
		return quat;

	float i = 1.0f / lengthSq;
	return
	{
		quat.x * -i,
		quat.y * -i,
		quat.z * -i,
		quat.w * i
	};
}

MATH_INLINE Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float percent)
{
	return
	{
		q1.x + percent * (q2.x - q1.x),
		q1.y + percent * (q2.y - q1.y),
		q1.z + percent * (q2.z - q1.z),
		q1.w + percent * (q2.w - q1.w)
	};
}

MATH_INLINE Quaternion Quaternion::NLerp(const Quaternion& q1, const Quaternion& q2, float percent)
{
	return Quaternion::Normalize(Quaternion::Lerp(q1, q2, percent));
}

MATH_INLINE Quaternion Quaternion::SLerp(const Quaternion& q1, const Quaternion& q2, float percent)
{
	Quaternion result = { 0 };
	Quaternion dest = q2;

	float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

	if (cosHalfTheta < 0)
	{
		dest.x = -dest.x; dest.y = -dest.y; dest.z = -dest.z; dest.w = -dest.w;
		cosHalfTheta = -cosHalfTheta;
	}

	if (Maths::Abs(cosHalfTheta) >= 1.0f) result = q1;
	else if (cosHalfTheta > 0.95f) result = Quaternion::NLerp(q1, dest, percent);
	else
	{
		float halfTheta = Maths::Acos(cosHalfTheta);
		float sinHalfTheta = Maths::Sqrt(1.0f - cosHalfTheta * cosHalfTheta);

		if (Maths::Abs(sinHalfTheta) < 0.001f)
		{
			result.x = (q1.x * 0.5f + dest.x * 0.5f);
			result.y = (q1.y * 0.5f + dest.y * 0.5f);
			result.z = (q1.z * 0.5f + dest.z * 0.5f);
			result.w = (q1.w * 0.5f + dest.w * 0.5f);
		}
		else
		{
			float ratioA = Maths::Sin((1 - percent) * halfTheta) / sinHalfTheta;
			float ratioB = Maths::Sin(percent * halfTheta) / sinHalfTheta;

			result.x = (q1.x * ratioA + dest.x * ratioB);
			result.y = (q1.y * ratioA + dest.y * ratioB);
			result.z = (q1.z * ratioA + dest.z * ratioB);
			result.w = (q1.w * ratioA + dest.w * ratioB);
		}
	}

	return result;
}

MATH_INLINE Quaternion Quaternion::FromVector3ToVector3(const Vector3& from, const Vector3& to)
{
	Quaternion result = { 0 };

	float cos2Theta = Vector3::DotProduct(from, to);
	Vector3 cross = Vector3::CrossProduct(from, to);

	result.x = cross.x;
	result.y = cross.y;
	result.z = cross.z;
	result.w = 1.0f + cos2Theta;

	result = Quaternion::Normalize(result);

	return result;
}

MATH_INLINE Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
	Quaternion result = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (axis.Magnitude() != 0.0f)
		angle *= 0.5f;

	Vector3 newAxis = axis.SafeNormalized();

	float sinres = Maths::Sin(angle);
	float cosres = Maths::Cos(angle);

	result.x = newAxis.x * sinres;
	result.y = newAxis.y * sinres;
	result.z = newAxis.z * sinres;
	result.w = cosres;

	result = Quaternion::Normalize(result);

	return result;
}

MATH_INLINE void Quaternion::ToAxisAngle(const Quaternion& quat, Vector3& outAxis, float& outAngle)
{
	Quaternion q = quat;
	if (fabs(q.w) > 1.0f) q = Quaternion::Normalize(quat);

	Vector3 resAxis = { 0.0f, 0.0f, 0.0f };
	float resAngle = 2.0f * Maths::Acos(q.w);
	float den = Maths::Sqrt(1.0f - q.w * q.w);

	if (den > 0.0001f)
	{
		resAxis.x = q.x / den;
		resAxis.y = q.y / den;
		resAxis.z = q.z / den;
	}
	else
	{
		// When angle is 0, we take an axis we don't care
		resAxis.x = 1.0f;
	}

	outAxis = resAxis;
	outAngle = resAngle;
}




MATH_INLINE Quaternion Quaternion::FromEulerRad(float roll, float pitch, float yaw)
{
	const float cy = Maths::Cos(yaw * 0.5f);
	const float sy = Maths::Sin(yaw * 0.5f);
	const float cp = Maths::Cos(pitch * 0.5f);
	const float sp = Maths::Sin(pitch * 0.5f);
	const float cr = Maths::Cos(roll * 0.5f);
	const float sr = Maths::Sin(roll * 0.5f);

	Quaternion q;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;
	q.w = cr * cp * cy + sr * sp * sy;

	return q;
}

MATH_INLINE Quaternion Quaternion::FromEulerRad(const Vector3& rotation)
{
	return Quaternion::FromEulerRad(rotation.x, rotation.y, rotation.z);
}

MATH_INLINE Vector3 Quaternion::ToEulerRad(const Quaternion& quat)
{
	Vector3 angles;
	{
		const float z2 = quat.z * quat.z;

		{
			const float sinXcosZ = 2.0f * (quat.w * quat.x + quat.y * quat.z);
			const float cosXcosZ = 1.0f - 2.0f * (quat.x * quat.x + z2);
			angles.x = Maths::Atan2(sinXcosZ, cosXcosZ);
		}
		{
			const float sinYcosZ = 2.f * ((quat.w * quat.y) + (quat.x * quat.z));
			const float cosYcosZ = 1.f - 2.f * (z2 + (quat.y * quat.y));
			angles.y = atan2f(sinYcosZ, cosYcosZ);
		}
		{
			const float sinZ = 2.f * ((quat.w * quat.z) - (quat.y * quat.x));

			if (Maths::Abs(sinZ) >= 1.f)
			{
				angles.z = copysignf(Maths::PIHalf, sinZ);
			}
			else
			{
				angles.z = Maths::Asin(sinZ);
			}
		}
	}

	return angles;
}

MATH_INLINE Quaternion Quaternion::FromMatrix(const Matrix4& mat)
{
	Vector3 s;
	s.x = Vector3{ mat.columns[0][0], mat.columns[0][1], mat.columns[0][2] }.Magnitude();
	s.y = Vector3{ mat.columns[1][0], mat.columns[1][1], mat.columns[1][2] }.Magnitude();
	s.z = Vector3{ mat.columns[2][0], mat.columns[2][1], mat.columns[2][2] }.Magnitude();

	float m00 = mat.columns[0][0] / s.x;
	float m01 = mat.columns[0][1] / s.y;
	float m02 = mat.columns[0][2] / s.z;
	float m10 = mat.columns[1][0] / s.x;
	float m11 = mat.columns[1][1] / s.y;
	float m12 = mat.columns[1][2] / s.z;
	float m20 = mat.columns[2][0] / s.x;
	float m21 = mat.columns[2][1] / s.y;
	float m22 = mat.columns[2][2] / s.z;

	Quaternion q;
	q.w = Maths::Sqrt(Maths::Max(0.0f, 1.0f + m00 + m11 + m22)) / 2;
	q.x = Maths::Sqrt(Maths::Max(0.0f, 1.0f + m00 - m11 - m22)) / 2;
	q.y = Maths::Sqrt(Maths::Max(0.0f, 1.0f - m00 + m11 - m22)) / 2;
	q.z = Maths::Sqrt(Maths::Max(0.0f, 1.0f - m00 - m11 + m22)) / 2;
	q.x *= Maths::Sign(q.x * (m21 - m12));
	q.y *= Maths::Sign(q.y * (m02 - m20));
	q.z *= Maths::Sign(q.z * (m10 - m01));
	q = Quaternion::Normalize(q);
	return q;
}

MATH_INLINE Vector3 Quaternion::RotateVectorByQuaternion(const Vector3& vec, const Quaternion& quat)
{
	return
	{
		vec.x* (quat.x * quat.x + quat.w * quat.w - quat.y * quat.y - quat.z * quat.z) + vec.y * (2 * quat.x * quat.y - 2 * quat.w * quat.z) + vec.z * (2 * quat.x * quat.z + 2 * quat.w * quat.y),
		vec.x* (2 * quat.w * quat.z + 2 * quat.x * quat.y) + vec.y * (quat.w * quat.w - quat.x * quat.x + quat.y * quat.y - quat.z * quat.z) + vec.z * (-2 * quat.w * quat.x + 2 * quat.y * quat.z),
		vec.x* (-2 * quat.w * quat.y + 2 * quat.x * quat.z) + vec.y * (2 * quat.w * quat.x + 2 * quat.y * quat.z) + vec.z * (quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z)
	};
}

MATH_INLINE Matrix4 Quaternion::ToMatrix(const Quaternion& quat)
{
	return
	{
		1 - 2 * quat.y * quat.y - 2 * quat.z * quat.z,  2 * quat.x * quat.y + 2 * quat.z * quat.w,      2 * quat.x * quat.z - 2 * quat.y * quat.w,      0.f,
		2 * quat.x * quat.y - 2 * quat.z * quat.w,      1 - 2 * quat.x * quat.x - 2 * quat.z * quat.z,  2 * quat.y * quat.z + 2 * quat.x * quat.w,      0.f,
		2 * quat.x * quat.z + 2 * quat.y * quat.w,      2 * quat.y * quat.z - 2 * quat.x * quat.w,      1 - 2 * quat.x * quat.x - 2 * quat.y * quat.y,  0.f,
		0.f,                                0.f,                                0.f,                                1.f
	};
}

MATH_INLINE Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
	return Quaternion::Multiply(*this, rhs);
}

MATH_INLINE Vector3 Quaternion::operator*(const Vector3& rhs) const
{
	return Quaternion::RotateVectorByQuaternion(rhs, *this);
}