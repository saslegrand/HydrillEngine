#pragma once
#include <deque>

#include "Types/Color.hpp"
#include "Maths/Vector3.hpp"
#include "Maths/Matrix4.hpp"

#include "Renderer/RenderPipeline.hpp"
#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/VertexBuffer.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"


/**
@brief Mesh render pipeline, manage the rendering of meshes
*/
class DebugRenderPipeline : public RenderPipeline
{

private:

	struct PrimitiveInstance
	{
		PrimitiveInstance(const float* vertices, unsigned int verticeSize, const std::vector<unsigned int>& indices, float width = 1.f, const Color3& color = Color3::Red);
		PrimitiveInstance(const float* vertices, unsigned int verticeSize, const std::vector<unsigned int>& indices,const Matrix4& model, float width = 1.f, const Color3& color = Color3::Red);

		Matrix4		 globaleModel = Matrix4::Identity;
		VertexArray	 VAO;
		VertexBuffer VBO;

		Color3 wireframecolor = Color3::Red;
		float  wireframeWidth = 1.f;

		std::vector<unsigned int> indices;

		unsigned int  size = 0;
	};

//	Variables

private:

	ShaderProgram m_shader;
	
	std::vector<PrimitiveInstance> m_instances;

public:


//	Constructors & Destructors

public:

	DebugRenderPipeline();
	~DebugRenderPipeline();

//	Functions

private:


public:

	/**
	@brief Initialize shader
	*/
	void Initialize() override;

	/**
	* @brief Render all debug primitives which were added then automatically clear the primitives
	*/
	void Render() override;

	/**
	@brief Clear all the primitives instance
	*/
	void ClearPrimitives();


	/*
	@brief Add a line in the instance list to be drawed in this frame,
	will draw a simple line from a point to another

	@param startWorldPosition : Start position in world space
	@param endWorldPosition : End position in world space
	@param width : Line width
	@param color : Line color
	*/
	void AddLine(const Vector3& startWorldPosition, const Vector3& endWorldPosition, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Add a line in the instance list to be drawed in this frame
	will locally draw a simple line from a point to another

	@param model : Transform matrix to draw a local line
	@param startWorldPosition : Start position in world space
	@param endWorldPosition : End position in world space
	@param width : Line width
	@param color : Line color
	*/
	void AddLine(const Matrix4& model, const Vector3& startLocalPosition, const Vector3& endLocalPosition, float width = 1.f, const Color3& color = Color3::Red);

	//	Primitives Shapes

	/*
	@brief Add a circle in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the circle
	@param pointCount : Numbre of point which form the circle
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddCircle(const Matrix4& model, float radius = 0.5f, unsigned int pointCount = 12, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Add a sphere in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the sphere
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddSphere(const Matrix4& model, float radius = 0.5f, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief  Add a cone in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param baseRadius : Base radius of the circle
	@param length : Cone length
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddCone(const Matrix4& model, float baseRadius, float length, float width, const Color3& color);

	/*
	@brief Add a cylinder in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the cylinder
	@param height : Cylinder height
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddCylinder(const Matrix4& model, float radius = 0.5f, float height = 1.f,float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Add a capsule in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the capsule
	@param height : Capsule height
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddCapsule(const Matrix4& model, float radius = 0.5f, float height = 1.f, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Add a box in the instance list to be drawed in this frame

	@param model : Transform matrix of the primitive to draw
	@param boxWidth : Box width
	@param boxHeight : Box height
	@param boxLength : Box length
	@param width : Wireframe line width
	@param color : Line color
	*/
	void AddBox(const Matrix4& model, float boxWidth = 1.f, float boxHeight = 1.f, float boxLength = 1.f, float width = 1.f, const Color3& color = Color3::Red);
};