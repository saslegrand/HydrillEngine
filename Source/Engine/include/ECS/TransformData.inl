
inline Vector3 TransformData::Right(const Matrix4& mat)
{
    return { mat.columns[0][0], mat.columns[0][1], mat.columns[0][2] };
}

inline Vector3 TransformData::Left(const Matrix4& mat)
{
    return -Right(mat);
}

inline Vector3 TransformData::Up(const Matrix4& mat)
{
    return { mat.columns[1][0], mat.columns[1][1], mat.columns[1][2] };
}

inline Vector3 TransformData::Down(const Matrix4& mat)
{
    return -Up(mat);
}

inline Vector3 TransformData::Forward(const Matrix4& mat)
{
    return -Back(mat);
}

inline Vector3 TransformData::Back(const Matrix4& mat)
{
    return { mat.columns[2][0], mat.columns[2][1], mat.columns[2][2] };
}


inline Vector3 TransformData::Translation(const Matrix4& mat)
{
    return { mat.columns[3][0], mat.columns[3][1], mat.columns[3][2] };
}

inline Quaternion TransformData::Rotation(const Matrix4& mat)
{
    return Quaternion::FromMatrix(mat);
}

inline Matrix4 TransformData::RotationMatrix(const Matrix4& mat)
{
    const Vector3 invScale = { 1.f / Right(mat).Magnitude(), 1.f / Up(mat).Magnitude(), 1.f / Back(mat).Magnitude() };

    return
    {
        mat.elements[0] * invScale.x, mat.elements[1] * invScale.x, mat.elements[2] * invScale.x, 0.f,
        mat.elements[4] * invScale.y, mat.elements[5] * invScale.y, mat.elements[6] * invScale.y, 0.f,
        mat.elements[8] * invScale.z, mat.elements[9] * invScale.z, mat.elements[10] * invScale.z, 0.f,
        0.f, 0.f, 0.f, 0.f
    };
}


inline Vector3 TransformData::Scaling(const Matrix4& mat)
{
    return { Right(mat).Magnitude(), Up(mat).Magnitude(), Back(mat).Magnitude() };
}

inline void TransformData::ComputeLocalTRS()
{
    // Translate

    localMatrix = Matrix4::Translate(position) *
                  Quaternion::ToMatrix(rotation) *
                  Matrix4::Scale(scale);
}

inline void TransformData::DecomposeMatrix(const Matrix4& mat, Vector3& position, Quaternion& rotation, Vector3& scale)
{
    position = Translation(mat);
    rotation = Quaternion::FromMatrix(mat);
    scale = Scaling(mat);
}