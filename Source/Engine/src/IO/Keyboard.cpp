#include "IO/Keyboard.hpp"

#include <IO/Window.hpp>
#include <nlohmann/json.hpp>

#include <Tools/SerializationUtils.hpp>

#include "EngineContext.hpp"
#include "Core/Time.hpp"
#include "Core/Logger.hpp"

Input::Keyboard* Input::Keyboard::m_instance = nullptr;

Input::Keyboard::Keyboard(Window* window)
{
	if (m_instance == nullptr)
	{
		m_instance = this;

		//	Subscribe events
		window->keyEvent.AddCallback(&Keyboard::OnKeyEvent, *this);
	}
}


void Input::Keyboard::OnKeyEvent(const int key, const int action)
{
	if (queueSize >= MAX_KEYQUEUE)
	{
		Logger::Warning("Keyboard - Maximum key number that can be updated at the same time has been reached");
		return;
	}

	keyList[key].Update(action);
	keyQueue[queueSize] = &keyList[key];

	queueSize++;
}


void Input::Keyboard::Refresh()
{
	//	Clear the queue
	for (unsigned int i = 0; i < queueSize; i++)
	{
		keyQueue[i]->Refresh();
		keyQueue[i] = nullptr;
	}

	queueSize = 0;
	
	// Compute all registered axes
	float deltaTime = EngineContext::Instance().time->GetDeltaTime();
	for (auto& namedAxis : namedAxesList)
		namedAxis.axis.Compute(deltaTime);
}

void Keyboard::SaveKeys(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonKeys = jsonFile["Keys"];

	nlohmann::json& jsonButtons = jsonKeys["Buttons"];

	for (auto& namedButton : m_instance->namedButtonList)
	{
		// Named button not valid, dont keep
		if (namedButton.id < 0)
			continue;

		nlohmann::json& jsonNamedButton = jsonButtons[namedButton.name];

		jsonNamedButton["Key"] = namedButton.id;
		jsonNamedButton["KeyName"] = namedButton.idName;
	}

	nlohmann::json& jsonAxes = jsonKeys["Axes"];

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

void Keyboard::LoadKeys(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonKeys = jsonFile["Keys"];

	m_instance->namedAxesList.clear();
	m_instance->namedButtonList.clear();

	nlohmann::json& jsonButtons = jsonKeys["Buttons"];
	for (nlohmann::json::iterator buttonIt = jsonButtons.begin(); buttonIt != jsonButtons.end(); ++buttonIt)
	{
		std::string buttonName = buttonIt.key();
		nlohmann::json& jsonButtonValues = buttonIt.value();

		Input::NamedButton& namedButton = Keyboard::AddKey(buttonName);

		Serialization::TryGetValue(jsonButtonValues, "Key", namedButton.id);
		Serialization::TryGetValue(jsonButtonValues, "KeyName", namedButton.idName);
	}

	nlohmann::json& jsonAxes = jsonKeys["Axes"];
	for (nlohmann::json::iterator axisIt = jsonAxes.begin(); axisIt != jsonAxes.end(); ++axisIt)
	{
		std::string axisName = axisIt.key();
		nlohmann::json& jsonAxisValues = axisIt.value();

		Input::NamedAxis& namedAxis = Keyboard::AddAxis(axisName);

		Serialization::TryGetValue(jsonAxisValues, "AxisNameNeg", namedAxis.idNameNeg);
		Serialization::TryGetValue(jsonAxisValues, "AxisNamePos", namedAxis.idNamePos);
		Serialization::TryGetValue(jsonAxisValues, "AxisNeg", namedAxis.axis.buttonIdNeg);
		Serialization::TryGetValue(jsonAxisValues, "AxisPos", namedAxis.axis.buttonIdPos);
		Serialization::TryGetValue(jsonAxisValues, "AxisAmplitude", namedAxis.axis.amplitude);
		Serialization::TryGetValue(jsonAxisValues, "AxisTimeMult", namedAxis.axis.timeMultiplier);
	}
}