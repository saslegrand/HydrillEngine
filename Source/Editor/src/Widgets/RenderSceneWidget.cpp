#include "Widgets/RenderSceneWidget.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <glad/gl.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#include <EngineContext.hpp>
#include <Renderer/RenderSystem.hpp>
#include <Physics/PhysicsSystem.hpp>
#include <Resources/Resource/Scene.hpp>
#include <Resources/SceneManager.hpp>
#include <IO/Keyboard.hpp>
#include <IO/Mouse.hpp>
#include <IO/Window.hpp>
#include <Core/Time.hpp>
#include <ECS/GameObject.hpp>
#include <ECS/TransformData.hpp>
#include <Core/Time.hpp>
#include <Tools/SerializationUtils.hpp>
#include <Tools/Curves.hpp>

#include "EditorContext.hpp"
#include "EditorSelectionManager.hpp"
#include "EditorTheme.hpp"
#include "PathConfig.hpp"
#include "GUI.hpp"

constexpr char Scene_Save_File[] = R"(Saves\\ScenesCamera.settings)";

RenderSceneWidget::RenderSceneWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: RenderWidget(widgetName, widgetID, isActive, "###SCENE")
{
	m_cameraEditor = Camera(60.0f, 0.1f, 500.f, 1920.f / 1080.f, Vector3(0.f,0.f,5.f));
	m_gizmoMode = ImGuizmo::TRANSLATE;
	m_refMode   = ImGuizmo::LOCAL;

	m_renderPicking.Initialize();
	m_renderGizmo.Initialize();

	ImGuizmo::AllowAxisFlip(false);

	m_scenesSavePath = INTERNAL_EDITOR_RESOURCES_ROOT + std::string(Scene_Save_File);

	m_sceneRenderParam.Initialize();
	m_sceneRenderParam.EnablePostProcess();
	
	m_sceneRenderParam.BindCamera(&m_cameraEditor);

	m_cameraPreviewRenderParam.Initialize();
	m_cameraPreviewRenderParam.EnablePostProcess();
}


RenderSceneWidget::~RenderSceneWidget()
{
}

void RenderSceneWidget::UpdateCamera()
{
	float dt = EngineContext::Instance().time->GetUnscaledDeltaTime();

	float accel = Input::Mouse::GetDeltaScroll().y * dt;
	m_cameraSpeed = Maths::Clamp(m_cameraSpeed + m_cameraSpeed * accel, Camera_Minimum_Speed, Camera_Maximum_Speed);

	float ForwardVelocity = 0.f;
	float StrafeVelocity  = 0.f;
	float UpVelocity	  = 0.f;

	if (Input::Keyboard::GetKeyDown(Key::S)) ForwardVelocity -= m_cameraSpeed;
	if (Input::Keyboard::GetKeyDown(Key::W)) ForwardVelocity += m_cameraSpeed;

	if (Input::Keyboard::GetKeyDown(Key::A)) StrafeVelocity -= m_cameraSpeed;
	if (Input::Keyboard::GetKeyDown(Key::D)) StrafeVelocity += m_cameraSpeed;

	if (Input::Keyboard::GetKeyDown(Key::LeftShift)) UpVelocity -= m_cameraSpeed;
	if (Input::Keyboard::GetKeyDown(Key::Space))	 UpVelocity += m_cameraSpeed;


	Vector3 movement;
	movement.x += Maths::Sin(-m_cameraRotation.y * Maths::DEGTORAD) * ForwardVelocity + Maths::Cos(-m_cameraRotation.y * Maths::DEGTORAD) * StrafeVelocity;
	movement.z -= Maths::Cos(-m_cameraRotation.y * Maths::DEGTORAD) * ForwardVelocity - Maths::Sin(-m_cameraRotation.y * Maths::DEGTORAD) * StrafeVelocity;
	movement.y += UpVelocity;
	
	if (m_grabbing)
	{
		movement += TransformData::Up(m_cameraEditor.GetTransformMatrix()) * Input::Mouse::GetDeltaPosition().y * m_cameraSpeed * m_grabCameraSpeed;
		movement -= TransformData::Right(m_cameraEditor.GetTransformMatrix()) * Input::Mouse::GetDeltaPosition().x * m_cameraSpeed * m_grabCameraSpeed;
	}

	m_cameraPosition += movement * dt * 10.f;

	if (!m_grabbing)
	{
		Vector3 rotation;
		rotation.y -= Input::Mouse::GetDeltaPosition().x * m_cameraSensivity;
		rotation.x -= Input::Mouse::GetDeltaPosition().y * m_cameraSensivity;

		m_cameraRotation += rotation * dt * 100.f;
		m_cameraRotation.x = Maths::Clamp(m_cameraRotation.x, -90.f, 90.f);

		Input::Mouse::ResetPosition();
	}

	m_cameraEditor.SetTransform(m_cameraPosition, Quaternion::FromEulerRad(m_cameraRotation * Maths::DEGTORAD));
	Input::Mouse::ResetDeltaPosition();

}


void RenderSceneWidget::Update()
{
	if (!isActive)
	{
		m_firstFrameFocus = false;
		return;
	}

	RenderWidget::Update();


	m_grabbing = Input::Keyboard::GetKeyDown(Key::LeftControl) || Input::Keyboard::GetKeyDown(Key::RightControl);

	if (m_captured)
	{
		if (m_firstFrameFocus)
		{
			Input::Mouse::ResetDeltaPosition();
			Input::Mouse::ResetPosition();
		}

		if (m_grabbing)
		{
			if (Input::Mouse::GetMode() == MouseMode::Disabled) Input::Mouse::SetMode(MouseMode::Normal);

			float left = m_renderFieldPosition.x;
			float right = m_renderFieldPosition.x + m_renderFieldDimension.x;
			float top = m_renderFieldPosition.y;
			float bottom = m_renderFieldPosition.y + m_renderFieldDimension.y;

			if (Input::Mouse::GetPosition().x < left) Input::Mouse::SetPositionX(right);
			if (Input::Mouse::GetPosition().x > right) Input::Mouse::SetPositionX(left);
			if (Input::Mouse::GetPosition().y < top) Input::Mouse::SetPositionY(bottom);
			if (Input::Mouse::GetPosition().y > bottom) Input::Mouse::SetPositionY(top);
		}
		else if (!m_grabbing && Input::Mouse::GetMode() == MouseMode::Normal) Input::Mouse::SetMode(MouseMode::Disabled);

		UpdateCamera();

		if (Input::Mouse::GetButtonUnpressed(MouseButton::Right) || !isActive)
		{
			Input::Mouse::SetMode(MouseMode::Normal);
			m_captured = false;
		}
	}

	m_firstFrameFocus = false;

	//	Here we will update local camera

	if (!m_captured && Input::Keyboard::GetKeyPressed(Key::Space))
	{
		switch (m_gizmoMode)
		{
		case ImGuizmo::TRANSLATE: m_gizmoMode = ImGuizmo::ROTATE; break;
		case ImGuizmo::ROTATE: m_gizmoMode = ImGuizmo::SCALE; break;
		case ImGuizmo::SCALE: m_gizmoMode = ImGuizmo::TRANSLATE; break;
		default: ImGuizmo::TRANSLATE; break;
		}
	}

	m_sceneRenderParam.UpdateFrameBuffer();

	m_cameraEditor.Update();

	if (isVisible) RenderFramebuffer();
}

void RenderSceneWidget::Display()
{
	if (!isActive) return;

	if (BeginWidget())
	{
		EditorContext::Instance().allowGameObjectCopy |= ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
		BeginRenderChildWindow();

		m_enablePicking = ImGui::IsWindowHovered();

		ImVec2 screenPos = ImGui::GetCursorScreenPos();
		ImVec2 dimension = ImGui::GetWindowSize();

		//	Render Image
		float width = dimension.x;
		float height = dimension.y;

		if (width != m_renderFieldDimension.x || height != m_renderFieldDimension.y)
		{
			m_renderFieldDimension = { width, height };
			m_sceneRenderParam.SetViewportRatio(width/ height);
			m_sceneRenderParam.SetContainerDimensions(m_renderFieldDimension);
			m_renderPicking.UpdateDimensions(m_renderFieldDimension);
		}

		ImVec2 dim = {
			m_sceneRenderParam.GetViewportDimensions().x,
			m_sceneRenderParam.GetViewportDimensions().y
		};

		ImGui::Image(GUI::CastTextureID(m_sceneRenderParam.GetRenderedImage()), dim, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

		m_renderFieldLocalPosition.x = screenPos.x;
		m_renderFieldLocalPosition.y = screenPos.y;

		m_renderFieldPosition.x = m_renderFieldLocalPosition.x - EditorContext::Instance().window->GetPosX();
		m_renderFieldPosition.y = m_renderFieldLocalPosition.y - EditorContext::Instance().window->GetPosY();

		//	Picking position update
		m_renderPicking.UpdatePosition(m_renderFieldPosition);

		//	Lerp animation camera preview displaying

		if (m_displayCameraPreview != (m_renderGizmo.selectedCamera != nullptr))
		{
			m_displayCameraPreview = m_renderGizmo.selectedCamera != nullptr;
			m_elapsedTimeCameraPreview = 0.0f;
		}

		if (m_displayCameraPreview)
		{
			m_cameraTargetPreviewWindowDimensions.x = m_renderFieldDimension.x * 0.2f;
			m_cameraTargetPreviewWindowDimensions.y = m_cameraTargetPreviewWindowDimensions.x / m_renderGizmo.selectedCamera->GetAspect();
		}

		m_elapsedTimeCameraPreview += EngineContext::Instance().time->GetUnscaledDeltaTime();
		float deltaLerp = Maths::Min(m_elapsedTimeCameraPreview / .25f, 1.0f);

		m_cameraPreviewWindowDimensions = m_displayCameraPreview ?
			EasingCurveFunctions::Lerp(Vector2::Zero, m_cameraTargetPreviewWindowDimensions, EasingCurve::EaseOutCubic, deltaLerp) :
			EasingCurveFunctions::Lerp(m_cameraTargetPreviewWindowDimensions, Vector2::Zero, EasingCurve::EaseInCubic, deltaLerp);
		m_cameraPreviewRenderParam.SetViewportDimensions(m_cameraPreviewWindowDimensions);

		if (m_cameraPreviewWindowDimensions != Vector2::Zero)
		{
			float alpha = m_displayCameraPreview ?
				EasingCurveFunctions::Lerp(0.0f, 1.0f, EasingCurve::EaseOutCubic, deltaLerp) :
				EasingCurveFunctions::Lerp(1.0f, 0.0f, EasingCurve::EaseInCubic, deltaLerp);

			GUI::PushStyle(ImGuiStyleVar_Alpha, alpha);

			ImVec2 childPosition;
			childPosition.x = m_renderFieldLocalPosition.x + m_renderFieldDimension.x - m_cameraPreviewWindowDimensions.x - 25.f;
			childPosition.y = m_renderFieldLocalPosition.y + m_renderFieldDimension.y - m_cameraPreviewWindowDimensions.y - 25.f;

			ImGui::SetItemAllowOverlap();
			ImGui::SetCursorScreenPos(childPosition);

			GUI::PushStyle(ImGuiStyleVar_WindowPadding, { 1.f,1.f });
			GUI::PushColor(ImGuiCol_ChildBg, EditorContext::Instance().theme->color_interactives.activeColor);
			bool opened = ImGui::BeginChild("###CAMERA_PREVIEW", ImVec2(m_cameraPreviewWindowDimensions.x, m_cameraPreviewWindowDimensions.y), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
			GUI::PopColor();
			GUI::PopStyle();

			if (opened)
			{
				if (m_displayCameraPreview)
				{
					m_cameraPreviewDimensions = Vector2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

					ImGui::Image(GUI::CastTextureID(m_cameraPreviewRenderParam.GetRenderedImage()), ImVec2(m_cameraPreviewDimensions.x, m_cameraPreviewDimensions.y), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
				}
			}

			ImGui::EndChild();
			GUI::PopStyle();
		}

		//	Editor functions

		ImGuizmo::SetDrawlist();
		ImGuiIO& io = ImGui::GetIO();

		float viewManipulateRight = io.DisplaySize.x;
		float viewManipulateTop = 0;

		ImGuizmo::SetRect(m_renderFieldLocalPosition.x, m_renderFieldLocalPosition.y, m_renderFieldDimension.x, m_renderFieldDimension.y);
		viewManipulateRight = m_renderFieldLocalPosition.x + m_renderFieldDimension.x;
		viewManipulateTop = m_renderFieldLocalPosition.y;

		if (!EditorContext::Instance().selectionManager->GetGameObjects().empty())
		{
			Gizmo();

			m_enablePicking &= !ImGuizmo::IsOver();
		}

		EndRenderChildWindow();


		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && Input::Keyboard::GetKeyPressed(Key::Delete))
		{
			EditorContext::Instance().selectionManager->DeleteGameObjects();
		}
	}
	EndWidget();
}

void RenderSceneWidget::Gizmo()
{
	EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
	GameObject* selected = selecManager->GetGameObject(0);

	if (selected == nullptr) return;

	GameObject* parent = selected->GetParent();
	Matrix4 mat = selected->transform.GetWorldMatrix();

	Vector3 snap = Vector3::One;
	switch (m_gizmoMode)
	{
	case ImGuizmo::TRANSLATE:
		snap = { m_snapValue[0],m_snapValue[0],m_snapValue[0] };
		break;
	case ImGuizmo::ROTATE:
		snap = { m_snapValue[1],m_snapValue[1],m_snapValue[1] };
		break;
	case ImGuizmo::SCALE:
		snap = { m_snapValue[2],m_snapValue[2],m_snapValue[2] };
		break;
	default:break;
	}

	if (ImGuizmo::Manipulate(
		m_cameraEditor.GetViewMatrix().elements, m_cameraEditor.GetProjectionMatrix().elements,
		static_cast<ImGuizmo::OPERATION>(m_gizmoMode), static_cast<ImGuizmo::MODE>(m_refMode),
		mat.elements, m_deltaMatrix, snap.elements))
	{
		// Retreive local transform
		if (selected->GetParent())
			mat = Matrix4::Inverse(selected->GetParent()->transform.GetWorldMatrix()) * mat;

		Vector3 position, rotation, scale;
		ImGuizmo::DecomposeMatrixToComponents(mat.elements, position.elements, rotation.elements, scale.elements);

		// Set locally and update transform hint fields
		switch (m_gizmoMode)
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			selected->transform.SetLocalPosition(position);
			selecManager->selectedObjectPosition = position;
			break;
		case ImGuizmo::OPERATION::ROTATE:
			selected->transform.SetLocalEulerAngles(rotation);
			selecManager->selectedObjectRotation = rotation;
			break;
		case ImGuizmo::OPERATION::SCALE:
			selected->transform.SetLocalScale(scale);
			selecManager->selectedObjectScale = scale;
			break;
		default:
			break;
		}
	}
}

void RenderSceneWidget::RenderFramebuffer()
{
	RenderSystem& renderer = SystemManager::GetRenderSystem();

	if (m_renderGizmo.selectedCamera)
	{
		m_cameraPreviewRenderParam.SetViewportDimensions(1920.f,1080.f);
		m_cameraPreviewRenderParam.BindCamera(m_renderGizmo.selectedCamera);
		renderer.Render(m_cameraPreviewRenderParam);
	}

	renderer.PushRenderPipeline(&m_renderGizmo, static_cast<unsigned int>(renderer.GetPushedRenderPipelineCount()-1));

	SystemManager::GetPhysicsSystem().DrawDebug();

	//	Picking is enabled
	if (m_enablePicking)
	{
		renderer.PushRenderPipeline(&m_renderPicking, 0);
		renderer.Render(m_sceneRenderParam);
		renderer.PopRenderPipeline();

		if (m_renderPicking.updated) m_keepFocus = true;
	}

	//	Picking diabled (while manipuling gizmo)
	else
	{
		renderer.Render(m_sceneRenderParam);
	}
	renderer.PopRenderPipeline();
}



void RenderSceneWidget::ParameterBar()
{
	ImVec2 dim = { ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 2.f };

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(7.5f, 2.5f));
	bool opened = ImGui::BeginChild("###ParamBar", dim, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding);
	GUI::PopStyle();

	if (opened && ImGui::BeginTable("Param", 4, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableNextColumn();
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Space :");

			ImGui::SameLine();

			if (ImGui::RadioButton("World", m_refMode == ImGuizmo::WORLD))
			{
				m_refMode = ImGuizmo::WORLD;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Local", m_refMode == ImGuizmo::LOCAL))
			{
				m_refMode = ImGuizmo::LOCAL;
			}
		}

		ImGui::TableNextColumn();
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Mode :");

			ImGui::SameLine();

			if (ImGui::RadioButton("Translate", m_gizmoMode == ImGuizmo::TRANSLATE))
			{
				m_gizmoMode = ImGuizmo::TRANSLATE;
			}


			ImGui::SameLine();

			if (ImGui::RadioButton("Rotate", m_gizmoMode == ImGuizmo::ROTATE))
			{
				m_gizmoMode = ImGuizmo::ROTATE;
			}


			ImGui::SameLine();

			if (ImGui::RadioButton("Scale", m_gizmoMode == ImGuizmo::SCALE))
			{
				m_gizmoMode = ImGuizmo::SCALE;
			}
		}
		ImGui::TableNextColumn();

		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Snap :");

			ImGui::SameLine();

			switch (m_gizmoMode)
			{
			case ImGuizmo::TRANSLATE:
				ImGui::DragFloat("###Snap", &m_snapValue[0], 0.25f, 0.001f, 500.f);
				break;
			case ImGuizmo::ROTATE:
				ImGui::DragFloat("###Snap", &m_snapValue[1], 0.25f, 0.001f, 500.f);
				break;
			case ImGuizmo::SCALE:
				ImGui::DragFloat("###Snap", &m_snapValue[2], 0.25f, 0.001f, 500.f);
				break;
			default:break;
			}
		}
		ImGui::TableNextColumn();

		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Speed :");

			ImGui::SameLine();

			ImGui::DragFloat("###NavSpeed", &m_cameraSpeed, 0.1f, Camera_Minimum_Speed, Camera_Maximum_Speed);
		}

		ImGui::EndTable();
	}
	
	ImGui::EndChild();
}

void RenderSceneWidget::CreateSaveSceneCamera(Scene& scene)
{
	json jsonFile;

	// Read json if exists
	if (std::filesystem::exists(m_scenesSavePath))
		ReadJson(m_scenesSavePath, jsonFile);

	nlohmann::json& jsonScene = jsonFile[std::string(scene.GetUID())];
	nlohmann::json& jsonCamera = jsonScene["Camera"];

	// Save camera transform
	Vector3 nullVector = Vector3::Zero;
	Serialization::SetContainer<float>(jsonCamera["Position"], nullVector);
	Serialization::SetContainer<float>(jsonCamera["Rotation"], nullVector);

	// Save camera frustum data
	Vector3 camFrustumData = { 60.f, 0.1f, 500.f };
	Serialization::SetContainer<float>(jsonCamera["Frustum"], camFrustumData);

	// Save camera editor preferences
	Vector2 camData = { 75.f, 0.05f };
	Serialization::SetContainer<float>(jsonCamera["Data"], camData);

	WriteJson(m_scenesSavePath, jsonFile);
}

void RenderSceneWidget::SaveSceneCamera()
{
	json jsonFile;

	// Read json if exists
	if (std::filesystem::exists(m_scenesSavePath))
		ReadJson(m_scenesSavePath, jsonFile);

	Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene();
	if (scene == nullptr)
		return;

	// Save current scene camera
	json& jsonScene = jsonFile[std::string(scene->GetUID())];
	SaveEditorCamera(jsonScene);

	WriteJson(m_scenesSavePath, jsonFile);
}

void RenderSceneWidget::LoadSceneCamera()
{
	json jsonFile;

	// Read json if exists
	if (!std::filesystem::exists(m_scenesSavePath))
		return; // No save (should not happen)

	ReadJson(m_scenesSavePath, jsonFile);

	Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene();
	if (scene == nullptr)
		return;

	json& jsonScene = jsonFile[std::string(scene->GetUID())];
	LoadEditorCamera(jsonScene);
}

void RenderSceneWidget::SaveEditorCamera(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonCamera = jsonFile["Camera"];

	// Save camera transform
	Serialization::SetContainer<float>(jsonCamera["Position"], m_cameraPosition);
	Serialization::SetContainer<float>(jsonCamera["Rotation"], m_cameraRotation);

	// Save camera frustum data
	Vector3 camFrustumData = { m_cameraEditor.GetFOV(), m_cameraEditor.GetNear(), m_cameraEditor.GetFar() };
	Serialization::SetContainer<float>(jsonCamera["Frustum"], camFrustumData);

	// Save camera editor preferences
	Vector2 camData = { m_cameraSensivity, m_cameraSpeed };
	Serialization::SetContainer<float>(jsonCamera["Data"], camData);
}

void RenderSceneWidget::LoadEditorCamera(nlohmann::json& jsonFile)
{
	nlohmann::json& jsonCamera = jsonFile["Camera"];

	// Load camera transform
	Serialization::TryGetContainer<float>(jsonCamera, "Position", m_cameraPosition);
	Serialization::TryGetContainer<float>(jsonCamera, "Rotation", m_cameraRotation);

	// Load camera frustum data
	Vector3 camFrustumData;
	if (Serialization::TryGetContainer<float>(jsonCamera, "Frustum", camFrustumData))
	{
		m_cameraEditor.SetFOV(camFrustumData.x);
		m_cameraEditor.SetNear(camFrustumData.y);
		m_cameraEditor.SetFar(camFrustumData.z);
	}

	// Load editor camera preferences
	Vector2 camData;
	if (Serialization::TryGetContainer<float>(jsonCamera, "Data", camData))
	{
		m_cameraSensivity = camData.x;
		m_cameraSpeed = camData.y;
	}

	// Prepare camera transform
	m_cameraEditor.SetTransform(m_cameraPosition, Quaternion::FromEulerRad(m_cameraRotation * Maths::DEGTORAD));
}
