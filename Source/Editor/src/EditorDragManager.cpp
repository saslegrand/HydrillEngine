#include "EditorDragManager.hpp"


void EditorDragManager::BeginDrag(Resource* itemToDrag)
{
	draggedResource = itemToDrag;
}


void EditorDragManager::BeginDrag(GameObject* itemToDrag)
{
	draggedGameObject = itemToDrag;
}


void EditorDragManager::EndDrag()
{
	draggedGameObject = nullptr;
	draggedResource = nullptr;
}


bool EditorDragManager::IsDragging()
{
	return draggedGameObject != nullptr || draggedResource != nullptr;
}


Resource* EditorDragManager::GetDraggedResource()
{
	return draggedResource;
}

GameObject* EditorDragManager::GetDraggedGameObject()
{
	return draggedGameObject;
}