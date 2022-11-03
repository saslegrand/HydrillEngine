#include "EditorSelectionManager.hpp"

#include <ECS/GameObject.hpp>
#include <Resources/SceneManager.hpp>
#include <EngineContext.hpp>

bool EditorSelectionManager::AddToSelection(HYGUID& selectedGameObject)
{
    if (!IsSelected(selectedGameObject))
    {
        m_gameObjects.push_back(selectedGameObject);

        onGameObjectUpdated.Invoke(&selectedGameObject);

        return true;
    }
    return false;
}

bool EditorSelectionManager::AddToSelection(const GameObject& selectedGameObject)
{
    HYGUID guid = selectedGameObject.GetUID();
    
    return AddToSelection(guid);
}


bool EditorSelectionManager::AddToSelection(Resource* selectedResource)
{
    if (!IsSelected(selectedResource))
    {
        m_resources.push_back(selectedResource);

        onResourceUpdated.Invoke(selectedResource);

        return true;
    }
    return false;
}


bool EditorSelectionManager::RemoveFromSelection(const HYGUID& selectedGameObject)
{
    auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), selectedGameObject);

    if (it != m_gameObjects.end())
    {
        m_gameObjects.erase(it);
        onGameObjectUpdated.Invoke(nullptr);
        return true;
    }
    return false;
}

bool EditorSelectionManager::RemoveFromSelection(const GameObject& selectedGameObject)
{    
    return RemoveFromSelection(selectedGameObject.GetUID());
}


bool EditorSelectionManager::RemoveFromSelection(Resource* selectedResource)
{
    auto it = std::find(m_resources.begin(), m_resources.end(), selectedResource);

    if (it != m_resources.end())
    {
        m_resources.erase(it);
        onResourceUpdated.Invoke(nullptr);
        return true;
    }
    return false;
}

void EditorSelectionManager::DeleteGameObjects()
{
    std::vector<HYGUID>& selectedObjects = GetGameObjects();
    std::vector<GameObject*> goToDelete;

    for (size_t i = 0; i < selectedObjects.size(); i++)
    {
        GameObject* selected = GetGameObject(i);
        if (!selected) continue;

        bool shouldDelete = true;

        for (auto it = goToDelete.begin(); it != goToDelete.end();)
        {
            GameObject* toDelete = it[0];

            // Check if the to delete object is descending from the selected object
            // If yes, dont delete it
            if (toDelete->IsDescendingFrom(selected))
                goToDelete.erase(it);
            else
                it++;

            // Check if the selected object is descending from the object to delete
            if (selected->IsDescendingFrom(toDelete))
                shouldDelete = false;
        }

        // Add to delete list
        if (shouldDelete)
            goToDelete.push_back(selected);
    }

    // Clear selection list
    ClearGameObjects();

    for (GameObject* go : goToDelete)
        SceneManager::DestroyGameObject(*go);
}

GameObject* EditorSelectionManager::GetGameObject(const size_t index)
{
    SceneManager* sceneManager = EngineContext::Instance().sceneManager;
    if (sceneManager && sceneManager->GetCurrentScene() && m_gameObjects.size() > index)
    {
        return EngineContext::Instance().sceneManager->GetCurrentScene()->GetGameObject(m_gameObjects[index]);
    }

    return nullptr;
}

GameObject* EditorSelectionManager::GetGameObjectFromScene(const HYGUID& guid)
{
    SceneManager* sceneManager = EngineContext::Instance().sceneManager;
    if (sceneManager && sceneManager->GetCurrentScene())
    {
        return EngineContext::Instance().sceneManager->GetCurrentScene()->GetGameObject(guid);
    }

    return nullptr;
}

std::vector<HYGUID>& EditorSelectionManager::GetGameObjects()
{
    return m_gameObjects;
}


std::vector<Resource*>& EditorSelectionManager::GetResources()
{
    return m_resources;
}


bool EditorSelectionManager::IsSelected(const HYGUID& selectedGameObject)
{
    return std::find(m_gameObjects.begin(), m_gameObjects.end(), selectedGameObject) != m_gameObjects.end();
}


bool EditorSelectionManager::IsSelected(const GameObject& gameObject)
{
    return IsSelected(gameObject.GetUID());
}

bool EditorSelectionManager::IsSelected(Resource* selectedResource)
{
    return std::find(m_resources.begin(), m_resources.end(), selectedResource) != m_resources.end();
}


void EditorSelectionManager::ClearAll()
{
    ClearResources();
    ClearGameObjects();
}


void EditorSelectionManager::ClearGameObjects()
{
    if (!m_gameObjects.empty())
    {
        m_gameObjects.clear();
        onGameObjectUpdated.Invoke(nullptr);
    }
}


void EditorSelectionManager::ClearResources()
{
    if (!m_resources.empty())
    {
        m_resources.clear();
        onResourceUpdated.Invoke(nullptr);
    }
}
