#include "Widgets/SceneGraphWidget.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <algorithm>

#include <Core/TaskQueue.hpp>
#include <ECS/GameObject.hpp>
#include <ECS/MeshComponent.hpp>
#include <ECS/SkeletalMeshComponent.hpp>
#include <EngineContext.hpp>
#include <Resources/SceneManager.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Resources/Resource/Model.hpp>
#include <IO/Keyboard.hpp>
#include <IO/Mouse.hpp>
#include <Maths/Maths.hpp>

#include "EditorContext.hpp"
#include "PathConfig.hpp"
#include "EditorDragManager.hpp"
#include "EditorSelectionManager.hpp"
#include "GUITypes.hpp"
#include "GUI.hpp"


SceneGraphWidget::SceneGraphWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: EditorWidget(widgetName, widgetID, isActive, "###SCENEGRAPH")
{
}

int getAllChildCount(GameObject* gameObject)
{
    int count = 0;

    for(GameObject* g : gameObject->GetChildren())
    { 
        count++;
        count += getAllChildCount(g);
    }

    return count;
}


void SceneGraphWidget::ResetSelections()
{
    m_startRangeIndex = m_endRangeIndex   = -1;
    EditorContext::Instance().selectionManager->ClearGameObjects();
}


void SceneGraphWidget::DisplayGameObject(GameObject* gameObject)
{
    EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
    std::vector<HYGUID>& selectedObjects = selecManager->GetGameObjects();

    //  Set default flags
    static ImGuiTreeNodeFlags defaultTreeNodeFlag =
        ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth |
        ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoAutoOpenOnLog;

    //  Children Loop
    for (GameObject* currObjectPtr : gameObject->GetChildren())
    {
        if (currObjectPtr == nullptr)
        {
            continue;
        }

        GameObject& currentGameObject = *currObjectPtr;
        ImGui::PushID(m_count);

        ImGuiTreeNodeFlags flag = defaultTreeNodeFlag;

        const char* name = currentGameObject.GetName().c_str();

        bool isSelected = EditorContext::Instance().selectionManager->IsSelected(currentGameObject.GetUID());

        //  Is selected ?
        if (isSelected) flag |= ImGuiTreeNodeFlags_Selected;

        //  If not children, do not draw the arrow
        if (currentGameObject.GetChildren().empty()) flag |= ImGuiTreeNodeFlags_Bullet;

        //  Do we need to force open this node ?
        auto it = std::find(m_toForceOpen.begin(), m_toForceOpen.end(), &currentGameObject);

        if (it != m_toForceOpen.end())
        {
            ImGui::SetNextItemOpen(true);
            m_toForceOpen.erase(it);
        }

        //  Save current cursor position for later use 
        ImVec2 savedCursorPos = ImGui::GetCursorScreenPos();

        bool showChildren = ImGui::TreeNodeEx(name, flag);

        //  Renaming
        //  --------

        if (m_tryRename)
        {
            //  Is currently selected and is first selected object
            if (!selectedObjects.empty())
            {
                if (selectedObjects[0] == currentGameObject.GetUID())
                {
                    //  Set renaming object pointer, renameBuffer and set focus to the next widget
                    m_renamingObject = &currentGameObject;
                    m_renameBuffer = currentGameObject.GetName();
                    ImGui::SetKeyboardFocusHere(0);
                    m_tryRename = false;
                }
            }
            else
            {
                m_tryRename = false;
            }
        }

        if (m_renamingObject == &currentGameObject)
        {
            ImGui::SetItemAllowOverlap();

            // Get cursor position and add indentation from treenode
            ImVec2 pos = savedCursorPos;

            pos.x = savedCursorPos.x + ImGui::GetStyle().FramePadding.x + ImGui::GetCurrentContext()->FontSize;
            ImGui::SetCursorScreenPos(pos);
            float width = ImGui::GetContentRegionAvail().x;

            EditorTheme* theme = EditorContext::Instance().theme;

            //  Set special style for this text field
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushItemWidth(width);

            //  Make char* buffer for ImGUi::InputText with a limited size of 64
            char buffer[64] = {};
            sprintf_s(buffer, "%s", m_renameBuffer.c_str());

            bool validate = false;
            if (ImGui::InputText("###GameObjectRenameField", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                validate = true;
            }
            GUI::PopStyle();

            m_renameBuffer = buffer;

            //  Validate name if focus is lost
            if (!ImGui::IsWindowFocused() || !isSelected || (!ImGui::IsItemHovered() && Input::Mouse::GetButtonDown(MouseButton::Left)))
            {
                validate = true;
            }

            if (validate)
            {
                //  If node currently open we want it to still be open after changing name
                if (showChildren)
                {
                    m_toForceOpen.push_back(m_renamingObject);
                }

                //  Apply new name if enter pressed
                m_renamingObject->SetName(buffer);
                m_renamingObject = nullptr;
                m_renameBuffer = "";
            }
        }


        //  Drag and Drop Actions
        //  ---------------------


        if (ImGui::BeginDragDropSource())
        {
            m_draggingSelected = isSelected;
            m_draggingObject = true;

            ImGui::Text(name);
            ImGui::SetDragDropPayload("DND_GameObject", name, sizeof(int));
            ImGui::EndDragDropSource();
        }
        else if (ImGui::BeginDragDropTarget())
        {

            if (ExternalDrop(&currentGameObject));

            // Set payload to carry the index of our item (could be anything)
            else if (ImGui::AcceptDragDropPayload("DND_GameObject"))
            {
                if (m_draggingSelected)
                {
                    for (int i = 0; i < selectedObjects.size(); i++)
                    {
                        GameObject* obj = selecManager->GetGameObject(i);

                        if (!obj) continue;

                        GameObject* parent = obj->GetParent();

                        if (parent)
                        {
                            if (!EditorContext::Instance().selectionManager->IsSelected(*parent))
                            {
                                EditorContext::Instance().taskQueue->AddTask([obj, &currentGameObject]() { obj->SetParent(&currentGameObject); });
                            }
                            continue;
                        }

                        obj->SetParent(&currentGameObject);
                    }
                    m_draggingObject = false;
                }
                else
                {
                    if (m_pressedItem)
                    {
                        GameObject* obj = m_pressedItem;
                        EditorContext::Instance().taskQueue->AddTask([obj, &currentGameObject]() { obj->SetParent(&currentGameObject); });
                    }
                }

                EditorContext::Instance().dragManager->EndDrag();
                m_pressedItem = nullptr;
            }
            ImGui::EndDragDropTarget();
        }

        //  Selection Actions
        //  -----------------

        else if (ImGui::IsItemHovered())
        {
            if (Input::Mouse::GetButtonPressed(MouseButton::Left))
            {
                EditorContext::Instance().dragManager->BeginDrag(&currentGameObject);
                m_pressedItem = &currentGameObject;
            }

            if (Input::Mouse::GetButtonPreciseClicked(MouseButton::Left, 10.f))
            {
                if (Input::Keyboard::GetKeyDown(Key::LeftShift) && m_startRangeIndex != -1)
                {
                    if (m_count < m_startRangeIndex)
                    {
                        m_endRangeIndex = Maths::Max(m_endRangeIndex, m_startRangeIndex);
                        m_startRangeIndex = m_count;
                    }
                    else if (m_count > m_startRangeIndex)
                    {
                        m_endRangeIndex = m_count;
                    }
                    else
                    {
                        m_endRangeIndex = -1;
                    }
                }

                //  If only clicked
                else if (!Input::Keyboard::GetKeyDown(Key::LeftControl))
                {
                    m_startRangeIndex = m_count;
                    EditorContext::Instance().selectionManager->ClearGameObjects();
                }

                //  Add if not yet selected, else, ONLY if control is down, it will remove the clicked object
                if (!EditorContext::Instance().selectionManager->AddToSelection(currentGameObject) && Input::Keyboard::GetKeyDown(Key::LeftControl))
                {
                    if (m_startRangeIndex == m_count) m_startRangeIndex++;

                    EditorContext::Instance().selectionManager->RemoveFromSelection(currentGameObject);
                }
                m_pressedItem = nullptr;
                m_keepFocus = true;
            }
        }

        //  Context Menu
        //  ------------

        GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
        bool gameObjectContextMenu = ImGui::BeginPopupContextItem("###SCENEGRAPH_GAMEOBJECT_CONTEXTMENU");
        GUI::PopStyle();

        if (gameObjectContextMenu)
        {
            m_contextMenuOpened = true;

            GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
            bool createMenuOpened = ImGui::BeginMenu("Create");
            GUI::PopStyle();

            if (createMenuOpened)
            {
                if (ImGui::MenuItem("New GameObject"))
                {
                    EditorContext::Instance().taskQueue->AddTask([&currentGameObject, this]()
                        {
                            GameObject* newObject = EngineContext::Instance().sceneManager->Instantiate("New Object");
                            newObject->SetParent(&currentGameObject);

                            EditorContext::Instance().selectionManager->ClearGameObjects();
                            EditorContext::Instance().selectionManager->AddToSelection(*newObject);

                            m_toForceOpen.push_back(&currentGameObject);

                            m_renameBuffer = newObject->GetName();
                            m_renamingObject = newObject;
                            m_tryRename = true;
                        });
                    m_keepFocus = true;
                }

                if (ImGui::MenuItem("Cube"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Cube", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\CubeInternal.mesh)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }

                if (ImGui::MenuItem("Capsule"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Capsule", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\CapsuleInternal.mesh)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Sphere"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Sphere", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\SphereInternal.mesh)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Light"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("Light", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\Light.prefab)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Camera"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("Camera", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\Camera.prefab)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }

                if (ImGui::MenuItem("ParticleSystem"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("ParticleSystem", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\ParticleSystem.prefab)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("SoundEmitter"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("SoundEmitter", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\SoundEmitter.prefab)"), &currentGameObject);
                        }
                    );

                    m_keepFocus = true;
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Rename"))
            {
                EditorContext::Instance().selectionManager->ClearGameObjects();
                EditorContext::Instance().selectionManager->AddToSelection(currentGameObject);

                m_renameBuffer = currentGameObject.GetName();
                m_renamingObject = &currentGameObject;
                m_tryRename = true;
                m_keepFocus = true;
            }

            if (ImGui::MenuItem("Duplicate"))
            {
                Duplicate();
            }

            if (ImGui::MenuItem("copy"))
            {
                Copy(isSelected ? nullptr : &currentGameObject);
            }

            ImGui::BeginDisabled(m_copied.empty());
            if (ImGui::MenuItem("paste"))
            {
                Paste(&currentGameObject);
            }
            ImGui::EndDisabled();

            if (ImGui::MenuItem("Delete"))
            {
                selecManager->DeleteGameObjects();
            }
            ImGui::EndPopup();
        }

        m_count++;

        //  If has children and if the arrow was clicked on
        if (showChildren)
        {
            ImGui::TreePush(name);
            DisplayGameObject(&currentGameObject);
            ImGui::TreePop();
        }
        //  We need to increment count anyway
        else
        {
            m_count += getAllChildCount(&currentGameObject);
        }

        ImGui::PopID();
    }
}

void SceneGraphWidget::UpdateMultiSelectedObject(class GameObject* gameObject)
{
    for (GameObject* child : gameObject->GetChildren())
    {
        //  Is in multiselection interval ? (when shift-click is pressed)
        if (m_count >= m_startRangeIndex && m_count <= m_endRangeIndex)
        {
            EditorContext::Instance().selectionManager->AddToSelection(*child);
        }

        m_count++;

        UpdateMultiSelectedObject(child);
    }
}


void SceneGraphWidget::Update()
{
    m_count = 0;

    m_tryRename |= Input::Keyboard::GetKeyPressed(Key::F2);
    Scene* scn = EngineContext::Instance().sceneManager->GetCurrentScene();

    if (scn && m_endRangeIndex != -1)
    {
        UpdateMultiSelectedObject(&scn->GetRoot());
        
        m_keepFocus = true;
    }
    m_endRangeIndex = -1;



    const auto& selected = EditorContext::Instance().selectionManager->GetGameObjects();

    if(EditorContext::Instance().allowGameObjectCopy && 
        (Input::Keyboard::GetKeyDown(Key::RightControl) || Input::Keyboard::GetKeyDown(Key::LeftControl)))
    {
        if (!selected.empty())
        {
            if (Input::Keyboard::GetKeyPressed(Key::C))
            {
                Copy();
            }

            if (Input::Keyboard::GetKeyPressed(Key::D))
            {
                Duplicate();
            }
        }

        if (!m_copied.empty() && Input::Keyboard::GetKeyPressed(Key::V))
        {
            selected.empty() ? Paste(nullptr) : Paste(EditorContext::Instance().selectionManager->GetGameObject(0));
        }
    }
    EditorContext::Instance().allowGameObjectCopy = false;
}

void SceneGraphWidget::Display()
{
    if (!isActive) return;

    m_contextMenuOpened = false;
    m_count = 0;

    bool onDropZone = false;

    GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    bool opened = BeginWidget();
    GUI::PopStyle();

    if (opened)
    {
        EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
        EditorContext::Instance().allowGameObjectCopy |= ImGui::IsWindowFocused();

        Scene* scn = EngineContext::Instance().sceneManager->GetCurrentScene();
        onDropZone |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        //  SCENE TITLE BAR
        {
            ImVec2 dim = { ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 2.f };

            GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(7.5f, 2.5f));
            bool barOpened = ImGui::BeginChild("###SceneTitle", dim, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding);
            GUI::PopStyle();

            if (barOpened)
            {
                onDropZone |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

                if (!scn)
                {
                    float textWidth = ImGui::CalcTextSize("No scene opened").x;
                    float width = ImGui::GetContentRegionAvail().x;

                    ImGui::SetCursorPosX(width * 0.5f - textWidth * 0.5f);
                    ImGui::Text("No scene opened");
                    ImGui::Separator();

                    EndWidget();
                    return;
                }

                {
                    float textWidth = ImGui::CalcTextSize(scn->GetName().c_str()).x;
                    float width = ImGui::GetContentRegionAvail().x;

                    ImGui::SetCursorPosX(width * 0.5f - textWidth * 0.5f);
                    ImGui::Text(scn->GetName().c_str());
                }

                ImGui::Separator();
            }
            ImGui::EndChild();
        }

        bool containerOpened = ImGui::BeginChild("###SceneGraphContainer", ImGui::GetContentRegionAvail());

        if (!containerOpened || !scn)
        {
            ImGui::EndChild();
            return;
        }

        onDropZone |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        if (Input::Keyboard::GetKeyPressed(Key::Delete))
        {
            EditorContext::Instance().selectionManager->DeleteGameObjects();
        }

        bool onWindow = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        GUI::PushStyle(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().ItemSpacing.y));
        GUI::PushStyle(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
        DisplayGameObject(&scn->GetRoot());
        GUI::PopStyle(2);

        GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
        bool sceneGraphContextMenu = !m_contextMenuOpened && ImGui::BeginPopupContextWindow("###SCENEGRAPH_CONTEXTMENU");
        GUI::PopStyle();

        if (sceneGraphContextMenu)
        {
            m_tryRename = false;
            m_renamingObject = nullptr;

            GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
            bool createMenuOpened = ImGui::BeginMenu("Create");
            GUI::PopStyle();

            if (createMenuOpened)
            {
                if (ImGui::MenuItem("Empty GameObject"))
                {
                    GameObject* newObject = EngineContext::Instance().sceneManager->Instantiate("New Object");

                    selecManager->ClearGameObjects();
                    selecManager->AddToSelection(*newObject);

                    m_keepFocus = true;

                    m_renameBuffer = newObject->GetName();
                    m_renamingObject = newObject;
                    m_tryRename = true;
                }

                if (ImGui::MenuItem("Cube"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Cube", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\CubeInternal.mesh)"));
                        }
                    );

                    m_keepFocus = true;
                }

                if (ImGui::MenuItem("Capsule"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Capsule", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\CapsuleInternal.mesh)"));
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Sphere"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiateMesh("Sphere", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\SphereInternal.mesh)"));
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Light"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("Light", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\Light.prefab)"));
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("Camera"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("Camera", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\Camera.prefab)"));
                        }
                    );

                    m_keepFocus = true;
                }

                if (ImGui::MenuItem("ParticleSystem"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("ParticleSystem", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\ParticleSystem.prefab)"));
                        }
                    );

                    m_keepFocus = true;
                }


                if (ImGui::MenuItem("SoundEmitter"))
                {
                    EditorContext::Instance().taskQueue->AddTask(
                        [&]()
                        {
                            InstantiatePrefab("SoundEmitter", INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Prefabs\SoundEmitter.prefab)"));
                        }
                    );

                    m_keepFocus = true;
                }
                ImGui::EndMenu();
            }


            ImGui::BeginDisabled(m_copied.empty());
            if (ImGui::MenuItem("paste"))
            {
                Paste();
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }


        if (onDropZone && Input::Mouse::GetButtonUnpressed(MouseButton::Left))
        {
            if (EditorContext::Instance().dragManager->IsDragging() && ExternalDrop(nullptr));

            else if (m_draggingObject && m_draggingSelected)
            {
                for (size_t i = 0; i < selecManager->GetGameObjects().size(); i++)
                {
                    GameObject* obj = selecManager->GetGameObject(i);

                    if (!obj) continue;

                    GameObject* parent = obj->GetParent();

                    if (parent && !(EditorContext::Instance().selectionManager->IsSelected(*parent)))
                    {
                        EditorContext::Instance().taskQueue->AddTask([obj, parent]() { obj->SetParent(parent); });
                        continue;
                    }
                }
                m_draggingObject = false;
            }
            else if (m_draggingObject && m_pressedItem)
            {
                GameObject* obj = m_pressedItem;
                EditorContext::Instance().taskQueue->AddTask([obj]() { obj->SetParent(nullptr); });
                m_pressedItem->SetParent(nullptr);
                m_draggingObject = false;
            }
            EditorContext::Instance().dragManager->EndDrag();
        }

        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && Input::Mouse::GetButtonPressed(MouseButton::Left))
        {
            ResetSelections();
        }
        ImGui::EndChild();
    }
    EndWidget();
}


bool SceneGraphWidget::ExternalDrop(GameObject* parent)
{
    if (!Input::Mouse::GetButtonUnpressed(MouseButton::Left)) return false;

    EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
    Resource* draggedResource = EditorContext::Instance().dragManager->GetDraggedResource();

    if (draggedResource)
    {
        SceneManager* scn = EngineContext::Instance().sceneManager;
        if (!scn) return false;

        const rfk::Class& arch = draggedResource->getArchetype();
        if (arch.isBaseOf(Prefab::staticGetArchetype()) || arch.isSubclassOf(Prefab::staticGetArchetype()))
        {
            EditorContext::Instance().taskQueue->AddTask(
                [scn, draggedResource, parent]()
                {
                    scn->Instantiate(static_cast<Prefab*>(draggedResource), parent);
                }
            );

            EditorContext::Instance().dragManager->EndDrag();
            return true;
        }

        else if (arch.isBaseOf(Mesh::staticGetArchetype()))
        {
            EditorContext::Instance().taskQueue->AddTask(
                [scn, draggedResource, parent]()
                {
                    GameObject* newObject = scn->Instantiate(draggedResource->GetFilename(), parent);
                    MeshComponent* meshComp = newObject->AddComponent<MeshComponent>();
                    meshComp->SetMesh(static_cast<Mesh*>(draggedResource));
                }
            );


            EditorContext::Instance().dragManager->EndDrag();
            return true;
        }

        else if (parent && arch.isBaseOf(Material::staticGetArchetype()))
        {
            if (parent && EditorContext::Instance().selectionManager->IsSelected(*parent))
            {
                for (size_t i = 0 ; i < selecManager->GetGameObjects().size(); i++)
                {
                    GameObject* obj = selecManager->GetGameObject(i);

                    if (!obj) continue;

                    if (MeshComponent* meshComp = nullptr; obj->TryGetComponent<MeshComponent>(&meshComp))
                    {
                        Material* draggedMat = static_cast<Material*>(draggedResource);

                        if (meshComp->materials.empty()) meshComp->materials.AddNewElement();
                        for (auto& mat : meshComp->materials)
                            mat = draggedMat;

                        meshComp->OnModify();
                    }
                    else if (SkeletalMeshComponent* skMeshComp = nullptr; obj->TryGetComponent<SkeletalMeshComponent>(&skMeshComp))
                    {
                        Material* draggedMat = static_cast<Material*>(draggedResource);

                        if (skMeshComp->materials.empty()) skMeshComp->materials.AddNewElement();

                        for (Material* mat : skMeshComp->materials)
                            mat = draggedMat;

                        skMeshComp->OnModify();
                    }
                }
            }
            else
            {
                if (MeshComponent* meshComp = nullptr; parent->TryGetComponent<MeshComponent>(&meshComp))
                {
                    Material* draggedMat = static_cast<Material*>(draggedResource);
                    for (Material* mat : meshComp->materials)
                        mat = draggedMat;
                }
                else if (SkeletalMeshComponent * skMeshComp = nullptr; parent->TryGetComponent<SkeletalMeshComponent>(&skMeshComp))
                {
                    Material* draggedMat = static_cast<Material*>(draggedResource);
                    for (Material* mat : skMeshComp->materials)
                        mat = draggedMat;
                }
            }

            EditorContext::Instance().dragManager->EndDrag();
            return true;
        }
    }

    return false;
}

void SceneGraphWidget::Duplicate()
{
    EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
    const auto& selected = selecManager->GetGameObjects();

    //  Duplicate selected gameObjects

    for (int i = 0; i < selected.size(); i++)
    {
        GameObject* obj = selecManager->GetGameObject(i);

        if (!obj) continue;

        EditorContext::Instance().taskQueue->AddTask([obj] { 
            GameObject* go = SceneManager::DuplicateGameObject(*obj);

            EditorContext::Instance().selectionManager->AddToSelection(*go);
            });
    }

    selecManager->ClearGameObjects();
    m_keepFocus = true;
}

void SceneGraphWidget::Copy(GameObject* gameObject)
{
    //  TODO : Need to copy the gameobject as he is and avoid stocking its pointer
    //  Because the pasted objects will have all modifications that the copied gameobject have gone through

    m_copied.clear();

    if (!gameObject)
    {
        EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
        const auto& selected = selecManager->GetGameObjects();

        for (size_t i = 0; i < selected.size(); i++)
        {
            GameObject* obj = selecManager->GetGameObject(i);

            if (!obj) continue;

            GameObject* parent = obj->GetParent();
            if (parent)
            {
                //  only register parent in copied object list
                if (!EditorContext::Instance().selectionManager->IsSelected(*parent))
                {
                    m_copied.push_back(obj);
                }
            }
            else
            {
                m_copied.push_back(obj);
            }
        }
    }
    else
    {
        m_copied.push_back(gameObject);
    }


    EditorContext::Instance().selectionManager->ClearGameObjects();
}

void SceneGraphWidget::Paste(GameObject* parent)
{
    EditorContext::Instance().selectionManager->ClearGameObjects();

    if (parent)
    {
        for (GameObject* copiedGameObject : m_copied)
            EditorContext::Instance().taskQueue->AddTask([parent, copiedGameObject]
                {
                    GameObject* go = SceneManager::DuplicateGameObject(*copiedGameObject);
                    go->SetParent(parent);

                    EditorContext::Instance().selectionManager->AddToSelection(*go);
                });
    }
    else
    {
        for (GameObject* copiedGameObject : m_copied)
            EditorContext::Instance().taskQueue->AddTask([copiedGameObject]
                {
                    GameObject* go = SceneManager::DuplicateGameObject(*copiedGameObject);

                    EditorContext::Instance().selectionManager->AddToSelection(*go);
                });
    }
    m_keepFocus = true;
}


GameObject* SceneGraphWidget::InstantiateMesh(const std::string& name, const std::string& path, GameObject* parent)
{
    Mesh* mesh = static_cast<Mesh*>(EngineContext::Instance().resourcesManager->GetResourceByPath(path));

    if (mesh == nullptr)
    {
        Logger::Error("InstantiatePrefab - SceneGraph - Couldn't add Mesh, because the mesh resource could not be found");
        return nullptr;
    }

    SceneManager* scn = EngineContext::Instance().sceneManager;

    GameObject* newObject = scn->Instantiate(name, nullptr);
    if (parent) newObject->SetParent(parent);

    MeshComponent* meshComp = newObject->AddComponent<MeshComponent>();
    meshComp->SetMesh(static_cast<Mesh*>(mesh));

    EditorContext::Instance().selectionManager->ClearGameObjects();
    EditorContext::Instance().selectionManager->AddToSelection(*newObject);

    m_renameBuffer = newObject->GetName();
    m_renamingObject = newObject;
    m_tryRename = true;

    return newObject;
}

GameObject* SceneGraphWidget::InstantiatePrefab(const std::string& name, const std::string& path, GameObject* parent)
{
    Prefab* prefab = static_cast<Prefab*>(EngineContext::Instance().resourcesManager->GetResourceByPath(path));

    if (prefab == nullptr)
    {
        Logger::Error("InstantiatePrefab - SceneGraph - Couldn't add prefab, because the prefab resource could not be found");
        return nullptr;
    }

    SceneManager* scn = EngineContext::Instance().sceneManager;
    GameObject* newObject = scn->Instantiate(prefab, parent);

    if (parent) newObject->SetParent(parent);

    EditorContext::Instance().selectionManager->ClearGameObjects();
    EditorContext::Instance().selectionManager->AddToSelection(*newObject);

    m_renameBuffer = newObject->GetName();
    m_renamingObject = newObject;
    m_tryRename = true;

    return newObject;
}