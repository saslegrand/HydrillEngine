#include "Widgets/ResourceInspectorWidget.hpp"

#include <unordered_map>
#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Renderer/RenderSystem.hpp> 
#include <Resources/Resource/Material.hpp> 
#include <Resources/Resource/Mesh.hpp> 
#include <Resources/Resource/Model.hpp> 
#include <Resources/Resource/Prefab.hpp> 
#include <Resources/Resource/Scene.hpp> 
#include <Resources/Resource/SkeletalMesh.hpp> 
#include <Resources/Resource/Texture.hpp> 
#include <Resources/Resource/Skybox.hpp> 
#include <Resources/Resource/Terrain.hpp> 
#include <Resources/Loaders/ResourcesLoader.hpp> 
#include <Renderer/Primitives/GLTexture.hpp> 

#include <Refureku/Refureku.h>
#include <ECS/GameObject.hpp> 
#include <ECS/Behavior.hpp> 
#include <ECS/Systems/MeshSystem.hpp> 
#include <IO/Keyboard.hpp> 
#include <IO/Mouse.hpp> 
#include <Tools/StringHelper.hpp> 
#include <Types/Color.hpp> 
#include <EngineContext.hpp> 

#include "Tools/InspectorUtils.hpp"
#include "GUITypes.hpp"
#include "GUI.hpp"
#include "EditorResourcePreview.hpp"
#include "EditorSelectionManager.hpp"
#include "EditorContext.hpp"


ResourceInspectorWidget::ResourceInspectorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: EditorWidget(widgetName, widgetID, isActive, "###RESOURCE_INSPECTOR")
{
	m_flag |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar;

	EditorContext::Instance().selectionManager->onResourceUpdated.AddCallback(&ResourceInspectorWidget::ResourceSelectionUpdate, *this);

	m_openedCadenasButtonTexture = EditorContext::Instance().images->LoadImage(R"(Icons\CadenasOpen.texture)");
	m_closedCadenasButtonTexture = EditorContext::Instance().images->LoadImage(R"(Icons\CadenasClose.texture)");
}


void ResourceInspectorWidget::ResourceSelectionUpdate(Resource* resource)
{
	if (m_lockCurrentResource) return;

	if (resource == nullptr)
	{
		currentResource = nullptr;
	}
	else if (!EditorContext::Instance().selectionManager->GetResources().empty())
	{
		ImGui::SetWindowFocus(m_nameID.c_str());
		currentResource = resource;
	}
}

void ResourceInspectorWidget::DisplayMaterial(Material* material)
{
	float width = ImGui::GetContentRegionAvail().x;
	ImVec2 dimensions = ImVec2(width * 0.75f, width * 0.75f);

	ImGui::SetCursorPosX(width * .5f - dimensions.x * 0.5f);

	Color4 borderColor = EditorContext::Instance().theme->color_borders.activeColor;
	ImVec4 border = { borderColor.r,borderColor.g,borderColor.b,borderColor.a };

	unsigned int textureID = 0;
	if (material) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(*material);

	ImGui::Image(GUI::CastTextureID(textureID), dimensions, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, border);

	if (InspectorMethods::DisplayEntity(material, &material->getArchetype()) && material)
	{
		material->Serialize();

		EditorContext::Instance().resourcePreview->GenerateResourcePreview(*material);
		SystemManager::GetMeshSystem().UpdateMeshesWithMaterial(material);
	}
}


void ResourceInspectorWidget::DisplayMesh(Mesh* mesh)
{
	float width = ImGui::GetContentRegionAvail().x;
	ImVec2 dimensions = ImVec2(width * 0.75f, width * 0.75f);

	ImGui::SetCursorPosX(width * .5f - dimensions.x * 0.5f);

	Color4 borderColor = EditorContext::Instance().theme->color_borders.activeColor;
	ImVec4 border = { borderColor.r,borderColor.g,borderColor.b,borderColor.a };

	unsigned int textureID = 0;
	if (mesh) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(*mesh);

	ImGui::Image(GUI::CastTextureID(textureID), dimensions, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, border);

	InspectorMethods::DisplayEntity(mesh, &mesh->getArchetype());
}


void ResourceInspectorWidget::DisplayModel(Model* model)
{
	float width = ImGui::GetContentRegionAvail().x;
	ImVec2 dimensions = ImVec2(width * 0.75f, width * 0.75f);

	ImGui::SetCursorPosX(width * .5f - dimensions.x * 0.5f);

	Color4 borderColor = EditorContext::Instance().theme->color_borders.activeColor;
	ImVec4 border = { borderColor.r,borderColor.g,borderColor.b,borderColor.a };

	unsigned int textureID = 0;
	if (model) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(*model);

	ImGui::Image(GUI::CastTextureID(textureID), dimensions, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, border);

	InspectorMethods::DisplayEntity(model, &model->getArchetype());
}


void ResourceInspectorWidget::DisplayPrefab(Prefab* prefab)
{
	InspectorMethods::DisplayEntity(prefab, &prefab->getArchetype());
}


void ResourceInspectorWidget::DisplayScene(Scene* scene)
{
	InspectorMethods::DisplayEntity(scene, &scene->getArchetype());
}


void ResourceInspectorWidget::DisplaySkeletalMesh(SkeletalMesh* skMesh)
{
	float width = ImGui::GetContentRegionAvail().x;
	ImVec2 dimensions = ImVec2(width * 0.75f, width * 0.75f);

	ImGui::SetCursorPosX(width * .5f - dimensions.x * 0.5f);

	Color4 borderColor = EditorContext::Instance().theme->color_borders.activeColor;
	ImVec4 border = { borderColor.r,borderColor.g,borderColor.b,borderColor.a };

	unsigned int textureID = 0;
	if (skMesh) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(*skMesh);

	ImGui::Image(GUI::CastTextureID(textureID), dimensions, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, border);

	InspectorMethods::DisplayEntity(skMesh, &skMesh->getArchetype());
}

void ResourceInspectorWidget::DisplaySkybox(Skybox* skybox)
{
	InspectorMethods::DisplayEntity(skybox, &skybox->getArchetype());

	RenderSystem& renderSystem = SystemManager::GetRenderSystem();

	if (ImGui::Button("Reimport"))
		skybox->Reload();

	if (ImGui::Button("Attach"))
		renderSystem.AttachNewSkybox(skybox);
	if (ImGui::Button("Detach"))
	{
		if (renderSystem.GetAttachedSkybox() == skybox)
			renderSystem.AttachNewSkybox(nullptr);
	}
}

void ResourceInspectorWidget::DisplayTerrain(Terrain* terrain)
{
	InspectorMethods::DisplayEntity(terrain, &terrain->getArchetype());

	if (ImGui::Button("Reimport"))
	{
		terrain->Reload(true);
		terrain->Serialize();
	}
}

void ResourceInspectorWidget::DisplayTexture(Texture* texture)
{
	float width = ImGui::GetContentRegionAvail().x;
	ImVec2 dimensions = ImVec2(width * 0.75f, width * 0.75f);

	ImGui::SetCursorPosX(width * .5f - dimensions.x * 0.5f);

	Color4 borderColor = EditorContext::Instance().theme->color_borders.activeColor;
	ImVec4 border = { borderColor.r,borderColor.g,borderColor.b,borderColor.a };

	ImGui::Image(GUI::CastTextureID(texture->GPUData->generatedTexture->GetID()), dimensions, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, border);

	std::string text = "ID     : " + std::to_string(texture->GPUData->generatedTexture->GetID());
	ImGui::Text(text.c_str());
	text = "Width  : " + std::to_string(texture->data.width);
	ImGui::Text(text.c_str());
	text = "Height : " + std::to_string(texture->data.height);
	ImGui::Text(text.c_str());
	text = "Type   : " + std::to_string(texture->data.type);
	ImGui::Text(text.c_str());

	if (InspectorMethods::DisplayEntity(texture, &texture->getArchetype()))
		texture->Serialize();


	if (ImGui::Button("Reimport"))
	{
		texture->UnloadFromGPUMemory();
		ResourcesLoader::LoadTextureUnsafe(*texture);
		texture->LoadInGPUMemory();

		// TODO Update texture in content browser
		EditorContext::Instance().resourcePreview->GenerateResourcePreview(*texture);
	}
}


void ResourceInspectorWidget::Display()
{
	if (!isActive) return;

	if (BeginWidget())
	{
		if (currentResource)
		{
			// First draw global elements (Name, Path, Type)

			const char* name = currentResource->GetFilename().c_str();

			float width = ImGui::GetContentRegionAvail().x;

			EditorContext::Instance().theme->defaultFont->UseBold();

			ImGui::AlignTextToFramePadding();
			ImVec2 texDimension = ImGui::CalcTextSize(name);
			ImGui::SetCursorPosX(width * 0.5f - ImGui::CalcTextSize(name).x * 0.5f);
			ImGui::Text(name);

			EditorContext::Instance().theme->defaultFont->EndFontStyle();

			ImGui::SameLine();

			ImVec2 iconDimension = { 20.f ,20.f };

			ImGui::SetCursorPosX(width - 10.f - texDimension.y * 1.25f);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y - (iconDimension.y - ImGui::GetFont()->FontSize) * 0.5f);

			unsigned int tex = m_lockCurrentResource ? m_closedCadenasButtonTexture : m_openedCadenasButtonTexture;

			GUI::PushColor(ImGuiCol_Button, { 0.0f,0.0f,0.0f,0.0f });
			GUI::PushColor(ImGuiCol_ButtonHovered, { 0.0f,0.0f,0.0f,0.05f });
			GUI::PushColor(ImGuiCol_ButtonActive, { 0.0f,0.0f,0.0f,0.1f });
			bool clicked = ImGui::ImageButton(GUI::CastTextureID(tex), iconDimension, ImVec2(0, 1), ImVec2(1, 0), -1);
			GUI::PopColor(3);

			if (clicked)
			{
				m_lockCurrentResource = !m_lockCurrentResource;


				if (!m_lockCurrentResource && !EditorContext::Instance().selectionManager->GetResources().empty())
				{
					currentResource = EditorContext::Instance().selectionManager->GetResources()[0];
				}
			}



			// Second draw from type
			// Third draw field

			if (currentResource->getArchetype() == Material::staticGetArchetype())		    DisplayMaterial(static_cast<Material*>(currentResource));
			else if (currentResource->getArchetype() == Mesh::staticGetArchetype())			DisplayMesh(static_cast<Mesh*>(currentResource));
			else if (currentResource->getArchetype() == Model::staticGetArchetype())		DisplayModel(static_cast<Model*>(currentResource));
			else if (currentResource->getArchetype() == Prefab::staticGetArchetype())		DisplayPrefab(static_cast<Prefab*>(currentResource));
			else if (currentResource->getArchetype() == Scene::staticGetArchetype())		DisplayScene(static_cast<Scene*>(currentResource));
			else if (currentResource->getArchetype() == SkeletalMesh::staticGetArchetype())	DisplaySkeletalMesh(static_cast<SkeletalMesh*>(currentResource));
			else if (currentResource->getArchetype() == Texture::staticGetArchetype())		DisplayTexture(static_cast<Texture*>(currentResource));
			else if (currentResource->getArchetype() == Skybox::staticGetArchetype())		DisplaySkybox(static_cast<Skybox*>(currentResource));
			else if (currentResource->getArchetype() == Terrain::staticGetArchetype())		DisplayTerrain(static_cast<Terrain*>(currentResource));
		}
	}
	EndWidget();
}

