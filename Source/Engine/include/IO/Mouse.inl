using namespace Input;


inline bool Mouse::ClickInfo::IsDoubleClicked(float acceptanceRadius) const
{
	if (acceptanceRadius >= 0.0f)
	{
		return m_isDoubleClick && acceptanceRadius * acceptanceRadius >= m_squareDeltaPositionDoubleClick;
	}
	return m_isDoubleClick;
}

inline bool Mouse::ClickInfo::IsClicked(float acceptanceRadius) const
{
	if (acceptanceRadius >= 0.0f)
	{
		return m_isClick && acceptanceRadius * acceptanceRadius >= m_squareDeltaPositionClick;
	}
	return m_isClick;
}

inline bool Mouse::ClickInfo::IsGrabbing() const
{
	return m_grabbed;
}


inline Vector2 Mouse::GetPosition()
{
	return m_instance->m_position;
}

inline Vector2 Mouse::GetScroll()
{
	return m_instance->m_scroll;
}


inline Vector2 Mouse::GetDeltaPosition()
{
	return m_instance->m_deltaPosition;
}

inline Vector2 Mouse::GetDeltaScroll()
{
	return m_instance->m_deltaScroll;
}

inline void Mouse::ResetDeltaPosition()
{
	m_instance->m_deltaPosition = Vector2::Zero;
}

inline bool Mouse::WasClicked()
{
	return m_instance->m_clicked;
}

inline std::vector<NamedButton>& Mouse::GetNamedButtonList()
{
	return m_instance->namedButtonList;
}

inline std::vector<NamedAxis>& Mouse::GetNamedAxesList()
{
	return m_instance->namedAxesList;
}

inline NamedButton& Mouse::AddButton(const std::string& buttonName)
{
	// Get default button from reflected metadata
	rfk::Enum const* buttonEnum = rfk::getDatabase().getFileLevelEnumByName("MouseButton");
	rfk::EnumValue const* buttonValue = buttonEnum->getEnumValueByName("Left");

	// Add named button
	return m_instance->namedButtonList.emplace_back(NamedButton(buttonName, "Left", static_cast<int>(buttonValue->getValue())));
}

inline NamedAxis& Mouse::AddAxis(const std::string& axisName)
{
	// Get default buttons from reflected metadata
	rfk::Enum const* buttonEnum = rfk::getDatabase().getFileLevelEnumByName("MouseButton");
	rfk::EnumValue const* buttonValueNeg = buttonEnum->getEnumValueByName("Left");
	rfk::EnumValue const* buttonValuePos = buttonEnum->getEnumValueByName("Right");

	// Add named axis
	return m_instance->namedAxesList.emplace_back(NamedAxis(axisName, "Left", "Right", Axis(static_cast<int>(buttonValueNeg->getValue()), static_cast<int>(buttonValuePos->getValue()))));
}


inline bool Mouse::GetButtonPressed(MouseButton keyId)
{
	return GetButton(keyId).pressed;
}

inline bool Mouse::GetButtonUnpressed(MouseButton keyId)
{
	return GetButton(keyId).unpressed;
}

inline bool Mouse::GetButtonDown(MouseButton keyId)
{
	return GetButton(keyId).down;
}

inline bool Mouse::GetButtonUp(MouseButton keyId)
{
	return GetButton(keyId).up;
}

inline bool Mouse::GetButtonPressed(const std::string& buttonName)
{
	Button* button = GetButton(buttonName);

	if (button == nullptr)
		return false;

	return button->pressed;
}

inline bool Mouse::GetButtonUnpressed(const std::string& buttonName)
{
	Button* button = GetButton(buttonName);

	if (button == nullptr)
		return false;

	return button->unpressed;
}

inline bool Mouse::GetButtonDown(const std::string& buttonName)
{
	Button* button = GetButton(buttonName);

	if (button == nullptr)
		return false;

	return button->down;
}

inline bool Mouse::GetButtonUp(const std::string& buttonName)
{
	Button* button = GetButton(buttonName);

	if (button == nullptr)
		return false;

	return button->up;
}

inline bool Mouse::GetButtonClicked(MouseButton buttonId)
{
	return m_instance->clickInfos[static_cast<int>(buttonId)].IsClicked(-1.0f);
}

inline bool Mouse::GetButtonPreciseClicked(MouseButton buttonId, float acceptanceRadius)
{
	return m_instance->clickInfos[static_cast<int>(buttonId)].IsClicked(acceptanceRadius);
}


inline bool Mouse::GetButtonDoubleClicked(MouseButton buttonId)
{
	return m_instance->clickInfos[static_cast<int>(buttonId)].IsDoubleClicked(-1.0f);
}

inline bool Mouse::GetButtonPreciseDoubleClicked(MouseButton buttonId, float acceptanceRadius)
{
	return m_instance->clickInfos[static_cast<int>(buttonId)].IsDoubleClicked(acceptanceRadius);
}

inline bool	Mouse::GetButtonGrabbed(MouseButton buttonId)
{
	return m_instance->clickInfos[static_cast<int>(buttonId)].IsGrabbing();
}

inline float Mouse::GetAxisRawValue(const std::string& axisName)
{
	Axis* axis = GetAxis(axisName);

	if (axis == nullptr)
		return 0.f;

	return axis->rawValue;
}

inline float Mouse::GetAxisValue(const std::string& axisName)
{
	Axis* axis = GetAxis(axisName);

	if (axis == nullptr)
		return 0.f;

	return axis->value;
}

template <class TNamedObject>
inline bool Mouse::FindInNamedList(std::vector<TNamedObject>& namedList, const std::string& buttonName, int& index)
{
	for (int i = 0; i < namedList.size(); i++)
	{
		if (namedList[i].name == buttonName)
		{
			index = i;
			return true;
		}
	}

	return false;
}

inline Button& Mouse::GetButton(const MouseButton buttonID)
{
	return m_instance->buttonList[static_cast<int>(buttonID)];
}

inline Button* Mouse::GetButton(const std::string& buttonName)
{
	int index;
	if (m_instance->FindInNamedList(m_instance->namedButtonList, buttonName, index))
		return &m_instance->buttonList[m_instance->namedButtonList[index].id];

	return nullptr;
}

inline Axis* Mouse::GetAxis(const std::string& axisName)
{
	int index;
	if (m_instance->FindInNamedList(m_instance->namedAxesList, axisName, index))
		return &m_instance->namedAxesList[index].axis;

	return nullptr;
}