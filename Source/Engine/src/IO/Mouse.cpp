#include "IO/Mouse.hpp"

#include <queue>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <IO/Window.hpp>
#include <nlohmann/json.hpp>

#include "Core/Logger.hpp"


using namespace Input;

Mouse* Mouse::m_instance = nullptr;

void Mouse::ClickInfo::Update(float deltaTime, const Button& button)
{
	Vector2 mousePos = Input::Mouse::GetPosition();
	bool resetDoubleClickCheck = false;

	m_isClick = false;
	m_isDoubleClick = false;

	//	Was the mouse pressed once
	UpdateClick(deltaTime, button);
	UpdateDoubleClick(deltaTime, button);

	if (m_grabbed && button.unpressed) m_grabbed = false;
}


void Mouse::ClickInfo::UpdateClick(float deltaTime, const Button& button)
{
	if (m_wasPressed)
	{
		//	Check
		m_elapsedTimePressed += deltaTime;

		if (m_elapsedTimePressed <= m_ClickDelta)
		{
			m_isClick = button.unpressed;
		}
		else
		{
			m_grabbed = true;
			m_wasPressed = false;
			m_elapsedTimePressed = 0.0f;
		}
	}
	else
	{
		m_singleClickSavedPosition = Input::Mouse::GetPosition();
		m_wasPressed = button.pressed;
	}

	if (m_isClick)
	{
		m_squareDeltaPositionClick = (m_singleClickSavedPosition - Input::Mouse::GetPosition()).SquaredMagnitude();
		m_singleClickSavedPosition = Vector2::Zero;

		m_wasPressed = false;
		m_elapsedTimePressed = 0.0f;
	}
}


void Mouse::ClickInfo::UpdateDoubleClick(float deltaTime, const Button& button)
{
	if (m_wasClicked)
	{
		//	Check
		m_elapsedTimeClicked += deltaTime;

		if (m_elapsedTimeClicked <= m_doubleClickDelta)
		{
			m_isDoubleClick = button.unpressed;
		}
		else
		{
			m_wasClicked = false;
			m_elapsedTimeClicked = 0.0f;
		}
	}
	else
	{
		m_wasClicked = m_isClick;
		m_doubleClickSavedPosition = Input::Mouse::GetPosition();
	}

	if (m_isDoubleClick)
	{
		m_squareDeltaPositionDoubleClick = (m_doubleClickSavedPosition - Input::Mouse::GetPosition()).SquaredMagnitude();
		m_doubleClickSavedPosition = Vector2::Zero;

		m_wasClicked = false;
		m_elapsedTimeClicked = 0.0f;
	}
}


Mouse::Mouse(Window* window)
{
	if (m_instance == nullptr)
	{
		m_instance = this;

		//	Subscribe events
		window->mouseButtonEvent.AddCallback(&Mouse::OnButtonEvent, *this);
		window->mousePosEvent.AddCallback(&Mouse::OnMousePosEvent, *this);
		window->mouseScrollEvent.AddCallback(&Mouse::OnMouseScrollEvent, *this);

		m_instance->m_window = window;
	}
}

void Mouse::Refresh()
{
	//	Clear the queue
	for (int i = 0; i < m_instance->queueSize; i++)
	{
		m_instance->buttonQueue[i]->Refresh();
		m_instance->buttonQueue[i] = nullptr;
	}

	m_instance->queueSize = 0;

	//	If no movement or scroll callbacks were called, set deltas to zero
	if (!m_moved)    m_deltaPosition = Vector2::Zero;
	if (!m_scrolled) m_deltaScroll   = Vector2::Zero;

	//	If disabled alwa
	if (GetMode() == MouseMode::Disabled) ResetPosition();
	else m_instance->m_savedPosition = m_instance->m_position;

	//	Refresh booleans
	m_scrolled = false;
	m_moved    = false;
	m_clicked  = false;
}


void Mouse::Update(float deltaTime)
{
	for (auto& [key, button] : clickInfos)
	{
		clickInfos[key].Update(deltaTime, buttonList[key]);
	}
}

void Mouse::SetPosition(const Vector2& newPosition)
{
	m_instance->m_position = newPosition;
	glfwSetCursorPos(m_instance->m_window->GetInstance(), newPosition.x, newPosition.y);
}

void Mouse::SetPositionX(float x)
{
	m_instance->m_position.x = x;
	glfwSetCursorPos(m_instance->m_window->GetInstance(), m_instance->m_position.x, m_instance->m_position.y);
}

void Mouse::SetPositionY(float y)
{
	m_instance->m_position.y = y;
	glfwSetCursorPos(m_instance->m_window->GetInstance(), m_instance->m_position.x, m_instance->m_position.y);
}

MouseMode Mouse::GetMode()
{
	return static_cast<MouseMode>(glfwGetInputMode(m_instance->m_window->GetInstance(), GLFW_CURSOR));
}

void Mouse::SetMode(MouseMode mode)
{
	glfwSetInputMode(m_instance->m_window->GetInstance(), GLFW_CURSOR, static_cast<int>(mode));
}

void Mouse::ResetPosition()
{
	SetPosition(m_instance->m_savedPosition);
}

void Mouse::ResetScroll()
{
	m_instance->m_scroll = { 0.0f,0.0f };
}

//	Callback functions

void Mouse::OnButtonEvent(const int key, const int action)
{
	if (queueSize >= MAX_MOUSEBUTTONQUEUE)
	{
		Logger::Warning("Mouse - Maximum mouse button number that can be updated at the same time has been reached");
		return;
	}
	if (action == static_cast<int>(Button::InputAction::Pressed))
	{
		m_clicked = true;
	}

	buttonList[key].Update(action);

	buttonQueue[queueSize] = &buttonList[key];
	queueSize++;
}


void Mouse::OnMousePosEvent(const float x, const float y)
{
	m_deltaPosition.x = (x - m_position.x) / m_window->GetWidth();
	m_deltaPosition.y = (y - m_position.y) / m_window->GetHeight();

	m_moved = true;

	m_position.x = x;
	m_position.y = y;
}


void Mouse::OnMouseScrollEvent(const float x, const float y)
{
	m_scrolled = true;

	Vector2 oldScroll = m_scroll;

	m_scroll.x += x;
	m_scroll.y += y;

	m_deltaScroll = m_scroll - oldScroll;
}


void Mouse::SaveButtons(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonButtons = jsonFile["Buttons"];

	for (auto& namedButton : m_instance->namedButtonList)
	{
		// Named button not valid, dont keep
		if (namedButton.id < 0)
			continue;

		nlohmann::json& jsonNamedButton = jsonButtons[namedButton.name];

		jsonNamedButton["Button"] = namedButton.id;
		jsonNamedButton["ButtonName"] = namedButton.idName;
	}

	nlohmann::json& jsonAxes = jsonFile["Axes"];

	for (auto& namedAxis : m_instance->namedAxesList)
	{
		// Named button not valid, dont keep
		if (namedAxis.axis.buttonIdNeg < 0 || namedAxis.axis.buttonIdPos < 0)
			continue;

		nlohmann::json& jsonNamedButton = jsonAxes[namedAxis.name];

		jsonNamedButton["AxisNameNeg"] = namedAxis.idNameNeg;
		jsonNamedButton["AxisNamePos"] = namedAxis.idNamePos;
		jsonNamedButton["AxisNeg"] = namedAxis.axis.buttonIdNeg;
		jsonNamedButton["AxisPos"] = namedAxis.axis.buttonIdPos;
		jsonNamedButton["AxisAmplitude"] = namedAxis.axis.amplitude;
		jsonNamedButton["AxisTimeMult"] = namedAxis.axis.timeMultiplier;
	}
}


void Mouse::LoadButtons(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonButtons = jsonFile["Buttons"];
	for (nlohmann::json::iterator buttonIt = jsonButtons.begin(); buttonIt != jsonButtons.end(); ++buttonIt)
	{
		std::string buttonName = buttonIt.key();
		nlohmann::json& jsonButtonValues = buttonIt.value();

		NamedButton& namedButton = Mouse::AddButton(buttonName);

		namedButton.id = jsonButtonValues["MouseButton"].get<int>();
		namedButton.idName = jsonButtonValues["MouseButtonName"].get<std::string>();
	}

	nlohmann::json& jsonAxes = jsonFile["Axes"];
	for (nlohmann::json::iterator axisIt = jsonAxes.begin(); axisIt != jsonAxes.end(); ++axisIt)
	{
		std::string axisName = axisIt.key();
		nlohmann::json& jsonAxisValues = axisIt.value();

		NamedAxis& namedAxis = Mouse::AddAxis(axisName);

		namedAxis.idNameNeg = jsonAxisValues["AxisNameNeg"].get<std::string>();
		namedAxis.idNamePos = jsonAxisValues["AxisNamePos"].get<std::string>();
		namedAxis.axis.buttonIdNeg = jsonAxisValues["AxisNeg"].get<int>();
		namedAxis.axis.buttonIdPos = jsonAxisValues["AxisPos"].get<int>();
		namedAxis.axis.amplitude = jsonAxisValues["AxisAmplitude"].get<float>();
		namedAxis.axis.timeMultiplier = jsonAxisValues["AxisTimeMult"].get<float>();
	}
}
