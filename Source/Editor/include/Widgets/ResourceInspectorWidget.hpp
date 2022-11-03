#pragma once

#include <Maths/Vector3.hpp>
#include <vector>

#include <Tools/Event.hpp>

#include "Widgets/EditorWidget.hpp"

class Resource;
class Material;
class Mesh;
class Model;
class Prefab;
class Scene;
class SkeletalMesh;
class Texture;
class Skybox;
class Terrain;


/**
* @brief This widget enable the user to tweak gameobjetcs from the scene
*/
class ResourceInspectorWidget : public EditorWidget
{

//	Variables

private:

	Resource* currentResource = nullptr;

	bool m_lockCurrentResource = false;

	unsigned int m_openedCadenasButtonTexture = 0;
	unsigned int m_closedCadenasButtonTexture = 0;

//	Constructor(s) & Destructor(s)

public:

	ResourceInspectorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);
	ResourceInspectorWidget() = default;

	~ResourceInspectorWidget() {}

//	Functions

private:

	/**
	@brief This function is called when selection manager has some changes with selected resources

	@param resource : newly selected resource, or is null if all selected objects were unselected
	*/
	void ResourceSelectionUpdate(Resource* resource);

	/**
	@brief Display the given material in the inspector

	@param material : material to display
	*/
	void DisplayMaterial(Material* material);

	/**
	@brief Display the given mesh in the inspector

	@param mesh : mesh to display
	*/
	void DisplayMesh(Mesh* mesh);

	/**
	@brief Display the given model in the inspector

	@param model : model to display
	*/
	void DisplayModel(Model* model);

	/**
	@brief Display the given prefab in the inspector

	@param prefab : prefab to display
	*/
	void DisplayPrefab(Prefab* prefab);

	/**
	@brief Display the given scene in the inspector

	@param scene : scene to display
	*/
	void DisplayScene(Scene* scene);

	/**
	@brief Display the given skeletal mesh in the inspector

	@param skMesh : skeletal mesh to display
	*/
	void DisplaySkeletalMesh(SkeletalMesh* skMesh);

	/**
	@brief Display the given texture in the inspector

	@param texture : texture to display
	*/
	void DisplayTexture(Texture* texture);

	/**
	@brief Display the given skybox in the inspector

	@param skybox : skybox to display
	*/
	void DisplaySkybox(Skybox* skybox);

	/**
	@brief Display the given terrain in the inspector

	@param terrain : terrain to display
	*/
	void DisplayTerrain(Terrain* terrain);

public:


	/**
	@brief Widget display function
	*/
	void Display() override;

};