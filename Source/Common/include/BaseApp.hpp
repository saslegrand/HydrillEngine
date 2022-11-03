#pragma once

#include <memory>

#include "Engine.hpp"
#include "IO/Window.hpp"

class BaseApp
{
//	Constructors & Destructors

public:

	virtual ~BaseApp() = default;

//	Variables

protected:

	std::unique_ptr<class Window>		m_window = nullptr;

	std::unique_ptr<class DllWrapper>	m_scriptingWrapper = nullptr;

	std::unique_ptr<class Engine>		m_engine = nullptr;

	CallbackID resizeEventID;
	CallbackID moveEventID;

//	Functions

public:

	virtual void Initialize(const char* in_windowName = "Hydrill Engine") = 0;

	virtual void CloseApp() = 0;

	virtual void Loop() = 0;

	virtual void Shutdown() = 0;

protected:

	virtual void FrameStart() = 0;

	virtual void FrameEnd() = 0;

	virtual void Update() = 0;

	virtual void Render() = 0;

	virtual void ResizeUpdate() = 0;
};