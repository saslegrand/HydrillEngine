#include "Core/ResourcesTaskPool.hpp"

#include <algorithm>

ResourcesTaskPool::ResourcesTaskPool()
{
	// Get/Set thread count in the pool
	uint32_t hardwareConcurency = std::thread::hardware_concurrency();
	m_threadCount = std::min(hardwareConcurency, m_threadCountLimit);

	// Create all threads
	for (uint32_t i = 0; i < m_threadCount; i++)
		m_threads.emplace_back(std::bind(&ResourcesTaskPool::ThreadRoutine, this));
}

ResourcesTaskPool::~ResourcesTaskPool()
{
	WaitingForMultiTasksCompletion();

	// ShutDown and notify all threads to end loop when owned task is over
	{
		std::unique_lock lq(m_queueMutex);
		m_shutDown = true;
		m_threadNotifier.notify_all();
	}

	// Threads join automatically (jthread)
}

void ResourcesTaskPool::WaitingForMultiTasksCompletion()
{
	// Wait for all tasks to be completed
	std::unique_lock lq(m_queueMutex);
	m_destroyNotifier.wait(lq, [&] { return !IsRunning(); });
}

void ResourcesTaskPool::AddMultiThreadTask(Task newTask)
{
	std::lock_guard queueGuard(m_queueMutex);

	// Add a new task to the queue
	m_multiTasks.push(newTask);

	// Notify thread that a new task has been added
	m_threadNotifier.notify_one();
}

void ResourcesTaskPool::AddSingleThreadTask(Task newTask)
{
	std::lock_guard queueGuard(m_queueMutex);

	// Add a new task to the queue
	m_singleTasks.push(newTask);
}

void ResourcesTaskPool::RunSingleTasks()
{
	while (!m_singleTasks.empty())
	{
		// Get task
		Task task = m_singleTasks.front();
		m_singleTasks.pop();

		// Run task
		task();
	}
}

void ResourcesTaskPool::ThreadRoutine()
{
	// While thread pool is running
	while (!m_shutDown)
	{
		// Wait for a new task or ppol shutdown
		std::unique_lock lq(m_queueMutex);

		// Check if the mutex has been successfully locked
		if (!lq.owns_lock())
			continue;

		// Wait for a new task and unlock queueMutex, relock when acquired
		m_threadNotifier.wait(lq, [&] { return m_shutDown || !m_multiTasks.empty(); });

		// Shutdown, cut the loop
		if (m_multiTasks.empty()) continue;

		// Add thread to the global count
		m_runningThreadCount.fetch_add(1);

		// Get the next task and unlock for other threads
		Task task = m_multiTasks.front();
		m_multiTasks.pop();

		// Unlock the mutex for the other threads
		lq.unlock();

		// Play task
		task();

		// Remove thread from global count
		m_runningThreadCount.fetch_sub(1);

		// Notify the destructor condition to kill the pool if needed
		m_destroyNotifier.notify_one();
	}

	// Log end thread
}