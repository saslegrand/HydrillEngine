#pragma once

#include <unordered_map>

#include "EditorRenderer/RenderPreview.hpp"

class Resource;

class EditorResourcePreview
{
//	Variables

private:

	RenderPreview m_renderPreview;

	std::unordered_map<const Resource*, unsigned int> m_resourcePreview;

//	Constructors & Destructors

public:

	EditorResourcePreview();
	~EditorResourcePreview();

//	Functions

public:

	/**
	@brief Initialize the preview renderer
	*/
	void Initialize();

	/**
	@brief Update all previews that need to be updated
	*/
	void Update();

	/**
	@brief Generate and return the texture ID of the given resource preview

	@ param resource : the resource that we need to generate and retrieve its preview image

	@return Preview texture ID
	*/
	unsigned int GenerateResourcePreview(Resource& resource);

	/**
	@brief Return the texture ID of the given resource preview

	@ param resource : the resource that we need to retrieve its preview image

	@return Preview texture ID
	*/
	unsigned int GetResourcePreview(Resource& resource);
};