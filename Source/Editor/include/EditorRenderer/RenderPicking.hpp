#pragma once

#include <Renderer/RenderPipeline.hpp>
#include <Renderer/MeshInstance.hpp>
#include <Renderer/Primitives/ShaderProgram.hpp>
#include <Renderer/Primitives/VertexArray.hpp>
#include <Maths/Vector2.hpp>
#include <Types/Color.hpp>

class GameObject;
class Component;

class RenderPicking : public RenderPipeline
{

//	Variables

private:

	ShaderProgram m_meshShader;
	ShaderProgram m_skeletalMeshShader;
	ShaderProgram m_gizmosShader;

	const GameObject* m_pressedObject = nullptr;
	
	bool m_wasPressed = false;

	unsigned int m_FBO;
	unsigned int m_RBO;
	unsigned int m_pickingTexture;

	VertexArray m_rectVAO;

	int m_gizmoOffsetID = 0;
	int m_skeletalOffsetID = 0;

	Vector2 m_position;
	Vector2 m_dimensions;

	std::vector<const GameObject*> m_gizmoGameObjects;


public:

	GameObject* pickedObject = nullptr;
	bool updated = false;
	Color4 selectedColor = Color4::White;

//	Functions

private:

	/**
	@brief Draw a mesh given a mesh instance. Shader and VAO must be bound before.

	@param instance : mesh instance to render
	*/
	void DrawMesh(const MeshInstance* instance);

	/**
	@brief Draw a skeletal mesh given a skeletal mesh instance. Shader and VAO must be bound before.

	@param instance : skeletal mesh instance to render
	*/
	void DrawSkeletalMesh(const SkeletalMeshInstance* instance);

	/**
	@brief Generate Gizmo objects (game objects which has components that can be showed as gizmos like, lights, cameras..)
	*/
	void GenerateGizmoObjects();


public :

	void Initialize() override;
	void Render() override;

	void RenderIDs();

	void UpdateDimensions(Vector2 newDimensions);
	void UpdatePosition(Vector2 newPosition);
};
