#pragma once

#include <unordered_map>

class Resource;

class EditorImages
{
//	Variables

private:

	std::unordered_map<const char*, unsigned int> generatedImages;

	//	TODO : Move in other class (EditorPreviewManager)
	//std::unordered_map<Resource*, unsigned int> resourcePreview;

//	Constructors & Destructors

public:

	EditorImages();
	~EditorImages();

//	Functions

public:

	/**
	@brief Load an image from a path using the resources manager then generate it with openGL

	@param path : complete path to the image

	@return unsigned int : openGL texture ID
	*/
	unsigned int LoadImage(const char* path);

	/**
	@brief Unload an image from a path by destroying its openGL ID

	@param path : complete path to the image
	*/
	void UnloadImage(const char* path);

	/**
	@brief Get an openGL texture ID given a path

	@param path : complete path of the image
	*/
	unsigned int GetImageFromPath(const char* path);


	//	TODO : Move in other class (EditorPreviewManager)
	//unsigned int GenerateResourcePreview(Resource* resource);
	//unsigned int GetResourcePreview(Resource* resource);
};