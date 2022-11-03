#include <Widgets/WidgetManager.hpp>

#include <string>

#include <imgui/imgui.h>

#include <EditorContext.hpp>
#include <EditorTheme.hpp>

WidgetManager::WidgetManager()
{
}

template<typename T>
void ClearPointerVector(std::vector<T*>& vector )
{

	while (vector.empty() == false)
	{
		delete vector.back();
		vector.pop_back();
	}
}

WidgetManager::~WidgetManager()
{
	m_widgetList.clear();
}


template<typename T>
bool AddToArray(T** list, T* element, EditorWidget*& globalList)
{
	for (int i = 0; i < MAX_DUPLICATED_WIDGET; i++)
	{
		if (list[i] == nullptr)
		{
			list[i] = element;
			globalList = list[i];

			return true;
		}
	}
	return false;
}


template<typename T>
int GetFreeIndex(const std::unique_ptr<T>* list)
{
	for (int i = 0; i < MAX_DUPLICATED_WIDGET; i++)
	{
		if (list[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

RenderSceneWidget& WidgetManager::GetSceneWidget() const
{
	return *m_sceneWindow.get();
}

void WidgetManager::AddWidget(const unsigned int type, const std::string& name)
{
	EditorWidget* newWidget = nullptr;

	int i = -1;

	switch (type)
	{

	//	Unique widget

	case WidgetTypes_ToolBar: 

		if (m_toolbar == nullptr)
		{
			m_toolbar = std::make_unique<ToolbarWidget>(name, 0, true);
			newWidget = m_toolbar.get();
			break;
		}
		return;

	case WidgetTypes_Game:

		if (m_gameWindow == nullptr)
		{
			m_gameWindow = std::make_unique<RenderGameWidget>(name, 0, true);
			newWidget = m_gameWindow.get();
			break;
		}
		return;

	case WidgetTypes_Console:

		if (m_console == nullptr)
		{
			m_console = std::make_unique<ConsoleWidget>(name, 0, true);
			newWidget = m_console.get();
			break;
		}
		return;

	case WidgetTypes_SceneGraph:

		if (m_sceneGraph == nullptr)
		{
			m_sceneGraph = std::make_unique<SceneGraphWidget>(name, 0, true);
			newWidget = m_sceneGraph.get();
			break;
		}
		return;

	case WidgetTypes_Scene:
		if (m_sceneWindow == nullptr)
		{
			m_sceneWindow = std::make_unique<RenderSceneWidget>(name, 0, true);
			newWidget = m_sceneWindow.get();
			break;
		}
		return;


	//	Multiplicable Widgets

	case WidgetTypes_GameObjectInspector:
		i = GetFreeIndex(m_gameObjectInspectors);
		if (i >= 0)
		{
			m_gameObjectInspectors[i] = std::make_unique<GameObjectInspectorWidget>(name, i, true);
			newWidget = m_gameObjectInspectors[i].get();
			break;
		}
		return;

	case WidgetTypes_ResourceInspector:
		i = GetFreeIndex(m_resourceInspectors);
		if (i >= 0)
		{
			m_resourceInspectors[i] = std::make_unique<ResourceInspectorWidget>(name, i, true);
			newWidget = m_resourceInspectors[i].get();
			break;
		}
		return;

	case WidgetTypes_ContentBrowser:
		i = GetFreeIndex(m_contentBrowsers);
		if (i >= 0)
		{
			m_contentBrowsers[i] = std::make_unique<ContentBrowserWidget>(name, i, true);
			newWidget = m_contentBrowsers[i].get();
			break;
		}
		return;

	default: 
		//	TODO : Warning
		return;
	}

	m_widgetList.push_back(newWidget);
}


void WidgetManager::UpdateWidgets()
{

	for (EditorWidget* widget : m_widgetList)
	{
		widget->Update();
	}
}

void WidgetManager::DisplayWidgets()
{
	for (EditorWidget* widget : m_widgetList)
	{
		widget->Display();
	}
}



void WidgetManager::WidgetMenuBar()
{

	if (ImGui::BeginMenu("Widgets"))
	{
		//	Show / Hide widgets
		//	-------------------

		int id = 0;
		for (EditorWidget* widget : m_widgetList)
		{
			if (widget == m_toolbar.get()) continue;

			std::string label = widget->GetName() + "###" + std::to_string(id);
			ImGui::MenuItem(label.c_str(), "", &widget->isActive);

			id++;
		}

		//	Add widget
		//	----------

		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("GameObject Inspector"))
			{
				AddWidget(WidgetTypes_GameObjectInspector, "GameObject Inspector");
			}

			if (ImGui::MenuItem("Resource Inspector"))
			{
				AddWidget(WidgetTypes_ResourceInspector, "Resource Inspector");
			}

			if (ImGui::MenuItem("Content browser"))
			{
				AddWidget(WidgetTypes_ContentBrowser, "Content Browser");
			}

			ImGui::EndMenu();
		}

		//	Remove widgets
		//	--------------

		if (ImGui::BeginMenu("Remove"))
		{
			if (ImGui::BeginMenu("GameObject Inspector"))
			{
				for (int i = 0; i < MAX_DUPLICATED_WIDGET; i++)
				{
					if (m_gameObjectInspectors[i] == nullptr) continue;

					if (ImGui::MenuItem(m_gameObjectInspectors[i]->GetName().c_str()))
					{
						auto it = std::find(m_widgetList.begin(), m_widgetList.end(), m_gameObjectInspectors[i].get());

						if (it != m_widgetList.end())
						{
							m_widgetList.erase(it);
						}

						m_gameObjectInspectors[i].reset();
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Resource Inspector"))
			{
				for (int i = 0; i < MAX_DUPLICATED_WIDGET; i++)
				{
					if (m_resourceInspectors[i] == nullptr) continue;

					if (ImGui::MenuItem(m_resourceInspectors[i]->GetName().c_str()))
					{
						auto it = std::find(m_widgetList.begin(), m_widgetList.end(), m_resourceInspectors[i].get());

						if (it != m_widgetList.end())
						{
							m_widgetList.erase(it);
						}

						m_resourceInspectors[i].reset();
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Content browser"))
			{
				for (int i = 0; i < MAX_DUPLICATED_WIDGET; i++)
				{
					if (m_contentBrowsers[i] == nullptr) continue;

					if (ImGui::MenuItem(m_contentBrowsers[i]->GetName().c_str()))
					{
						auto it = std::find(m_widgetList.begin(), m_widgetList.end(), m_contentBrowsers[i].get());

						if (it != m_widgetList.end())
						{
							m_widgetList.erase(it);
						}

						m_contentBrowsers[i].reset();
					}
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}