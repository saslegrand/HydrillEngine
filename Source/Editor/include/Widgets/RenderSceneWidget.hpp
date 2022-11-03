#pragma once

#include <memory>

#include <nlohmann/json_fwd.hpp>
#include <Maths/Vector3.hpp>
#include <Types/Serializable.hpp>
#include <Renderer/RenderObjects/Camera.hpp>
#include <Renderer/RenderpassParameters.hpp>

#include "Widgets/RenderWidget.hpp"
#include "EditorRenderer/RenderPicking.hpp"
#include "EditorRenderer/RenderGizmo.hpp"

class Scene;

constexpr float Camera_Minimum_Speed = 1.f;
constexpr float Camera_Maximum_Speed = 50.f;

/**
@brief This is a render widget which will display the scene editor
*/
class RenderSceneWidget : public RenderWidget, public Serializable
{

//	Variables

private:

	FrameBuffer m_cameraPreviewFrameBuffer;

	Camera m_cameraEditor;

	Vector3 m_cameraPosition = { 0.f,0.f,5.f };
	Vector3 m_cameraRotation = { 0.f,0.f,0.f };
	Vector2 m_renderFieldPosition;
	Vector2 m_renderFieldLocalPosition;
	Vector2 m_renderFieldDimension;

	float m_cameraSpeed = 5.f;
	float m_grabCameraSpeed = 100.f;

	Vector2 m_cameraPreviewDimensions;
	Vector2 m_cameraPreviewWindowDimensions;
	Vector2 m_cameraTargetPreviewWindowDimensions;

	float m_cameraSensivity = 75.f;
	bool  m_grabbing = false;

	int    m_gizmoMode = 1;
	int    m_refMode = 0;
	float  m_snapValue[3] = { 0.25f, 5.0f, 0.0f };
	float* m_deltaMatrix = nullptr;

	RenderpassParameters m_cameraPreviewRenderParam;
	RenderpassParameters m_sceneRenderParam;

	RenderPicking m_renderPicking;
	RenderGizmo m_renderGizmo;

	bool m_enablePicking;

	float m_elapsedTimeCameraPreview = 0.0f;
	bool  m_displayCameraPreview = false;

	std::string m_scenesSavePath;

//	Constructor(s) & Destructor(s)

public:

	RenderSceneWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);
	virtual ~RenderSceneWidget();

//	Functions

private:

	/**
	@brief Update local camera
	*/
	void UpdateCamera();

	/**
	@brief Selected object's Gizmo
	*/
	void Gizmo();

public:
	/**
	@brief Create a camera save for the current scene (set to default)

	@param scene : scene to which the camera is bound to
	*/
	void CreateSaveSceneCamera(Scene& scene);

	/**
	@brief Save the current scene camera
	*/
	void SaveSceneCamera();

	/**
	@brief Load the current scene camera
	*/
	void LoadSceneCamera();

	/**
	@brief Save the current editor camera

	@param jsonFile : file to save into
	*/
	void SaveEditorCamera(nlohmann::json& jsonFile);

	/**
	@brief Load the current editor camera

	@param jsonFile : file to load from
	*/
	void LoadEditorCamera(nlohmann::json& jsonFile);

	/**
	@brief Only called if the widget is active and then call Update framebuffer
	*/
	void Update() override;

	/**
	@brief Widget display function
	*/
	void Display() override;	

	/**
	@brief Update the framebuffer and resize it if the scene window was resized then render the scene inside
	*/
	void RenderFramebuffer() override;

	/**
	@brief Parameter bar of the Scene window
	*/
	void ParameterBar() override;
};
