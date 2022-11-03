
inline void TaskQueue::AddTask(Task&& newTask)
{
	m_tasks.emplace_back(std::forward<Task>(newTask));
}

inline void TaskQueue::AddPriorityTask(Task&& newTask)
{
	m_tasks.emplace_front(std::forward<Task>(newTask));
}

inline void TaskQueue::InvokeTasks()
{
	// Run all registered tasks
	while (!m_tasks.empty())
	{
		Task task = m_tasks.front();	// Get the front task of the list
		m_tasks.pop_front();			// Remove the task

		task();							// Run the task
	}
}