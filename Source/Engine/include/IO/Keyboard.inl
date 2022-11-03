
using namespace Input;

inline std::vector<NamedButton>& Keyboard::GetNamedButtonList()
{
	return m_instance->namedButtonList;
}

inline std::vector<NamedAxis>& Keyboard::GetNamedAxesList()
{
	return m_instance->namedAxesList;
}

inline NamedButton& Keyboard::AddKey(const std::string& keyName)
{
	// Get default key from reflected metadata
	rfk::Enum const* keyEnum = rfk::getDatabase().getFileLevelEnumByName("Key");
	rfk::EnumValue const* keyValue = keyEnum->getEnumValueByName("A");

	// Add named key
	return m_instance->namedButtonList.emplace_back(NamedButton(keyName, "A", static_cast<int>(keyValue->getValue())));
}

inline NamedAxis& Keyboard::AddAxis(const std::string& axisName)
{
	// Get default keys from reflected metadata
	rfk::Enum const* keyEnum = rfk::getDatabase().getFileLevelEnumByName("Key");
	rfk::EnumValue const* keyValueNeg = keyEnum->getEnumValueByName("A");
	rfk::EnumValue const* keyValuePos = keyEnum->getEnumValueByName("B");

	// Add named axis
	return m_instance->namedAxesList.emplace_back(NamedAxis(axisName, "A", "B", Axis(static_cast<int>(keyValueNeg->getValue()), static_cast<int>(keyValuePos->getValue()))));
}

inline bool Keyboard::GetKeyPressed(Key keyId)
{
	return GetKey(keyId).pressed;
}

inline bool Keyboard::GetKeyUnpressed(Key keyId)
{
	return GetKey(keyId).unpressed;
}

inline bool Keyboard::GetKeyDown(Key keyId)
{
	return GetKey(keyId).down;
}

inline bool Keyboard::GetKeyUp(Key keyId)
{
	return GetKey(keyId).up;
}

inline bool Keyboard::GetKeyPressed(const std::string& keyName)
{
	Button* key = GetKey(keyName);

	if (key == nullptr)
		return false;

	return key->pressed;
}

inline bool Keyboard::GetKeyUnpressed(const std::string& keyName)
{
	Button* key = GetKey(keyName);

	if (key == nullptr)
		return false;

	return key->unpressed;
}

inline bool Keyboard::GetKeyDown(const std::string& keyName)
{
	Button* key = GetKey(keyName);

	if (key == nullptr)
		return false;

	return key->down;
}

inline bool Keyboard::GetKeyUp(const std::string& keyName)
{
	Button* key = GetKey(keyName);

	if (key == nullptr)
		return false;

	return key->up;
}

inline float Keyboard::GetAxisRawValue(const std::string& axisName)
{
	Axis* axis = GetAxis(axisName);

	if (axis == nullptr)
		return 0.f;

	return axis->rawValue;
}

inline float Keyboard::GetAxisValue(const std::string& axisName)
{
	Axis* axis = GetAxis(axisName);

	if (axis == nullptr)
		return 0.f;

	return axis->value;
}

template <class TNamedObject>
inline bool Keyboard::FindInNamedList(std::vector<TNamedObject>& namedList, const std::string& keyName, int& index)
{
	for (int i = 0; i < namedList.size(); i++)
	{
		if (namedList[i].name == keyName)
		{
			index = i;
			return true;
		}
	}

	return false;
}

inline Button& Keyboard::GetKey(Key keyCode)
{
	return m_instance->keyList[static_cast<int>(keyCode)];
}

inline Button* Keyboard::GetKey(const std::string& keyName)
{
	int index;
	if (m_instance->FindInNamedList(m_instance->namedButtonList, keyName, index))
		return &m_instance->keyList[m_instance->namedButtonList[index].id];

	return nullptr;
}

inline Axis* Keyboard::GetAxis(const std::string& axisName)
{
	int index;
	if (m_instance->FindInNamedList(m_instance->namedAxesList, axisName, index))
		return &m_instance->namedAxesList[index].axis;

	return nullptr;
}