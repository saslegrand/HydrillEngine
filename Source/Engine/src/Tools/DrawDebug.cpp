#include "Tools/DrawDebug.hpp"
#include "EngineContext.hpp"
#include "Renderer/RenderSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"


void DrawDebug::DrawLine(const Vector3& startWorldPosition, const Vector3& endWorldPosition, float width, Color3 color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddLine(startWorldPosition, endWorldPosition, width, color);
}

void DrawDebug::DrawCircle(const Matrix4& model, float radius, unsigned int pointCount, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddCircle(model, radius, pointCount, width, color);
}

void DrawDebug::DrawSphere(const Matrix4& model, float radius, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddSphere(model, radius, width, color);
}

void DrawDebug::DrawCone(const Matrix4& model, float baseRadius, float length, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddCone(model, baseRadius, length, width, color);
}

void DrawDebug::DrawCylinder(const Matrix4& model, float radius, float height, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddCylinder(model, radius, height, width, color);
}

void DrawDebug::DrawCapsule(const Matrix4& model, float radius, float height, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddCapsule(model, radius, height, width, color);
}

void DrawDebug::DrawBox(const Matrix4& model, float boxWidth, float boxHeight, float boxLength, float width, const Color3& color)
{
	SystemManager::GetRenderSystem().debugRenderPipeline.AddBox(model, boxWidth, boxHeight, boxLength, width, color);
}
