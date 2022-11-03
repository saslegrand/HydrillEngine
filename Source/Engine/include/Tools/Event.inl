#include "Event.hpp"

template <typename Class, typename ...Args>
inline ClassEventCallback<Class, void (Class::*)(Args...)>::ClassEventCallback(ClassEventCallback::FuncClass in_func, Class* in_class) noexcept
	: m_func(in_func), m_class(in_class) {}

template <typename Class, typename ...Args>
inline void ClassEventCallback<Class, void (Class::*)(Args...)>::operator()(Args... args) const noexcept
{
	return (m_class->*m_func)(args...);
};




template <typename ...Args>
inline FuncEventCallback<void (*)(Args...)>::FuncEventCallback(FuncEventCallback::FuncClass in_func) noexcept
	: m_func(in_func) {}

template <typename ...Args>
inline void FuncEventCallback<void (*)(Args...)>::operator()(Args... args) const noexcept
{
	return m_func(args...);
};



template <typename ...Args>
Event<Args...>::~Event()
{
	// Disable linked handles
	for (EventHandle<Args...>* handle : m_handles)
		handle->m_eventPtr = nullptr;
}

template <typename ...Args>
template <typename Class>
inline CallbackID Event<Args...>::AddCallback(void (Class::* func)(Args...), Class& obj) noexcept
{
	auto pair = m_callbacks.emplace_back(std::make_pair(m_globalID++, std::make_shared<ClassEventCallback<Class, void (Class::*)(Args...)>>(func, &obj)));
	return pair.first;
}

template <typename ...Args>
inline CallbackID Event<Args...>::AddCallback(void (*func)(Args...)) noexcept
{
	auto pair = m_callbacks.emplace_back(std::make_pair(m_globalID++, std::make_shared<FuncEventCallback<void (*)(Args...)>>(func)));
	return pair.first;
}

template <typename ...Args>
template <typename Class>
inline void Event<Args...>::AddCallback(void (Class::* func)(Args...), Class& obj, EventHandle<Args...>& handle) noexcept
{
	auto pair = m_callbacks.emplace_back(std::make_pair(m_globalID++, std::make_shared<ClassEventCallback<Class, void (Class::*)(Args...)>>(func, &obj)));

	// Set handle values
	handle.m_calbackID = pair.first;
	handle.m_eventPtr = this;

	// Store the handle for destruction
	m_handles.emplace_back(&handle);
}

template <typename ...Args>
inline void Event<Args...>::AddCallback(void (*func)(Args...), EventHandle<Args...>& handle) noexcept
{
	auto pair = m_callbacks.emplace_back(std::make_pair(m_globalID++, std::make_shared<FuncEventCallback<void (*)(Args...)>>(func)));

	handle.m_calbackID = pair.first;
	handle.m_eventPtr = this;
	m_handles.emplace_back(&handle);
}

template <typename ...Args>
inline bool Event<Args...>::RemoveCallback(CallbackID callbackID) noexcept
{
	// Find and remove callback from paired id
	for (size_t index = 0; index < m_callbacks.size(); index++)
	{
		if (m_callbacks[index].first == callbackID)
		{
			m_callbacks.erase(m_callbacks.begin() + index);
			return true;
		}
	}

	return false;
}

template <typename ...Args>
bool Event<Args...>::RemoveHandle(EventHandle<Args...>&& handle) noexcept
{
	// Find and remove callback from paired id
	for (size_t index = 0; index < m_handles.size(); index++)
	{
		if (m_handles[index]->m_calbackID == handle.m_calbackID)
		{
			m_handles.erase(m_handles.begin() + index);
			return true;
		}
	}

	return false;
}

template <typename ...Args>
inline void Event<Args...>::ClearCallbacks() noexcept
{
	if (!m_callbacks.empty())
		m_callbacks.clear();
}

template <typename ...Args>
inline void Event<Args...>::Invoke(Args... args) const noexcept
{
	if (m_callbacks.empty())
		return;

	for (auto& [key, callback] : m_callbacks)
		(*callback)(std::forward<Args>(args)...);
}


// HANDLE REGION
template <typename ...Args>
EventHandle<Args...>::EventHandle(CallbackID ID, Event<Args...>* eventPtr)
	: m_calbackID(ID), m_eventPtr(eventPtr) { }

template <typename ...Args>
EventHandle<Args...>::EventHandle(EventHandle&& handle) noexcept
{
	// Move the handle
	m_calbackID = handle.m_calbackID;
	m_eventPtr = handle.m_eventPtr;

	handle.m_eventPtr = nullptr;
}

template <typename ...Args>
EventHandle<Args...>::~EventHandle()
{
	// Auto remove the callback on the event
	if (m_eventPtr != nullptr)
	{
		m_eventPtr->RemoveHandle(std::move(*this));
		m_eventPtr->RemoveCallback(m_calbackID);
	}
}

template <typename ...Args>
EventHandle<Args...>& EventHandle<Args...>::operator=(EventHandle&& handle) noexcept
{
	// Destroy cuurent handle
	this->~EventHandle();

	// Move the handle
	m_calbackID = handle.m_calbackID;
	m_eventPtr = handle.m_eventPtr;

	handle.m_eventPtr = nullptr;

	return *this;
}