

inline Scene* SceneManager::GetCurrentScene() const
{
	if (m_curSceneIndex >= m_scenes.size())
		return nullptr;

	return m_scenes[m_curSceneIndex];
}