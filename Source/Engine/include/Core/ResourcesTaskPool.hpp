#pragma once

#include <thread>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <functional>

/**
This class allow to multithread the resources loading. The task pool is divided between
the single task (meant to be used in the main thread) and the multi task (task given to a random thread).
*/
class ResourcesTaskPool
{
	using Task = std::function<void()>;

private:
	// Mutex to synchronize access to task queue
	std::mutex m_queueMutex;
	std::condition_variable m_threadNotifier;
	std::condition_variable m_destroyNotifier;

	uint32_t m_threadCountLimit = 8;
	uint32_t m_threadCount = 0;

	std::atomic<uint32_t> m_runningThreadCount = { 0 };

	std::queue<Task> m_multiTasks;
	std::queue<Task> m_singleTasks;
	std::vector<std::jthread> m_threads;

	bool m_shutDown = false;


public:
	ResourcesTaskPool();
	~ResourcesTaskPool();

private:
	/**
	@brief Base routine for every thread created
	*/
	void ThreadRoutine();

public:
	/**
	Add a new task multithreaded task to the task pool.
	This task will be given to a random thread in the task pool.

	@param newTask : Task to add
	*/
	void AddMultiThreadTask(Task newTask);

	/**
	Add a new main thread task to the thread pool.
	This task will be added to the monothread tasks.
	Call RunSingleTask to run all the monothread tasks (meant to be called in the main thread)

	@param newTask : Task to add
	*/
	void AddSingleThreadTask(Task newTask);

	/**
	@brief Loop over all added single tasks in the caller thread
	*/
	void RunSingleTasks();

	/**
	@brief Check if the task pool is currently running. (multi tasks empty and no thread running)
	*/
	bool IsRunning() const;

	/**
	@brief Wait till the task pool is running
	*/
	void WaitingForMultiTasksCompletion();
};

#include "Core/ResourcesTaskPool.inl"