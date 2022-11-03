#pragma once

#include <Maths/Vector3.hpp>
#include <vector>

#include <Tools/Event.hpp>
#include <Types/GUID.hpp>

#include "Widgets/EditorWidget.hpp"

class GameObject;
class BaseObject;

struct SuggestionData
{
	bool regen = true;
	bool isPopupOpen = false;
	int  activeIdx = 0;

	std::string researchedString;
	std::vector<std::string> suggestionList;
};


/**
@brief This widget enable the user to tweak gameobjetcs from the scene
*/
class GameObjectInspectorWidget : public EditorWidget
{

//	Variables

private:

	HYGUID currentGameObject;

	SuggestionData suggestion;

	bool m_localTransform = false;

	bool m_openSuggestionPopup = false;
	int  m_selectedIndice = 0;

	bool m_lockCurrentGameObject = false;

	unsigned int m_openedCadenasButtonTexture = 0;
	unsigned int m_closedCadenasButtonTexture = 0;

//	Constructor(s) & Destructor(s)

public:

	GameObjectInspectorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);
	GameObjectInspectorWidget() = default;

	~GameObjectInspectorWidget() {}

//	Functions

private:

	/**
	@brief "Add component" browser

	@param currentObj : binded game object
	*/
	void AddComponentBrowser(GameObject& currentObj);

	/**
	@brief suggestion popup of the add component browser, will display component depending on the serached string

	@param currentObj : binded game object
	*/
	void SuggestionPopup(GameObject& currentObj);

	/**
	@brief Display component binded to the given gameObject

	@param currentObj : binded game object
	*/
	void DisplayComponents(GameObject& currentObj);

	/**
	@brief Generate suggestion depending on the searched string in add component browser
	*/
	void GenerateSuggestions();

	/**
	@brief This function is called when selection manager has some changes with selected games objects
	
	@param gameObject : newly selected game object, or is null if all selected objects were unselected
	*/
	void GameObjectSelectionUpdate(HYGUID* gameObject);

public:

	/**
	@brief Widget display function
	*/
	void Display() override;

};
