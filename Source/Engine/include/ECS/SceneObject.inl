
inline bool SceneObject::IsActive() const
{
	return m_isActive;
}

inline void SceneObject::SetName(const std::string& name)
{
	m_name = name;
}

inline const std::string& SceneObject::GetName() const
{
	return m_name.string();
}

inline const HYGUID& SceneObject::GetUID() const
{
	return m_UID;
}