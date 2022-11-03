
inline bool ResourcesTaskPool::IsRunning() const
{
	return !(m_multiTasks.empty() && m_runningThreadCount.load() == 0);
}