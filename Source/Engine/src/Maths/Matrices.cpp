#include "Generated/Matrix3.rfks.h"
#include "Generated/Matrix4.rfks.h"

Matrix4 const Matrix4::Identity =
{
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

Matrix3 const Matrix3::Identity =
{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
};

Matrix4 Matrix3::ToMatrix4(const Matrix3& mat3)
{
	return {
		mat3.elements[0], mat3.elements[1], mat3.elements[2], 0.f,
		mat3.elements[3], mat3.elements[4], mat3.elements[5], 0.f,
		mat3.elements[6], mat3.elements[7], mat3.elements[8], 0.f,
			0.f,				0.f,			0.f,		  1.f
	};
}

Matrix3 Matrix4::ToMatrix3(const Matrix4& mat4)
{
	return {
		mat4.elements[0], mat4.elements[1], mat4.elements[2],
		mat4.elements[4], mat4.elements[5], mat4.elements[6],
		mat4.elements[8], mat4.elements[9], mat4.elements[10]
	};
}