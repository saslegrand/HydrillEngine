
// Operators for dirty flag
inline int operator| (EDirtyFlags lhs, EDirtyFlags rhs) { return static_cast<int>(lhs) | static_cast<int>(rhs); }
inline int operator| (int lhs, EDirtyFlags rhs) { return lhs | static_cast<int>(rhs); }
inline void operator|= (int& lhs, EDirtyFlags rhs) { lhs = lhs | rhs; }
inline int operator& (EDirtyFlags lhs, EDirtyFlags rhs) { return static_cast<int>(lhs) & static_cast<int>(rhs); }
inline int operator& (int lhs, EDirtyFlags rhs) { return lhs & static_cast<int>(rhs); }

inline const Vector3& Transform::LocalPosition() const
{
	return data.position;
}

inline Quaternion Transform::LocalRotation() const
{
	return data.rotation;
}

inline const Vector3& Transform::LocalScale() const
{
	return data.scale;
}

inline Vector3 Transform::EulerAngles() const
{
	return Quaternion::ToEulerRad(Rotation()) * Maths::RADTODEG;
}

inline Vector3 Transform::LocalEulerAngles() const
{
	return Quaternion::ToEulerRad(data.rotation) * Maths::RADTODEG;
}


inline void Transform::SetLocalPosition(const Vector3& newPosition)
{
	data.position = newPosition;
	SetDirty(EDirtyFlags::Local);
}

inline void Transform::SetLocalRotation(const Quaternion& newRotation)
{
	data.rotation = newRotation;
	SetDirty(EDirtyFlags::Local);
}

inline void Transform::SetLocalEulerAngles(const Vector3& newRotation)
{
	data.rotation = Quaternion::FromEulerRad(newRotation * Maths::DEGTORAD);
	SetDirty(EDirtyFlags::Local);
}

inline void Transform::SetLocalScale(const Vector3& newScale)
{
	if (newScale == Vector3::Zero) return;

	data.scale = newScale;
	SetDirty(EDirtyFlags::Local);
}


inline Vector3 Transform::Back() const
{
	return data.Back(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::Forward() const
{
	return data.Forward(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::Right() const
{
	return data.Right(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::Left() const
{
	return data.Left(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::Up() const
{
	return data.Up(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::Down() const
{
	return data.Down(data.worldMatrix).SafeNormalized();
}

inline Vector3 Transform::LocalBack() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Back, data.rotation);
}

inline Vector3 Transform::LocalForward() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Forward, data.rotation);
}

inline Vector3 Transform::LocalRight() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Right, data.rotation);
}

inline Vector3 Transform::LocalLeft() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Left, data.rotation);
}

inline Vector3 Transform::LocalUp() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Up, data.rotation);
}

inline Vector3 Transform::LocalDown() const
{
	return Quaternion::RotateVectorByQuaternion(Vector3::Down, data.rotation);
}


inline Matrix4& Transform::GetWorldMatrix()
{
	return data.worldMatrix;
}

inline Matrix4& Transform::GetLocalMatrix()
{
	return data.localMatrix;
}

inline void Transform::ResetDirtyFlag()
{
	m_dirtyFlag = 0;
}

inline bool Transform::IsDirty() const
{
	return static_cast<bool>(m_dirtyFlag);
}

inline void Transform::SetDirty(EDirtyFlags flag)
{
	m_dirtyFlag |= flag;
}