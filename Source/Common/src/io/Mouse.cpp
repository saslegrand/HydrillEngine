#include <io/Mouse.hpp>
#include <io/Window.hpp>

#include <GLFW/glfw3.h>

Input::Mouse* Input::Mouse::m_instance = nullptr;

Input::Mouse::Mouse(Window* window)
{
	if (m_instance == nullptr)
	{
		m_instance = this;

		window->mouseButtonEvent.AddCallback(&Mouse::OnButtonEvent, this);
		window->mousePosEvent.AddCallback(&Mouse::OnMousePosEvent, this);
		window->mouseScrollEvent.AddCallback(&Mouse::OnMouseScrollEvent, this);

		m_instance->m_window = window;
	}
}

void Input::Mouse::RefreshQueue()
{
	for (int i = 0; i < m_instance->queueSize; i++)
	{
		m_instance->buttonQueue[i]->Refresh();
		m_instance->buttonQueue[i] = nullptr;
	}

	m_instance->queueSize = 0;
}

MouseMode Input::Mouse::GetMode()
{
	return static_cast<MouseMode>(glfwGetInputMode(m_instance->m_window->GetInstance(), GLFW_CURSOR));
}

void Input::Mouse::SetMode(MouseMode mode)
{
	glfwSetInputMode(m_instance->m_window->GetInstance(), GLFW_CURSOR, static_cast<int>(mode));
}



//	Callback functions

void Input::Mouse::OnButtonEvent(const int key, const int action)
{
	if (queueSize >= MAX_MOUSEBUTTONQUEUE)
	{
		//	TODO : Warning Log Message here
		return;
	}

	buttonList[key].Update(action);
	buttonQueue[queueSize] = &buttonList[key];
	queueSize++;
}

void Input::Mouse::OnMousePosEvent(const float x, const float y)
{
	Vector2 oldPosition = m_position;

	m_position.x = x;
	m_position.y = y;

	m_deltaPosition = m_position - oldPosition;
}

void Input::Mouse::OnMouseScrollEvent(const float x, const float y)
{
	Vector2 oldScroll = m_scroll;

	m_scroll.x += x;
	m_scroll.y += y;

	m_deltaScroll = m_scroll - oldScroll;
}
