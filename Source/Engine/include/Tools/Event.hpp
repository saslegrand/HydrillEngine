#pragma once
#include <memory>

#include "Tools/ReflectedSTD.hpp"

typedef unsigned int CallbackID;

template <typename Func>
class EventCallback;

/**
@brief Abstract event callback
*/
template <typename ...Args>
class EventCallback<void (*)(Args...)>
{
public:
	// Default basic constuctor
	EventCallback() {}
	virtual ~EventCallback() = default;

	// Delete copy and rValue constructors
	EventCallback(const EventCallback&) = delete;
	EventCallback(EventCallback&&) = delete;

	EventCallback& operator=(const EventCallback&) = delete;
	EventCallback& operator=(EventCallback&&) noexcept = delete;

	/**
	@brief Abstract operator() to call associated function
	*/
	virtual void operator()(Args... args) const noexcept = 0;
};


template <typename T, typename Func>
class ClassEventCallback;

/**
@brief Callback of a member function from it's parent class

@param m_class : object reference
@param m_func  : object function pointer
*/
template <typename Class, typename ...Args>
class ClassEventCallback<Class, void (Class::*)(Args...)> final : public EventCallback<void (*)(Args...)>
{
	using FuncClass = void (Class::*)(Args...);

public:
	~ClassEventCallback() final = default;

	// Delete copy and rValue constructors
	ClassEventCallback(const ClassEventCallback&) = delete;
	ClassEventCallback(ClassEventCallback&&) = delete;

	// Delete copy and rValue operator=
	ClassEventCallback& operator=(const ClassEventCallback&) = delete;
	ClassEventCallback& operator=(ClassEventCallback&&) noexcept = delete;

	// Default constructor
	ClassEventCallback(FuncClass in_func, Class* in_class) noexcept;

	/**
	@brief Call the associated member function from ptr class

	@param args : function parameters
	*/
	void operator()(Args... args) const noexcept final;

private:
	FuncClass	m_func;
	Class* m_class;
};


template <typename Func>
class FuncEventCallback;

/**
@brief Callback of a simple free function

@param m_func  : free function
*/
template <typename ...Args>
class FuncEventCallback<void (*)(Args...)> final : public EventCallback<void (*)(Args...)>
{
	using FuncClass = void (*)(Args...);

public:
	~FuncEventCallback() final = default;

	// Delete copy and rValue constructors
	FuncEventCallback(const FuncEventCallback&) = delete;
	FuncEventCallback(FuncEventCallback&&) = delete;

	// Delete copy and rValue operator=
	FuncEventCallback& operator=(const FuncEventCallback&) = delete;
	FuncEventCallback& operator=(FuncEventCallback&&) noexcept = delete;

	// Default constructor
	FuncEventCallback(FuncClass in_func) noexcept;

	/**
	@brief Call the associated member function from ptr class

	@param args : function parameters
	*/
	void operator()(Args... args) const noexcept final;

private:
	FuncClass	m_func;
};


template <typename ...Args>
struct EventHandle;

/**
@brief Event class, store callbacks an invoke with selected parameters

@param callbacks : All callbacks registered
*/
template <typename ...Args>
class Event
{
	using Callback = std::shared_ptr<EventCallback<void (*)(Args...)>>;

private:
	CallbackID m_globalID = 1;

	HyVector<std::pair<CallbackID, Callback>> m_callbacks;
	HyVector<EventHandle<Args...>*> m_handles;

public:
	~Event();

	/**
	@brief Add new function class callback

	@param func : member function
	@param obj  : origin class
	*/
	template <typename Class>
	CallbackID AddCallback(void (Class::* func)(Args...), Class& obj) noexcept;

	/**
	@brief Add new function callback

	@param func : function
	*/
	CallbackID AddCallback(void (*func)(Args...)) noexcept;

	/**
	@brief Add new function class callback and link the event to an handle

	@param func : member function
	@param obj  : origin class
	@param handle  : event handle
	*/
	template <typename Class>
	void AddCallback(void (Class::* func)(Args...), Class& obj, EventHandle<Args...>& handle) noexcept;

	/**
	@brief Add new function callback and link the event to an handle

	@param func : function
	@param obj  : origin class
	@param handle  : event handle
	*/
	void AddCallback(void (*func)(Args...), EventHandle<Args...>& handle) noexcept;

	/**
	@brief Delete a callback from it's ID

	@param callbackID : Id of the callback targeted
	@param bool : true if succeeded, false otherwise
	*/
	bool RemoveCallback(CallbackID callbackID) noexcept;

	/**
	@brief Delete an handle link to the event (for destruction)

	@param handle : Handle to remove
	*/
	bool RemoveHandle(EventHandle<Args...>&& handle) noexcept;

	/**
	@brief Delete all callbacks
	*/
	void ClearCallbacks() noexcept;

	/**
	@brief Invoke callback for every bind functions

	@param args : functions parameters
	*/
	void Invoke(Args... args) const noexcept;
};


/**
@brief Util class for events, allow to auto remove the callback on the event (Scoped variable)
*/
template <typename ...Args>
struct EventHandle
{
	EventHandle() = default;
	EventHandle(CallbackID ID, Event<Args...>* eventPtr);
	EventHandle(EventHandle const& handle) = delete;
	EventHandle(EventHandle&& handle) noexcept;

	EventHandle& operator=(EventHandle const& handle) = delete;
	EventHandle& operator=(EventHandle&& handle) noexcept;

	~EventHandle();

	//Keep the id of the callback and the corrsponding event to delete
	CallbackID m_calbackID = 0;
	Event<Args...>* m_eventPtr = nullptr;
};

#include "Event.inl"