#include "Widgets/ContentBrowserWidget.hpp"

#include <deque>
#include <queue>
#include <algorithm>
#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <IO/Keyboard.hpp>
#include <IO/Mouse.hpp>
#include <IO/Window.hpp>
#include <Core/Time.hpp>
#include <Core/TaskQueue.hpp>
#include <EngineContext.hpp>
#include <Renderer/RenderSystem.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Resources/Resource/Mesh.hpp>
#include <Resources/Resource/Material.hpp>
#include <Resources/Resource/Skybox.hpp>
#include <Resources/Resource/Scene.hpp>
#include <Resources/Resource/Script.hpp>
#include <Resources/Resource/Terrain.hpp>
#include <Resources/Loaders/ResourcesLoader.hpp>
#include <Resources/SceneManager.hpp>
#include <Refureku/Refureku.h>
#include <Tools/Curves.hpp>

#include "PathConfig.hpp"
#include "EditorContext.hpp"
#include "EditorResourcePreview.hpp"
#include "EditorSelectionManager.hpp"
#include "GUI.hpp"
#include "Widgets/WidgetManager.hpp"
#include "Widgets/RenderSceneWidget.hpp"
#include "EditorData.hpp"
#include "EditorDragManager.hpp"
#include "Tools/ScriptBuilder.hpp"


namespace fs = std::filesystem;

struct FileBase
{
	fs::path path;
};

struct File : public FileBase
{
	Resource* resource = nullptr;

	unsigned int thumbnailTexture = 0;
};

struct Directory : public FileBase
{
	std::vector<Directory> subdirectories;
};

struct ActiveDirectory : public Directory
{
	std::vector<File> files;

	int selectedChildID = -1;
};

struct PimplContentBrowser
{
	ActiveDirectory activeDirectory;

	unsigned int backArrowTexture = 0;
	unsigned int searchTexture = 0;
	unsigned int folderIconTexture = 0;
	unsigned int scriptIconTexture = 0;
	unsigned int sceneIconTexture = 0;
	unsigned int terrainIconTexture = 0;
	unsigned int skyboxIconTexture = 0;
	unsigned int prefabTexture = 0;

	float currentSearchOffset = 0.0f;
	float elapsedTimeSearchBar = 0.0f;
	float searchBarAppearTimeLimit = 0.2f;

	bool  contextMenuOpened = false;
	bool  consumeClick = false;
	bool  creationPopupOpened = false;
	bool  searchButtonPressed = false;

	void* draggedObject = nullptr;
	bool  onDrag = false;
	bool  refreshFolder = false;
	bool  displaySearchBar = false;

	std::string m_draggedFilename = "";
	std::string searchedString = "";
	std::string renameBuffer = "";

	char filenameCreateBuffer[256];

	FileBase* renamingFile = nullptr;

	const Directory* pressedDir;
	Directory rootDir;

	CallbackID focusReloadID;

	std::vector<std::string> showedExtensions = 
	{
		".model",
		".scene",
		".texture",
		".mat",
		".mesh",
		".skmesh",
		".skybox",
		".script",
		".prefab",
		".terrain",
		".sound"
	};

	std::vector<std::string> resourceExtensions =
	{
		".model",
		".scene",
		".texture",
		".mat",
		".mesh",
		".skmesh",
		".skybox",
		".terrain",
		".prefab",
		".script",
		".sound"
	};
};


ContentBrowserWidget::ContentBrowserWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive) : EditorWidget(widgetName, widgetID, isActive, "###CONTENT_BROWSER")
{
	m_flag |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	m_pimpl = std::make_unique<PimplContentBrowser>();
	m_pimpl->activeDirectory.path = ASSETS_ROOT_RAW;
	m_pimpl->rootDir.path = ASSETS_ROOT_RAW;

	EditorImages* img = EditorContext::Instance().images;
	m_pimpl->backArrowTexture = img->LoadImage(R"(Icons\BackArrow.texture)");
	m_pimpl->searchTexture = img->LoadImage(R"(Icons\Search.texture)");
	m_pimpl->folderIconTexture = img->LoadImage(R"(Icons\FolderIcon.texture)");
	m_pimpl->scriptIconTexture = img->LoadImage(R"(Icons\Script.texture)");
	m_pimpl->sceneIconTexture = img->LoadImage(R"(Icons\Scene.texture)");
	m_pimpl->skyboxIconTexture = img->LoadImage(R"(Icons\Skybox.texture)");
	m_pimpl->terrainIconTexture = img->LoadImage(R"(Icons\Terrain.texture)");
	m_pimpl->prefabTexture = img->LoadImage(R"(Icons\Prefab.texture)");
	
	LoadDirectoryTree(m_pimpl->rootDir);
	
	m_pimpl->focusReloadID = EditorContext::Instance().window->focusEvent.AddCallback(&ContentBrowserWidget::CheckResourceToReload, *this);
	m_pimpl->focusReloadID = EditorContext::Instance().window->focusEvent.AddCallback(&ContentBrowserWidget::LoadActiveDirectory, *this);

	//
	LoadActiveDirectory();
}


ContentBrowserWidget::~ContentBrowserWidget()
{
	EditorContext::Instance().window->focusEvent.RemoveCallback(m_pimpl->focusReloadID);
}


void ContentBrowserWidget::Update()
{
	m_pimpl->consumeClick = false;
	if (m_pimpl->refreshFolder)
	{
		LoadActiveDirectory();
	}
}


void ContentBrowserWidget::ChangeDirectory(const std::filesystem::path& directory)
{
	m_pimpl->activeDirectory.path = directory;
	LoadActiveDirectory();
}


void ContentBrowserWidget::LoadDirectoryTree(Directory& directory)
{
	UnloadDirectoryTree(directory);

	for (const fs::directory_entry& dir_entry : fs::directory_iterator{ directory.path })
	{
		//	If it is a directory
		if (dir_entry.is_directory())
		{
			Directory& subDir = directory.subdirectories.emplace_back();
			subDir.path = dir_entry.path();

			LoadDirectoryTree(subDir);
		}
	}
}

void ContentBrowserWidget::UnloadDirectoryTree(Directory& directory)
{
	for (Directory& dir : directory.subdirectories)
	{
		UnloadDirectoryTree(dir);
	}
	directory.subdirectories.clear();
}

void ContentBrowserWidget::CheckResourceToReload()
{
	ResourcesLoader::LoadNotCachedResources(m_pimpl->rootDir.path.string());
}

void ContentBrowserWidget::LoadActiveDirectory()
{
	m_pimpl->activeDirectory.subdirectories.clear();
	m_pimpl->activeDirectory.files.clear();

	m_pimpl->activeDirectory.selectedChildID = -1;
	m_pimpl->refreshFolder = false;


	for (const fs::directory_entry& dir_entry : fs::directory_iterator{ m_pimpl->activeDirectory.path })
	{
		//	If it is a directory
		if (dir_entry.is_directory())
		{
			Directory& subDir = m_pimpl->activeDirectory.subdirectories.emplace_back();
			subDir.path = dir_entry.path();
		}

		//	Else if it is a file
		else
		{
			std::string extension = dir_entry.path().extension().string();

			if (std::find(m_pimpl->showedExtensions.begin(), m_pimpl->showedExtensions.end(), extension) != m_pimpl->showedExtensions.end())
			{
				File& file = m_pimpl->activeDirectory.files.emplace_back();

				file.path = dir_entry.path();

				//	If it is a resource file
				if (std::find(m_pimpl->resourceExtensions.begin(), m_pimpl->resourceExtensions.end(), extension) != m_pimpl->resourceExtensions.end())
				{
					file.resource = EngineContext::Instance().resourcesManager->GetResourceByPath(file.path.string());
					if (file.resource)
					{
						file.thumbnailTexture = EditorContext::Instance().resourcePreview->GetResourcePreview(*file.resource);

						if (file.thumbnailTexture == 0)
						{
							const rfk::Class& classOfResource = file.resource->getArchetype();

							if	    (classOfResource == Script::staticGetArchetype())  file.thumbnailTexture = m_pimpl->scriptIconTexture;
							else if (classOfResource == Scene::staticGetArchetype())   file.thumbnailTexture = m_pimpl->sceneIconTexture;
							else if (classOfResource == Terrain::staticGetArchetype()) file.thumbnailTexture = m_pimpl->terrainIconTexture;
							else if (classOfResource == Skybox::staticGetArchetype())  file.thumbnailTexture = m_pimpl->skyboxIconTexture;
							else if (classOfResource == Prefab::staticGetArchetype())  file.thumbnailTexture = m_pimpl->prefabTexture;
						}
					}
				}
			}
		}
	}
}


void ContentBrowserWidget::Display()
{
	if (!isActive) return;

	m_pimpl->contextMenuOpened = false;

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool opened = BeginWidget();
	GUI::PopStyle();

	if (opened)
	{

		ImVec2 tableDimensions = ImGui::GetContentRegionAvail();
		if (ImGui::BeginTable("###ContentBrowserTable", 2, ImGuiTableFlags_SizingFixedFit, tableDimensions))
		{
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), 0, tableDimensions.x * 0.2f);
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), 0, tableDimensions.x * 0.8f);

			ImGui::TableNextColumn();

			DisplayFolderExplorer();

			ImGui::TableNextColumn();

			//	Folder navigation display
			DisplayNavigationBar();

			//	Content display
			DisplayContent();

			ImGui::EndTable();
		}
	}
	EndWidget();
}



void ContentBrowserWidget::DisplayNavigationBar()
{
	const fs::path& current = m_pimpl->activeDirectory.path;


	fs::path root(ASSETS_ROOT_RAW);

	//	Use deque to print button folder in the right way (parent/child/...)
	std::deque<fs::path> parentPath;

	//	Initialize deque
	{
		parentPath.push_back(current);
		fs::path parent = current;

		while (parent != root)
		{
			parent = parent.parent_path();
			parentPath.push_front(parent);

			if (parent.string().empty()) break;
		}
	}

	//	Use particular style for these buttons
	GUI::PushColor(ImGuiCol_Button, Color4(0, 0, 0, 0));
	GUI::PushColor(ImGuiCol_ButtonHovered, Color4(0, 0, 0, 25));
	GUI::PushColor(ImGuiCol_ButtonActive, Color4(0, 0, 0, 50));

	float panelWidth = ImGui::GetContentRegionAvail().x;
	float panelHeight = ImGui::GetFontSize() + ImGui::GetStyle().WindowPadding.y + ImGui::GetStyle().ScrollbarSize;

	float height = ImGui::GetFontSize();
	ImVec2 buttonDimensions = ImVec2(height * 1.25f, height * 1.25f);

	if (ImGui::BeginTable("FolderNav", 3, ImGuiTableFlags_SizingFixedFit, ImVec2(panelWidth, panelHeight)))
	{
		//	TABLE INITIALIZATION

		ImGuiTable* table = ImGui::GetCurrentContext()->CurrentTable;
	
		float firstColumnWidth  = buttonDimensions.x * 2.f;				//	Back button
		float secondColumnWidth = panelWidth * 0.7f - firstColumnWidth;	//	Navigation folders
		float thirdColumnWidth  = panelWidth * 0.3f - firstColumnWidth; //	SearchBar

		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), 0, firstColumnWidth);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), 0, secondColumnWidth);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(2), 0, thirdColumnWidth);

		//	TABLE START

		//	BACK BUTTON COLUMN

		ImGui::TableNextColumn();

		ImGui::BeginDisabled(current == root);

		ImGui::SetCursorPosY(ImGui::GetCursorPos().y - (buttonDimensions.y - ImGui::GetFont()->FontSize) * 0.5f);
		if (ImGui::ImageButton(GUI::CastTextureID(m_pimpl->backArrowTexture), buttonDimensions, ImVec2(0, 1), ImVec2(1, 0), -1))
		{
			EditorContext::Instance().taskQueue->AddTask([&, current]() {
				ChangeDirectory(current.parent_path());
				});
		}
		ImGui::EndDisabled();
		
		//	NAVIGATION BAR COLUMN

		ImGui::TableNextColumn();

		if (ImGui::BeginTable("FolderNavPath", static_cast<int>(parentPath.size()), ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX, ImVec2(panelWidth * 0.8f - buttonDimensions.x * 2.f, panelHeight)))
		{
			int ID = 0;

			//	Draw buttons for navigations
			for (const fs::path& path : parentPath)
			{
				//	Change to next column
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();

				std::string str_ID = "###" + std::to_string(ID);
				std::string buttonName = path.filename().string() + str_ID;

				if (ImGui::Button(buttonName.c_str()))
				{
					EditorContext::Instance().taskQueue->AddTask([&, path]() {
						ChangeDirectory(path);
						});
				}

				if (path == parentPath.back()) break;

				ImGui::SameLine();
				ImGui::Text(" >> ");

				ID++;
			}
			//	End the table
			ImGui::EndTable();
		}

		//	SEARCH BAR COLUMN

		ImGui::TableNextColumn();

		//	Save current cursor position
		float xPos = ImGui::GetCursorPosX();

		//	Precalculate search bar max width (remove button space from avalaible width)
		float searchBarMaxWidth = thirdColumnWidth - (buttonDimensions.x + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2.f);

		//	Lerp animation variables
		m_pimpl->elapsedTimeSearchBar = Maths::Min(m_pimpl->elapsedTimeSearchBar + EngineContext::Instance().time->GetUnscaledDeltaTime(), m_pimpl->searchBarAppearTimeLimit);
		float deltaLerp = m_pimpl->elapsedTimeSearchBar / m_pimpl->searchBarAppearTimeLimit;

		float offset = m_pimpl->displaySearchBar ?
			EasingCurveFunctions::Lerp(searchBarMaxWidth, 0.0f, EasingCurve::EaseInQuart, deltaLerp):
			EasingCurveFunctions::Lerp(0.0f, searchBarMaxWidth, EasingCurve::EaseOutQuart, deltaLerp);
		
		if (offset != searchBarMaxWidth)
		{
			if (m_pimpl->displaySearchBar && m_pimpl->searchButtonPressed)
			{
				ImGui::SetKeyboardFocusHere(0);
				m_pimpl->searchButtonPressed = false;
			}

			char* searchedString = const_cast<char*>(m_pimpl->searchedString.c_str());

			float alpha = m_pimpl->displaySearchBar ?
				EasingCurveFunctions::Lerp(0.0f, 1.0f, EasingCurve::EaseInQuart, deltaLerp) :
				EasingCurveFunctions::Lerp(1.0f, 0.0f, EasingCurve::EaseOutQuart, deltaLerp);

			GUI::PushStyle(ImGuiStyleVar_Alpha, alpha);

			ImGui::SetCursorPosX(xPos + offset);
			ImGui::SetNextItemWidth(searchBarMaxWidth - offset);
			if (ImGui::InputText("###ContentBrowserSearchBar", searchedString, 50))
			{
				m_pimpl->searchedString = std::string(searchedString);
			}
			GUI::PopStyle();

			ImGui::SameLine();
		}
		
		ImGui::SetCursorPosX(xPos + searchBarMaxWidth + ImGui::GetStyle().ItemSpacing.x);
		ImGui::SetCursorPosY(ImGui::GetCursorPos().y - (buttonDimensions.y - ImGui::GetFont()->FontSize) * 0.5f);

		if (ImGui::ImageButton(GUI::CastTextureID(m_pimpl->searchTexture), buttonDimensions, ImVec2(0, 1), ImVec2(1, 0), -1))
		{
			m_pimpl->searchedString = "";
			m_pimpl->displaySearchBar = !m_pimpl->displaySearchBar;
			m_pimpl->elapsedTimeSearchBar = 0.0f;
			m_pimpl->searchButtonPressed = true;
		}
		else
		{
			m_pimpl->searchButtonPressed = false;
		}

		//	End the table
		ImGui::EndTable();
	}

	//	Remove the button style
	GUI::PopColor(3);

	ImGui::Separator();
}


void ContentBrowserWidget::DisplayDirectoryTree(Directory& dir)
{
	static ImGuiTreeNodeFlags defaultTreeNodeFlag =
		ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth |
		ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoAutoOpenOnLog;

	ImGuiTreeNodeFlags flag = defaultTreeNodeFlag;

	if (dir.path == m_pimpl->activeDirectory.path)
	{
		flag |= ImGuiTreeNodeFlags_Selected;
	}
	if (dir.subdirectories.empty()) flag |= ImGuiTreeNodeFlags_Leaf;

	bool open = ImGui::TreeNodeEx(dir.path.filename().string().c_str(), flag);

	if (ImGui::IsItemHovered())
	{
		if (Input::Mouse::GetButtonPreciseClicked(MouseButton::Left))
		{
			EditorContext::Instance().taskQueue->AddTask([&, dir]() {
				ChangeDirectory(dir.path);
				});
		}
	}

	if (open)
	{
		ImGui::TreePush(dir.path.filename().string().c_str());
		for (Directory& subdir : dir.subdirectories)
		{
			DisplayDirectoryTree(subdir);
		}
		ImGui::TreePop();
	}
}

void ContentBrowserWidget::DisplayFolderExplorer()
{
	GUI::PushColor(ImGuiCol_ChildBg, Color4(0, 0, 0, 50));
	bool open = ImGui::BeginChild("###DirectoriesTree", ImGui::GetContentRegionAvail(), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
	GUI::PopColor();

	if (open)
	{
		GUI::PushStyle(ImGuiStyleVar_IndentSpacing, 10.f);
		DisplayDirectoryTree(m_pimpl->rootDir);
		GUI::PopStyle();
	}
	ImGui::EndChild();
}

GameObject* DEBUG_Go = nullptr;

void ContentBrowserWidget::DisplayContent()
{
	const fs::path& current = m_pimpl->activeDirectory.path;

	//	Get Window Dimensions
	float panelHeight = ImGui::GetContentRegionAvail().y;
	float panelWidth = ImGui::GetContentRegionAvail().x;

	float padding = EditorContext::Instance().editorDatas->contentBrowser.filePadding;
	float width = padding + EditorContext::Instance().editorDatas->contentBrowser.fileThumbnailSize;

	//	Compute column count from dimension, padding and thumbnail size
	int columnCount = Maths::Min((int)(panelWidth / width), IMGUI_TABLE_MAX_COLUMNS);

	//	Force ColumnCount to one
	columnCount = columnCount < 1 ? 1 : columnCount;

	//	Create Child Window
	ImGui::BeginChild("Content", ImVec2(panelWidth, panelHeight), true);

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool ContentBrowserContextMenu = !m_pimpl->contextMenuOpened && ImGui::BeginPopupContextWindow("###CONTENTBROWSER_CONTEXTMENU");

	std::string popupToOpen = "";

	//	Context menu
	if (ContentBrowserContextMenu)
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				popupToOpen = "###CreateFolder";
				m_pimpl->consumeClick = true;
			}

			if (ImGui::MenuItem("Material"))
			{
				popupToOpen = "###CreateMaterial";
				m_pimpl->consumeClick = true;
			}

			if (ImGui::MenuItem("Scene"))
			{
				popupToOpen = "###CreateScene";
				m_pimpl->consumeClick = true;
			}

			if (ImGui::MenuItem("Script"))
			{
				popupToOpen = "###CreateScript";
				m_pimpl->consumeClick = true;
			}

			if (ImGui::MenuItem("Skybox"))
			{
				popupToOpen = "###CreateSkybox";
				m_pimpl->consumeClick = true;
			}

			if (ImGui::MenuItem("Terrain"))
			{
				popupToOpen = "###CreateTerrain";
				m_pimpl->consumeClick = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Paste"))
		{
		}

		ImGui::EndPopup();
	}

	GUI::PopStyle();

	if (!popupToOpen.empty())ImGui::OpenPopup(popupToOpen.c_str());


	DisplayCreationPopup("###CreateFolder", "Name the new folder", "NewFolder",
		[&, current](const std::string& text)
		{
			std::filesystem::create_directory(current.string() + '\\' + text.c_str());
			LoadDirectoryTree(m_pimpl->rootDir);
		}
	);

	DisplayCreationPopup("###CreateMaterial", "Name the new material", "NewMaterial",
		[current](const std::string& text)
		{
			std::string path = current.string() + '\\' + std::string(text) + ".mat";
			EngineContext::Instance().resourcesManager->CreateResource<Material>(path);
		}
	);

	DisplayCreationPopup("###CreateScene", "Name the new scene", "NewScene",
		[current](const std::string& text)
		{
			EngineContext& engine = EngineContext::Instance();

			RenderSystem& renderSystem = SystemManager::GetRenderSystem();

			Skybox* prevSkybox = renderSystem.GetAttachedSkybox();
			renderSystem.AttachNewSkybox(nullptr);

			std::string path = current.string() + '\\' + std::string(text) + ".scene";
			Scene* newScene = engine.resourcesManager->CreateResource<Scene>(path);
			EditorContext::Instance().widgetManager->GetSceneWidget().CreateSaveSceneCamera(*newScene);

			renderSystem.AttachNewSkybox(prevSkybox);
		}
	);

	DisplayCreationPopup("###CreateScript", "Name the new script", "NewScript",
		[current](const std::string& text)
		{
			std::string path = current.string() + '\\' + std::string(text) + ".script";
			Script* newScript = EngineContext::Instance().resourcesManager->CreateResource<Script>(path);
			ScriptBuilder::ConstructScript(text);
		}
	);

	DisplayCreationPopup("###CreateSkybox", "Name the new skybox", "NewSkybox",
		[current](const std::string& text)
		{
			std::string path = current.string() + '\\' + std::string(text) + ".skybox";
			EngineContext::Instance().
				EngineContext::Instance().resourcesManager->CreateResource<Skybox>(path);
		}
	);

	DisplayCreationPopup("###CreateTerrain", "Name the new terrain", "NewTerrain",
		[current](const std::string& text)
		{
			std::string path = current.string() + '\\' + std::string(text) + ".terrain";
			EngineContext::Instance().
				EngineContext::Instance().resourcesManager->CreateResource<Terrain>(path);
		}
	);

	//	Create Table with fixed size to not do some strange shifting while sizing the window
	//	Also change CellPadding stylevar for row height changing depending on the padding
	GUI::PushStyle(ImGuiStyleVar_CellPadding, ImVec2(0.0f, padding * 0.25f));
	if (ImGui::BeginTable("ContentTable", columnCount, ImGuiTableFlags_SizingFixedFit))
	{
		//	Predefine width of each columns
		for (int column = 0; column < columnCount; column++)
		{
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(column), 0, width);
		}

		int ID = 0;

		bool filtering = !m_pimpl->searchedString.empty();

		for (Directory& directory : m_pimpl->activeDirectory.subdirectories)
		{
			if (filtering)
			{
				//	Insensitive Case search

				std::string dirName = directory.path.filename().string();
				std::transform(dirName.begin(), dirName.end(), dirName.begin(), ::tolower);

				std::string searched = m_pimpl->searchedString;
				std::transform(searched.begin(), searched.end(), searched.begin(), ::tolower);

				if (dirName.find(searched) == std::string::npos) continue;
			}

			//	Change to next column
			ImGui::TableNextColumn();

			bool test = DisplayDirectory(directory, ID);

			if (!test)
				break;

			ID++;
		}

		for (File& file : m_pimpl->activeDirectory.files)
		{
			if (filtering)
			{
				std::string filename = file.path.filename().string();
				std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

				std::string searched = m_pimpl->searchedString;
				std::transform(searched.begin(), searched.end(), searched.begin(), ::tolower);

				if (filename.find(searched) == std::string::npos) continue;
			}

			//	Change to next column
			ImGui::TableNextColumn();

			DisplayFile(file, ID);

			ID++;
		}

		//	End the table
		ImGui::EndTable();
	}
	GUI::PopStyle(1);


	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
	{
		if (!ImGui::IsAnyItemHovered() && Input::Mouse::GetButtonPreciseClicked(MouseButton::Left))
		{
			m_pimpl->activeDirectory.selectedChildID = -1;
		}

		GameObject* dragged = EditorContext::Instance().dragManager->GetDraggedGameObject();

		if (dragged)
		{
			if (Input::Mouse::GetButtonUnpressed(MouseButton::Left))
			{
				Prefab& prefab = *EngineContext::Instance().resourcesManager->CreateResource<Prefab>();
				prefab.CreateFromGameObject(dragged, dragged->GetName(), m_pimpl->activeDirectory.path.string());

				LoadActiveDirectory();
				EditorContext::Instance().dragManager->EndDrag();
			}
		}
	}

	//	End the child window
	ImGui::EndChild();
}


void ContentBrowserWidget::DisplayCreationPopup(const char* popupID, const char* message, const std::string& defaultFilename, const std::function<void(const std::string&)>& createFunction)
{
	float  width = EditorContext::Instance().window->GetWidth() * 0.25f;
	float  height = EditorContext::Instance().window->GetHeight() * 0.15f;
	float  widthPadding = width * 0.1f;
	float  heightPadding = height * 0.1f;

	ImVec2 buttonDim = ImVec2(width * 0.25f, height * 0.2f);

	ImGui::SetNextWindowSize(ImVec2(width, height));

	if (ImGui::BeginPopupModal(popupID, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
	{
		bool close = false;
		if (!m_pimpl->consumeClick && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && Input::Mouse::WasClicked())
		{
			close = true;
		}


		if (!m_pimpl->creationPopupOpened)
		{
			memcpy(m_pimpl->filenameCreateBuffer, &defaultFilename[0], defaultFilename.size());
			m_pimpl->filenameCreateBuffer[defaultFilename.size()] = '\0';
		}

		ImGui::SetCursorPosX(width * 0.5f - ImGui::CalcTextSize(message).x * 0.5f);
		ImGui::Text(message);

		ImGui::SetCursorPosX(widthPadding);
		ImGui::SetNextItemWidth(width - 2.f * widthPadding);

		GUI::PushStyle(ImGuiStyleVar_FrameRounding, 1.f);

		ImGui::InputText("###NEW_FILENAME", m_pimpl->filenameCreateBuffer, 256);

		bool create = false;

		if (ImGui::IsItemFocused() && Input::Keyboard::GetKeyUnpressed(Key::Enter))
		{
			create = true;
		}

		ImGui::SetCursorPosY(height - heightPadding - buttonDim.y);
		ImGui::SetCursorPosX(widthPadding);

		GUI::PushColor(ImGuiCol_Button, Color4::DarkGreen);
		GUI::PushColor(ImGuiCol_ButtonHovered, Color4::DarkGreen * 0.75f);
		GUI::PushColor(ImGuiCol_ButtonActive, Color4::DarkGreen * 0.50f);

		create |= ImGui::Button("Create", buttonDim);

		GUI::PopColor(3);

		if (create)
		{
			bool filenameIsTaken = false;
			for (const File& file : m_pimpl->activeDirectory.files)
			{
				std::string currentFilename = StringHelper::GetFileNameWithoutExtension(file.path.filename().string());
				if (currentFilename.compare(m_pimpl->filenameCreateBuffer) == 0)
				{
					filenameIsTaken = true;
				}
			}

			for (const Directory& dir : m_pimpl->activeDirectory.subdirectories)
			{
				std::string currentFilename = dir.path.filename().string();
				if (currentFilename.compare(m_pimpl->filenameCreateBuffer) == 0)
				{
					filenameIsTaken = true;
				}
			}

			if (!filenameIsTaken && std::string(m_pimpl->filenameCreateBuffer).compare("") != 0)
			{
				createFunction(m_pimpl->filenameCreateBuffer);
				m_pimpl->refreshFolder = true;

				close = true;
			}
		}


		ImGui::SameLine();

		ImGui::SetCursorPosX(width - widthPadding - width * 0.25f);

		GUI::PushColor(ImGuiCol_Button, Color4::DarkRed);
		GUI::PushColor(ImGuiCol_ButtonHovered, Color4::DarkRed * 0.75f);
		GUI::PushColor(ImGuiCol_ButtonActive, Color4::DarkRed * 0.50f);

		if (ImGui::Button("Cancel", buttonDim))
		{
			close = true;
		}

		GUI::PopColor(3);
		GUI::PopStyle();

		if (close)
		{
			m_pimpl->filenameCreateBuffer[0] = '\0';
			m_pimpl->creationPopupOpened = false;

			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();

			return;
		}

		ImGui::EndPopup();

		m_pimpl->creationPopupOpened = true;
	}
}


void ContentBrowserWidget::DisplayFile(File& file, const int ID)
{
	std::string str_ID = "###" + std::to_string(ID);
	std::string name = file.path.filename().string();
	std::string ext = file.path.filename().extension().string();

	float thumbnailSize = EditorContext::Instance().editorDatas->contentBrowser.fileThumbnailSize;
	std::string fullName = name + str_ID;
	bool  isSelected = m_pimpl->activeDirectory.selectedChildID == ID;

	ImGui::PushID(fullName.c_str());

	Color4 borderColor = { 0,0,0,50 };
	Color4 tintColor = Color4::White;

	if (isSelected)
	{
		borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;
		tintColor = { 0.8f, 0.8f, 0.8f };
	}

	ImVec4 tint = { tintColor.r,tintColor.g,tintColor.b,tintColor.a };

	GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);

	GUI::PushColor(ImGuiCol_Border, borderColor);
	GUI::PushColor(ImGuiCol_BorderShadow, { 0.0f,0.0f,0.0f,0.0f });
	GUI::PushColor(ImGuiCol_Button, { 0.0f,0.0f,0.0f,0.0f });
	GUI::PushColor(ImGuiCol_ButtonHovered, { 0.0f,0.0f,0.0f,0.05f });
	GUI::PushColor(ImGuiCol_ButtonActive, { 0.0f,0.0f,0.0f,0.1f });

	bool clicked = ImGui::ImageButton(GUI::CastTextureID(file.thumbnailTexture), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0), 3, ImVec4(0.f, 0.f, 0.f, 0.1f), tint);
	GUI::PopColor(5);
	GUI::PopStyle();

	ImVec2 imageDimensions = ImGui::GetItemRectSize();

	if (ImGui::BeginDragDropSource())
	{
		if (m_pimpl->onDrag == false)
		{
			m_pimpl->onDrag = true;

			if (file.resource)
			{
				EditorContext::Instance().dragManager->BeginDrag(file.resource);
			}
		}

		ImGui::Text(name.c_str());
		ImGui::SetDragDropPayload("DND_Resources", nullptr, 0);
		ImGui::EndDragDropSource();
	}
	else
	{
		m_pimpl->onDrag = false;
	}

	bool focusRenameTextField = false;

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		if (Input::Mouse::GetButtonPreciseDoubleClicked(MouseButton::Left))
		{
			if (EditorContext::Instance().gameState == GameState::Sleep && 
				file.resource->getArchetype() == Scene::staticGetArchetype())
			{
				Scene* scene = static_cast<Scene*>(file.resource);
				SceneManager* sceneManager = EngineContext::Instance().sceneManager;

				if (sceneManager->GetCurrentScene() && sceneManager->GetCurrentScene() != scene)
				{
					EditorContext::Instance().taskQueue->AddTask([scene, sceneManager]() {
						sceneManager->SaveCurrentScene();
						EditorContext::Instance().selectionManager->ClearGameObjects();

						RenderSceneWidget& sceneWidget = EditorContext::Instance().widgetManager->GetSceneWidget();

						sceneWidget.SaveSceneCamera();
						sceneManager->ChangeCurrentScene(scene);
						sceneWidget.LoadSceneCamera();
						});
				}
			}
			else if (file.resource->getArchetype() == Script::staticGetArchetype())
			{
				bool isSoutionOpened = Window::IsWindowOpen("HydrillScripting - Microsoft Visual Studio");

				std::string solutionPath = std::string(BINARIES_ROOT_RAW) + R"(\..\HydrillScripting.sln)";

				if (!isSoutionOpened)
				{
					system(("start devenv " + solutionPath).c_str());
				}
				else
				{
					std::string headerPath = std::string(INTERNAL_ENGINE_SCRIPTING_ROOT_RAW) + R"(\include\)" + StringHelper::GetFileNameWithoutExtension(file.path.filename().string()) + ".hpp";
					std::string sourcePath = std::string(INTERNAL_ENGINE_SCRIPTING_ROOT_RAW) + R"(\src\)" + StringHelper::GetFileNameWithoutExtension(file.path.filename().string()) + ".cpp";
					system(("start devenv /Edit " + headerPath).c_str());
					system(("start devenv /Edit " + sourcePath).c_str());
				}
			}
		}

		if (Input::Mouse::GetButtonPreciseClicked(MouseButton::Left) || Input::Mouse::GetButtonPreciseClicked(MouseButton::Right))
		{
			if (!Input::Keyboard::GetKeyDown(Key::LeftControl)) EditorContext::Instance().selectionManager->ClearResources();

			if (file.resource) EditorContext::Instance().selectionManager->AddToSelection(file.resource);

			m_pimpl->activeDirectory.selectedChildID = ID;

			//	Keep the focus
			ImGui::SetWindowFocus();
		}
	}

	bool deleteConfirmation = false;
	if(isSelected)
		if (ImGui::IsWindowFocused())
	{
		if (Input::Keyboard::GetKeyPressed(Key::F2))
		{
			m_pimpl->renamingFile = &file;
			m_pimpl->renameBuffer = name;
			focusRenameTextField = true;
		}

		if (Input::Keyboard::GetKeyPressed(Key::Delete))
		{
			deleteConfirmation = true;
		}
	}
	//	CONTEXT MENU
	//	------------


	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool AssetContextMenu = ImGui::BeginPopupContextItem("###CONTENTBROWSER_FILE_CONTEXTMENU");
	GUI::PopStyle();

	if (AssetContextMenu)
	{
		m_pimpl->contextMenuOpened = true;

		if (ImGui::MenuItem("Copy"))
		{

		}

		if (ImGui::MenuItem("Delete"))
		{
			m_pimpl->consumeClick = true;
			deleteConfirmation = true;
		}

		ImGui::EndPopup();
	}

	if (deleteConfirmation)
	{
		ImGui::OpenPopup("###DeleteFile");
	}

	DeleteConfirm([&file]() {

		if (!file.resource)
		{
			fs::remove(file.path);
			return;
		}

		EditorSelectionManager* selec = EditorContext::Instance().selectionManager;
		ResourcesManager* resourceManager = EngineContext::Instance().resourcesManager;
		SceneManager* sceneManager = EngineContext::Instance().sceneManager;

		std::string path = file.path.string();

		if (selec->IsSelected(file.resource))
		{
			selec->ClearResources();
		}

		if (StringHelper::GetFileExtensionFromPath(path).compare(".script") == 0)
		{
			ScriptBuilder::DestructScript(StringHelper::GetFileNameFromPathWithoutExtension(StringHelper::GetFilePathWithoutExtension(path)));
		}

		if (StringHelper::GetFileExtensionFromPath(path).compare(".scene") == 0)
		{
			resourceManager->DeleteResource(file.resource);
		}
		else
		{
			sceneManager->SaveCurrentScene();
			sceneManager->ReloadCurrentScene();
		}

		},
		("Please confirm to delete \"" + file.path.filename().string() + "\".").c_str()
	);

	//	RENAMING
	//	--------

	//  Set special style for this text field
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

	ImVec2 savedPos = ImGui::GetCursorPos();

	Color4 buttonBG = Color4(0, 0, 0, 50);
	GUI::PushColor(ImGuiCol_Button, buttonBG);
	GUI::PushColor(ImGuiCol_ButtonHovered, buttonBG);
	GUI::PushColor(ImGuiCol_ButtonActive, buttonBG);
	if (ImGui::Button(name.c_str(), ImVec2(imageDimensions.x, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.f)))
	{
		m_pimpl->activeDirectory.selectedChildID = ID;
	}
	GUI::PopColor(3);

	//	Display Filename
	if (m_pimpl->renamingFile == &file)
	{
		ImGui::SetItemAllowOverlap();
		ImGui::SetCursorPos(savedPos);

		EditorTheme* theme = EditorContext::Instance().theme;

		ImGui::PushItemWidth(imageDimensions.x);

		//  Make char* buffer for ImGUi::InputText with a limited size of 64
		char buffer[64] = {};
		sprintf_s(buffer, "%s", m_pimpl->renameBuffer.c_str());

		if (focusRenameTextField)
		{
			ImGui::SetKeyboardFocusHere(0);
		}

		bool validate = ImGui::InputText("###FileRenameField", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		m_pimpl->renameBuffer = buffer;

		if (!isSelected)
		{
			validate = true;
		}

		if (validate)
		{
			file.resource->Rename(buffer);
			file.path = file.resource->GetFilepath();
			m_pimpl->renamingFile = nullptr;
			m_pimpl->renameBuffer = "";
		}
	}

	GUI::PopStyle();

	ImGui::PopID();
}


bool ContentBrowserWidget::DisplayDirectory(Directory& dir, const int ID)
{
	std::string str_ID = "###" + std::to_string(ID);
	std::string name = dir.path.filename().string();
	std::string fullName = name + str_ID;

	float thumbnailSize = EditorContext::Instance().editorDatas->contentBrowser.fileThumbnailSize;
	bool isActive = dir.path == m_pimpl->activeDirectory.path;
	bool  isSelected = m_pimpl->activeDirectory.selectedChildID == ID;

	ImGui::PushID(fullName.c_str());

	Color4 borderColor = { 0,0,0,50 };
	Color4 tintColor = { 180, 165, 100 };

	if (isSelected)
	{
		borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;
		tintColor = { 150, 135, 70 };
	}

	ImVec4 tint = { tintColor.r,tintColor.g,tintColor.b,tintColor.a };

	GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
	GUI::PushColor(ImGuiCol_Border, borderColor);

	GUI::PushColor(ImGuiCol_BorderShadow, { 0.0f,0.0f,0.0f,0.0f });
	GUI::PushColor(ImGuiCol_Button, { 0.0f,0.0f,0.0f,0.0f });
	GUI::PushColor(ImGuiCol_ButtonHovered, { 0.0f,0.0f,0.0f,0.05f });
	GUI::PushColor(ImGuiCol_ButtonActive, { 0.0f,0.0f,0.0f,0.1f });

	bool clicked = ImGui::ImageButton(GUI::CastTextureID(m_pimpl->folderIconTexture), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0), 3, ImVec4(0, 0, 0, 0), tint);

	GUI::PopColor(5);
	GUI::PopStyle();

	bool deleteConfirmation = false;

	ImVec2 imageDimensions = ImGui::GetItemRectSize();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		if (Input::Mouse::GetButtonPreciseDoubleClicked(MouseButton::Left))
		{
			EditorContext::Instance().taskQueue->AddTask([&, dir]() {
				ChangeDirectory(dir.path);
				});
		}
	
		if (Input::Mouse::GetButtonPreciseClicked(MouseButton::Left) || Input::Mouse::GetButtonPreciseClicked(MouseButton::Right))
		{
			m_pimpl->activeDirectory.selectedChildID = ID;
		}

		//	Drag and drop
		GameObject* dragged = EditorContext::Instance().dragManager->GetDraggedGameObject();

		if (dragged)
		{
			if (Input::Mouse::GetButtonUnpressed(MouseButton::Left))
			{
				Prefab& prefab = *EngineContext::Instance().resourcesManager->CreateResource<Prefab>();
				prefab.CreateFromGameObject(dragged, dragged->GetName(), m_pimpl->activeDirectory.path.string());

				LoadActiveDirectory();
				EditorContext::Instance().dragManager->EndDrag();
			}
		}
	}

	if (isSelected && ImGui::IsWindowFocused())
	{
		if (Input::Keyboard::GetKeyPressed(Key::Delete))
		{
			deleteConfirmation = true;
		}
	}
	//	CONTEXT MENU
	//	------------


	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool AssetContextMenu = ImGui::BeginPopupContextItem("###CONTENTBROWSER_DIRECTORY_CONTEXTMENU");
	GUI::PopStyle();

	if (AssetContextMenu)
	{
		m_pimpl->contextMenuOpened = true;

		if (ImGui::MenuItem("Copy"))
		{

		}

		if (ImGui::MenuItem("Delete"))
		{
			m_pimpl->consumeClick = true;
			deleteConfirmation = true;
		}

		ImGui::EndPopup();
	}

	if (deleteConfirmation)
	{
		ImGui::OpenPopup("###DeleteFile");
	}

	DeleteConfirm([&]()
		{
			UnloadDirectoryTree(m_pimpl->rootDir);
			DeleteDirectory(dir.path);
			LoadDirectoryTree(m_pimpl->rootDir);
		},
		("Please confirm to delete the folder \"" + dir.path.filename().string() + "\".\nThis will delete all files inside.").c_str()
	);

	//  Set special style for this text field

	ImVec2 savedPos = ImGui::GetCursorPos();

	Color4 buttonBG = Color4(0, 0, 0, 50);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	GUI::PushColor(ImGuiCol_Button, buttonBG);
	GUI::PushColor(ImGuiCol_ButtonHovered, buttonBG);
	GUI::PushColor(ImGuiCol_ButtonActive, buttonBG);
	if(ImGui::Button(name.c_str(), ImVec2(imageDimensions.x, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.f)))
	{
		m_pimpl->activeDirectory.selectedChildID = ID;
	}
	GUI::PopColor(3);
	GUI::PopStyle();

	ImGui::PopID();

	return true;
}


void ContentBrowserWidget::DeleteDirectory(const fs::path& dirPath)
{
	EditorSelectionManager* selec = EditorContext::Instance().selectionManager;
	ResourcesManager* resourceManager = EngineContext::Instance().resourcesManager;
	SceneManager* sceneManager = EngineContext::Instance().sceneManager;

	bool needSceneReload = false;

	std::queue<const fs::path*> trashCollector;

	for (const fs::directory_entry& dir_entry : fs::directory_iterator{ dirPath })
	{
		//	If it is a directory
		if (dir_entry.is_directory())
		{
			DeleteDirectory(dir_entry);
		}

		//	Else if it is a file
		else
		{
			std::string extension = dir_entry.path().extension().string();

			//	If it is a resource file
			if (std::find(m_pimpl->resourceExtensions.begin(), m_pimpl->resourceExtensions.end(), extension) != m_pimpl->resourceExtensions.end())
			{
				std::string path = dir_entry.path().string();

				Resource* resource = resourceManager->GetResourceByPath(path);
				if (resource)
				{
					if (selec->IsSelected(resource))
					{
						selec->ClearResources();
					}

					if (StringHelper::GetFileExtensionFromPath(path).compare(".script"))
					{
						ScriptBuilder::DestructScript(StringHelper::GetFileNameFromPathWithoutExtension(StringHelper::GetFilePathWithoutExtension(path)));
					}


					resourceManager->DeleteResource(resource);
					needSceneReload = true;
				}
			}
			else
			{
				fs::remove(dir_entry);
			}
		}
	}
	
	fs::remove(dirPath);

	if (needSceneReload)
	{
		sceneManager->ReloadCurrentScene();
	}
}

void ContentBrowserWidget::DeleteConfirm(const std::function<void()>& deleteFunction, const char* message)
{
	//	DELETE POPUP
	//	------------

	float  width = EditorContext::Instance().window->GetWidth() * 0.25f;
	float  height = EditorContext::Instance().window->GetHeight() * 0.15f;
	float  widthPadding = width * 0.1f;
	float  heightPadding = height * 0.1f;

	ImVec2 winPos = ImVec2(
		EditorContext::Instance().window->GetWidth() * 0.5f - width * 0.5f,
		EditorContext::Instance().window->GetHeight() * 0.5f - height * 0.5f
	);

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(ImVec2(width, height));

	if (ImGui::BeginPopupModal("###DeleteFile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
	{
		ImVec2 buttonDim = ImVec2(width * 0.25f, height * 0.2f);

		if (!m_pimpl->consumeClick && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && Input::Mouse::WasClicked())
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(width * 0.5f - ImGui::CalcTextSize(message).x * 0.5f);
		ImGui::Text(message);

		ImGui::SetCursorPosX(widthPadding);
		ImGui::SetNextItemWidth(width - 2.f * widthPadding);

		ImGui::SetCursorPosY(height - heightPadding - buttonDim.y);
		ImGui::SetCursorPosX(widthPadding);

		GUI::PushStyle(ImGuiStyleVar_FrameRounding, 1.f);

		GUI::PushColor(ImGuiCol_Button, Color4::DarkGreen);
		GUI::PushColor(ImGuiCol_ButtonHovered, Color4::DarkGreen * 0.75f);
		GUI::PushColor(ImGuiCol_ButtonActive, Color4::DarkGreen * 0.50f);

		if (ImGui::Button("Confirm", buttonDim))
		{
			deleteFunction();
			ImGui::CloseCurrentPopup();

			m_pimpl->refreshFolder = true;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(width - widthPadding - width * 0.25f);

		GUI::PopColor(3);

		GUI::PushColor(ImGuiCol_Button, Color4::DarkRed);
		GUI::PushColor(ImGuiCol_ButtonHovered, Color4::DarkRed * 0.75f);
		GUI::PushColor(ImGuiCol_ButtonActive, Color4::DarkRed * 0.50f);

		if (ImGui::Button("Cancel", buttonDim))
		{
			ImGui::CloseCurrentPopup();
		}

		GUI::PopColor(3);
		GUI::PopStyle();

		ImGui::EndPopup();
	}
}
