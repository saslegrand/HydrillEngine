
inline bool BaseObject::IsActive() const
{
	return m_isActive;
}

inline void BaseObject::SetName(const std::string& name)
{
	m_name = name;
}

inline const std::string& BaseObject::GetName() const
{
	return m_name;
}

inline const HYGUID& BaseObject::GetUID() const
{
	return m_UID;
}