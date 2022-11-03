#pragma once

#include "Maths/Vector3.hpp"
#include "Maths/Matrix4.hpp"
#include "Types/Color.hpp"
#include "EngineDLL.hpp"

/**
@brief Debug draw functions
*/
namespace DrawDebug
{
	/*
	@brief Draw a simple line from a point to another

	@param startWorldPosition : Start position in world space
	@param endWorldPosition : End position in world space
	@param width : Line width
	@param color : Line color
	*/
	ENGINE_API void DrawLine(const Vector3& startWorldPosition, const Vector3& endWorldPosition, float width = 1.f, Color3 color = Color3::Red);


	/*
	@brief Draw a circle

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the circle
	@param pointCount : Numbre of point which form the circle
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawCircle(const Matrix4& model, float radius = 0.5f, unsigned int pointCount = 12, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Draw a wireframe sphere

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the sphere
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawSphere(const Matrix4& model, float radius = 0.5f, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Draw a wireframe cone (FROM TOP)

	@param model : Transform matrix of the primitive to draw
	@param baseRadius : Base radius of the circle
	@param length : Cone length
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawCone(const Matrix4& model, float baseRadius, float length, float width, const Color3& color);

	/*
	@brief Draw a cylinder

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the cylinder
	@param height : Cylinder height
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawCylinder(const Matrix4& model, float radius = 0.5f, float height = 1.f, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Draw a capsule

	@param model : Transform matrix of the primitive to draw
	@param radius : Radius of the capsule
	@param height : Capsule height
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawCapsule(const Matrix4& model, float radius = 0.5f, float height = 1.f, float width = 1.f, const Color3& color = Color3::Red);

	/*
	@brief Draw a Box

	@param model : Transform matrix of the primitive to draw
	@param boxWidth : Box width
	@param boxHeight : Box height
	@param boxLength : Box length
	@param width : Wireframe line width
	@param color : Line color
	*/
	ENGINE_API void DrawBox(const Matrix4& model, float boxWidth = 1.f, float boxHeight = 1.f, float boxLength = 1.f, float width = 1.f, const Color3& color = Color3::Red);
}