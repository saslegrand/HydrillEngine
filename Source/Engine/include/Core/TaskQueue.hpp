#pragma once

#include <deque>
#include <functional>

/**
Task class. Add any type of task and play on Invoke.
Meant to be use at the end of the loop to avoid deleting while lopping on an entity.
Tasks are stored in a deque, you can add important tasks that are stored at the end of the dequeue to played in priority
*/
class TaskQueue
{
public:
	using Task = std::function<void()>;

private:
	std::deque<Task> m_tasks;

public:
	TaskQueue() = default;
	TaskQueue(TaskQueue const& queue) = delete;
	TaskQueue(TaskQueue&& queue) = delete;

	TaskQueue& operator=(TaskQueue const& queue) = delete;
	TaskQueue& operator=(TaskQueue&& queue) = delete;

public:
	/**
	Add a new task to the queue.
	Call InvokeTasks to run all the tasks

	@param newTask : Task to add
	*/
	inline void AddTask(Task&& newTask);

	/**
	Add a new task at the end of the queue.
	Call InvokeTasks to run all the tasks

	@param newTask : Task to add
	*/
	inline void AddPriorityTask(Task&& newTask);

	/**
	Call all registered task from queue to head then clear the queue.
	*/
	inline void InvokeTasks();
};

#include "Core/TaskQueue.inl"