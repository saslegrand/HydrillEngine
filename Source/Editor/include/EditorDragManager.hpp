#pragma once

#include <memory>
#include <vector>

#include "Tools/Event.hpp"
#include "EditorImages.hpp"

class Resource;
class GameObject;

/**
@brief  Editor drag manager
*/
class EditorDragManager
{

private:

	Resource*   draggedResource = nullptr;
	GameObject* draggedGameObject = nullptr;

//	Functions

public:

	void EndDrag();
	void BeginDrag(Resource*   itemToDrag);
	void BeginDrag(GameObject* itemToDrag);

	Resource* GetDraggedResource();
	GameObject* GetDraggedGameObject();

	bool IsDragging();

};
