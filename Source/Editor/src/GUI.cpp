#include "GUI.hpp"

#include <string>
#include <cmath>
#include <stdint.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <Types/Color.hpp>
#include <Core/TaskQueue.hpp>
#include <Tools/ReflectedSTD.hpp>
#include <Physics/BoolVector3.hpp>
#include <Tools/StringHelper.hpp>
#include <Maths/Vector4.hpp>
#include <Maths/Quaternion.hpp>
#include <Maths/Matrix3.hpp>
#include <Maths/Matrix4.hpp>

#include <EngineContext.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Resources/Resource/Mesh.hpp>

#include <Core/Logger.hpp>
#include <Refureku/Refureku.h>
#include <ECS/Component.hpp>
#include <ECS/GameObject.hpp>
#include <IO/Mouse.hpp>

#include "io/Window.hpp"
#include "EditorTheme.hpp"
#include "EditorContext.hpp"
#include "EditorResourcePreview.hpp"
#include "EditorSelectionManager.hpp"
#include "EditorDragManager.hpp"

constexpr int	String_Max_Buffer_Size = 128;
constexpr float decimalStep = 0.1f;
constexpr float integerStep = 1.f;

void GUI::FieldLabel(std::string_view label)
{
	std::string result;

	int size = static_cast<int>(label.size());

	int i = 0;

	//	Remove conventions prefixes
	if (label[0] == 'm' && label[1] == '_')
	{
		i += 2;
	}
	
	while (label[i] == '_')
	{
		i++;
	}

	//	Force uppercase for the first characters
	if (label[i] >= 'a' && label[i] <= 'z')
	{
		result.push_back(label[i] - ('a' - 'A'));
		i++;
	}

	bool wasUppercase = false;


	for (i; i < size; i++)
	{
		//	If is an uppercase character
		if (label[i] >= 'A' && label[i] <= 'Z')
		{
			//	Was the last character an uppercase ?
			if (!wasUppercase)
			{
				//	If it wasn't then add a space between the lowecase and the uppercase character
				result.push_back(' ');
			}

			//	Then add this uppercase character
			result.push_back(label[i]);

			//	Set 'wasUppercase' boolean to true
			wasUppercase = true;
		}
		else
		{
			wasUppercase = false;

			//	If is a special character considered as a space
			if (label[i] == '_')
			{
				if (!result.empty())
				{
					result.push_back(' ');
				}
			}

			//	If not a special character
			else
			{
				result.push_back(label[i]);
			}
		}
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text(result.c_str());
}

bool GUI::BeginField(const char* ID)
{
	float panelWidth = ImGui::GetContentRegionAvail().x;

	bool out = ImGui::BeginTable(ID, 2, ImGuiTableFlags_SizingStretchProp);
	if (out)
	{
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

		ImGui::TableNextColumn();
	}

	return out;
}

void GUI::BeginFieldContent()
{
	ImGui::TableNextColumn();
}

void GUI::EndField()
{
	ImGui::EndTable();
}


// Base function to inspect a fundamental field (int, float, char, string, etc..)
bool RFKInspectObject::TryInspectFundamental(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, bool& isModified)
{
	// Check if the field is coresponding to a fundamental field
	auto it = m_inspectMap.find(fieldData.arch->getId());
	if (it == m_inspectMap.end())
		return false;

	// Draw the fundamental field
	isModified = it->second(fieldData, properties, GUI::EFieldInspectType::FUNDAMENTAL);

	return true;
}

// Base function to inspect an enum field (enum, enum class)
bool RFKInspectObject::TryInspectEnum(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, bool& isModified)
{
	// Check if the field is coresponding to an enum
	if (!rfk::enumCast(fieldData.arch))
		return false;

	// Draw the enum
	isModified = GUI::DrawField<int16_t>(fieldData, properties, GUI::EFieldInspectType::ENUM);

	return true;
}

bool RFKInspectObject::TryDisplayEntity(GUI::InspectFieldData& fieldData, rfk::Class const* fieldClass, PropertyContainer const& properties)
{
	// Check if the entity inherit from BaseObject (else discarded)
	if (fieldClass->isSubclassOf(BaseObject::staticGetArchetype()))
	{
		bool opened = false;

		ImGui::BeginGroup();
		if (GUI::BeginField("###CLASS_FIELD"))
		{
			GUI::FieldLabel(fieldData.name.data());

			GUI::BeginFieldContent();

			if (ImGui::TreeNodeEx("###DropDown"))
			{
				opened = true;
				ImGui::TreePop();
			}
		}
		GUI::EndField();

		if (opened)
		{
			ImGui::Indent(ImGui::GetStyle().IndentSpacing);
			ImGui::BeginGroup();

			// Display the field members
			bool modified = InspectorMethods::DisplayEntity(reinterpret_cast<BaseObject*>(fieldData.data), fieldClass);

			if (modified)
			{
				// Check for a potential OnModify event attached
				auto it = properties.find(POnModify::staticGetArchetype().getId());
				if (it != properties.end())
				{
					// Call the attached function
					POnModify const* onModifyProp = static_cast<POnModify const*>(it->second);
					onModifyProp->Call(*fieldData.ownerClass, fieldData.ownerObject);
				}
			}

			ImGui::EndGroup();
			ImGui::EndGroup();

			return modified;
		}
		ImGui::EndGroup();
	}

	return false;
}

bool RFKInspectObject::TryInspectReference(GUI::InspectFieldData& fieldData, rfk::Class const* fieldClass, PropertyContainer const& properties, bool& isModified)
{
	auto TryDrawRef = [&]<typename TType>() {
		rfk::Class const& typeArch = TType::staticGetArchetype();

		// Check if the field archetype is based on the given archetype
		if (!fieldClass->isBaseOf(typeArch) && !fieldClass->isSubclassOf(typeArch))
			return false;

		// Draw the reference field
		isModified = GUI::DrawField<TType>(fieldData, properties, GUI::EFieldInspectType::REFERENCE);

		return true;
	};

	if (TryDrawRef.operator()<Resource>	 () ||
		TryDrawRef.operator()<Component> () ||
		TryDrawRef.operator()<GameObject>())
		return true;

	return false;
}

inline std::map<size_t, bool (*)(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, GUI::EFieldInspectType fieldType)> const RFKInspectObject::m_inspectMap
{
		{ rfk::getType<bool>().getArchetype()->getId(),					&GUI::DrawField<bool> },
		{ rfk::getType<char>().getArchetype()->getId(),					&GUI::DrawField<char> },
		{ rfk::getType<short>().getArchetype()->getId(),				&GUI::DrawField<short> },
		{ rfk::getType<unsigned short>().getArchetype()->getId(),		&GUI::DrawField<unsigned short> },
		{ rfk::getType<int>().getArchetype()->getId(),					&GUI::DrawField<int> },
		{ rfk::getType<unsigned int>().getArchetype()->getId(),			&GUI::DrawField<unsigned int> },
		{ rfk::getType<long long>().getArchetype()->getId(),			&GUI::DrawField<long long> },
		{ rfk::getType<unsigned long long>().getArchetype()->getId(),	&GUI::DrawField<unsigned long long> },
		{ rfk::getType<float>().getArchetype()->getId(),				&GUI::DrawField<float> },
		{ rfk::getType<double>().getArchetype()->getId(),				&GUI::DrawField<double> },

		{ rfk::getType<HyString>().getArchetype()->getId(),				&GUI::DrawField<HyString> },
		{ rfk::getType<Color3>().getArchetype()->getId(),				&GUI::DrawField<Color3> },
		{ rfk::getType<Color4>().getArchetype()->getId(),				&GUI::DrawField<Color4> },
		{ rfk::getType<BoolVector3>().getArchetype()->getId(),			&GUI::DrawField<BoolVector3> },
		{ rfk::getType<Vector2>().getArchetype()->getId(),				&GUI::DrawField<Vector2> },
		{ rfk::getType<Vector3>().getArchetype()->getId(),				&GUI::DrawField<Vector3> },
		{ rfk::getType<Vector4>().getArchetype()->getId(),				&GUI::DrawField<Vector4> },
		{ rfk::getType<Quaternion>().getArchetype()->getId(),			&GUI::DrawField<Quaternion> },
		{ rfk::getType<Matrix3>().getArchetype()->getId(),				&GUI::DrawField<Matrix3> },
		{ rfk::getType<Matrix4>().getArchetype()->getId(),				&GUI::DrawField<Matrix4> }
};

template <typename Type>
void SetMinMaxFromProperties(Type& min, Type& max, Type minClamp, Type maxClamp, GUI::InspectPropertiesData const& propertiesData)
{
	min = Maths::Clamp(static_cast<Type>(propertiesData.dragMin), minClamp, maxClamp);
	max = Maths::Clamp(static_cast<Type>(propertiesData.dragMax), minClamp, maxClamp);
}

#include <functional>

// Base function to draw a fundamental field or a SceneObject reference
bool DrawFieldCustom(const char* label, std::string_view id, GUI::InspectPropertiesData const& propertiesData, std::function<bool()> function)
{
	bool out = false;
	std::string ID = id.data() + std::string(label);

	if (GUI::BeginField(ID.c_str()))
	{
		ImGui::BeginDisabled(propertiesData.readOnly);
			
		GUI::FieldLabel(label);

		GUI::BeginFieldContent();

		GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 0.5f);

		out = function();

		GUI::PopStyle();

		ImGui::EndDisabled();

		GUI::EndField();
	}

	return out;
}

// Main function to display a field in the editor inspector (resources or gameobjects)
template <typename Type>
bool GUI::DrawField(InspectFieldData& fieldData, PropertyContainer const& properties, GUI::EFieldInspectType fieldType)
{
	GUI::InspectPropertiesData propertiesData;

	// Check for known poperties and prepare display data
	{
		auto it = properties.find(PRange::staticGetArchetype().getId());
		if (it != properties.end())
		{
			PRange const& range = *static_cast<PRange const*>(it->second);
			propertiesData.dragMin = range.getMin();
			propertiesData.dragMax = range.getMax();
		}

		it = properties.find(PReadOnly::staticGetArchetype().getId());
		propertiesData.readOnly = it != properties.end();
	}

	bool isModified = false;

	// Display from the field type
	switch (fieldType)
	{
	case GUI::EFieldInspectType::FUNDAMENTAL:
		isModified =  GUI::DrawFundamental<Type>(fieldData.name.data(), fieldData.data, propertiesData);
		break;
	case GUI::EFieldInspectType::ENUM:
		isModified = GUI::DrawEnum(fieldData.name.data(), fieldData.arch, fieldData.data, propertiesData);
		break;
	case GUI::EFieldInspectType::FLAG:
		isModified = GUI::DrawFlag(fieldData.name.data(), fieldData.arch, fieldData.data, propertiesData);
		break;
	case GUI::EFieldInspectType::REFERENCE:
		isModified = GUI::DrawRef<Type>(fieldData.name.data(), fieldData.arch, fieldData.data, propertiesData);
		break;
	default:
		break;
	}

	if (isModified)
	{
		// Check for a potential OnModify event attached
		auto it = properties.find(POnModify::staticGetArchetype().getId());
		if (it != properties.end())
		{
			// Call the attached function
			POnModify const* onModifyProp = static_cast<POnModify const*>(it->second);
			onModifyProp->Call<Type>(*fieldData.ownerClass, fieldData.ownerObject, fieldData.data);
		}
	}

	return isModified;
}

// Base function to display an enum in the editor inspector
bool GUI::DrawEnum(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	rfk::Enum const* e = rfk::enumCast(fieldArch);

	if (e == nullptr) return false;
	
	bool out = false;
	std::string ID = "###ENUM_" + std::string(e->getName());

	if (BeginField(ID.c_str()))
	{
		FieldLabel(fieldName);

		BeginFieldContent();

		//	Here we need to get int 16 or 32, if not the value is messed up
		const rfk::EnumValue* currentValue = e->getEnumValue(*static_cast<rfk::int16*>(fieldData));

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		//	Mono selection enum
		if (ImGui::BeginCombo("", currentValue->getName()))
		{
			// Get all the enum values from the archetype
			auto enumValues = e->getEnumValuesByPredicate([](rfk::EnumValue const& enumValue, void* data) { return true; }, nullptr);

			// Iterate on enum values
			for (const auto& enumValue : enumValues)
			{
				bool isSelected = currentValue == enumValue;
				if (ImGui::Selectable(enumValue->getName(), &isSelected))
				{
					// Set the field data
					*static_cast<rfk::int16*>(fieldData) = static_cast<rfk::int16>(enumValue->getValue());
					out = true;
				}
			}
			ImGui::EndCombo();
		}
	}
	EndField();
	

	return out;
}

// Base function to display a flag in the editor inspector
bool GUI::DrawFlag(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(fieldName, "###RESOURCE_", propertiesData, [&]() {

		bool out = false;

		rfk::ClassTemplateInstantiation const* cTempInstance = rfk::classTemplateInstantiationCast(fieldArch);
		rfk::TypeTemplateArgument const& typeTempArg = *static_cast<const rfk::TypeTemplateArgument*>(&cTempInstance->getTemplateArgumentAt(0));
		rfk::Enum const* enumField = rfk::enumCast(typeTempArg.getType().getArchetype());

		if (enumField == nullptr)
			return false;

		rfk::Method const* setMethod = cTempInstance->getMethodByName("EnableFromInt");
		rfk::Method const* removeMethod = cTempInstance->getMethodByName("DisableFromInt");
		rfk::Method const* checkMethod = cTempInstance->getMethodByName("TestBitFromInt");

		std::size_t valuesCount = enumField->getEnumValuesCount();
		for (std::size_t valueIndex = 0; valueIndex < valuesCount; valueIndex++)
		{
			rfk::EnumValue const& enumValue = enumField->getEnumValueAt(valueIndex);
			uint64_t valueFlag = enumValue.getValue();

			bool isActive = checkMethod->invokeUnsafe<bool>(fieldData, std::move(valueFlag));
			ImGuiDir dir = isActive ? 1 : 8;

			std::string id = fieldName + std::string(enumValue.getName());
			ImGui::PushID(id.c_str());

			if (ImGui::ArrowButton("Yo", dir))
			{
				out = true;

				if (isActive)
				{
					isActive = false;
					removeMethod->invokeUnsafe<void>(fieldData, std::move(valueFlag));
				}
				else
				{
					isActive = true;
					setMethod->invokeUnsafe<void>(fieldData, std::move(valueFlag));
				}
			}

			ImGui::SameLine();

			ImGui::BeginDisabled(!isActive);
			ImGui::Text(enumValue.getName());
			ImGui::EndDisabled();

			ImGui::PopID();
		}

		ImGui::Spacing(); ImGui::Spacing();

		return out;
		});
}

// Base function to display a SceneObject reference in the editor inspector
template <typename Type>
bool GUI::DrawRef(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	Logger::Warning("GUI - The field you tried to display is not supported");
	return false;
}

// Base function to display a Resource reference in the editor inspector
template<>
bool GUI::DrawRef<Resource>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(fieldName, "###RESOURCE_", propertiesData, [&]() {
		
		bool out = false;

		// Retrieve the dragged resource
		Resource* draggedResource = EditorContext::Instance().dragManager->GetDraggedResource();

		bool isCompatible = draggedResource && draggedResource->getArchetype().getId() == fieldArch->getId();

		if (ImGui::BeginTable("###PreviewAndDescription", 2, ImGuiTableFlags_SizingStretchProp))
		{
			float descWidth = ImGui::GetContentRegionAvail().x;

			ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, descWidth * 0.25f);
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, descWidth * 0.75f);

			ImGui::TableNextColumn();

			float width = ImGui::GetContentRegionAvail().x;

			ImVec2 thumbnaildimensions = ImVec2(width * 0.90f, width * 0.90f);
			ImVec2 thumbnailPos = ImGui::GetCursorPos();

			Color4 borderColor = EditorContext::Instance().theme->color_borders.defaultColor;

			if (EditorContext::Instance().dragManager->IsDragging())
			{
				if (isCompatible) borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;

				else borderColor = Color4::DarkRed;
			}

			GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
			GUI::PushColor(ImGuiCol_Border, borderColor);

			Resource** resourcePtr = static_cast<Resource**>(fieldData);

			// Draw the resource preview if one attached
			unsigned int textureID = 0;
			if (resourcePtr && *resourcePtr != nullptr) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(**resourcePtr);

			ImGui::ImageButton(GUI::CastTextureID(textureID), thumbnaildimensions, ImVec2(0, 1), ImVec2(1, 0), 2);

			// Check if the mouse click was release
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && *resourcePtr)
			{
				EditorSelectionManager* selec = EditorContext::Instance().selectionManager;
				EditorContext::Instance().taskQueue->AddTask([selec, resourcePtr] {
					selec->ClearResources();
					selec->AddToSelection(*resourcePtr);
					});
			}

			GUI::PopColor();
			GUI::PopStyle();

			if (ImGui::BeginDragDropTarget())
			{
				if (ImGui::AcceptDragDropPayload("DND_Resources"))
				{
					if (draggedResource)
					{
						if (isCompatible)
						{
							// Set the new field resource reference
							*resourcePtr = draggedResource;
							out = true;
						}
					}
					EditorContext::Instance().dragManager->EndDrag();
				}
				ImGui::EndDragDropTarget();

				if (isCompatible)
				{
					ImGui::SetCursorPos(thumbnailPos);
					ImGui::SetItemAllowOverlap();

					GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
					GUI::PushColor(ImGuiCol_Border, EditorContext::Instance().theme->color_interactives.activeColor);

					unsigned int textureID = 0;
					if (draggedResource) textureID = EditorContext::Instance().resourcePreview->GetResourcePreview(*draggedResource);

					ImGui::ImageButton(GUI::CastTextureID(textureID), thumbnaildimensions, ImVec2(0, 1), ImVec2(1, 0), 2);
					GUI::PopColor();
					GUI::PopStyle();
				}
			}
			else if (resourcePtr && *resourcePtr && ImGui::BeginDragDropSource())
			{
				EditorContext::Instance().dragManager->BeginDrag(*resourcePtr);

				ImGui::Text((*resourcePtr)->GetFilename().c_str());
				ImGui::SetDragDropPayload("DND_Resources", nullptr, 0);
				ImGui::EndDragDropSource();
			}

			GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
			bool resourceFieldContextMenu = ImGui::BeginPopupContextItem("###RESOURCE_FIELD_CONTEXTMENU");
			GUI::PopStyle();

			if (resourceFieldContextMenu)
			{
				if (ImGui::MenuItem("Remove"))
				{
					*resourcePtr = nullptr;
					out = true;
				}
				ImGui::EndPopup();
			}

			ImGui::TableNextColumn();

			if (*resourcePtr)
			{
				ImGui::Text(("Name : " + StringHelper::GetFileNameWithoutExtension((*resourcePtr)->GetFilename())).c_str());
				ImGui::Text(("From : " + (*resourcePtr)->GetFilepath()).c_str());
			}

			ImGui::EndTable();
		}

		return out;
		});
}

// Base function to display a GameObject reference in the editor inspector
template<>
bool GUI::DrawRef<GameObject>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(fieldName, "###GAMEOBJECT_", propertiesData, [&]() {

		GameObject** goPtr = static_cast<GameObject**>(fieldData);
		GameObject* draggedGameObject = EditorContext::Instance().dragManager->GetDraggedGameObject();

		bool out = false;
		bool isCompatible = draggedGameObject != nullptr;

		std::string text = "none";

		if (*goPtr) text = (*goPtr)->GetName();

		Color4 borderColor = EditorContext::Instance().theme->color_borders.defaultColor;

		if (EditorContext::Instance().dragManager->IsDragging())
		{
			if (isCompatible) borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;

			else borderColor = Color4::DarkRed;
		}

		ImVec2 savedPos = ImGui::GetCursorPos();
		ImVec2 savedDim = ImVec2(ImGui::GetContentRegionAvail().x, 0);

		GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
		GUI::PushColor(ImGuiCol_Border, borderColor);
		ImGui::Button(text.c_str(), savedDim);
		GUI::PopColor();
		GUI::PopStyle();

		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload("DND_GameObject"))
			{
				if (isCompatible)
				{
					*goPtr = draggedGameObject;
					out = true;
				}

				EditorContext::Instance().dragManager->EndDrag();
			}
			ImGui::EndDragDropTarget();

			if (isCompatible)
			{
				ImGui::SetCursorPos(savedPos);
				ImGui::SetItemAllowOverlap();

				GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
				GUI::PushColor(ImGuiCol_Border, EditorContext::Instance().theme->color_interactives.activeColor);
				ImGui::Button(text.c_str(), savedDim);
				GUI::PopColor();
				GUI::PopStyle();
			}

			GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
			bool gamebjectFieldContextMenu = ImGui::BeginPopupContextItem("###GAMEOBJECT_FIELD_CONTEXTMENU");
			GUI::PopStyle();

			if (gamebjectFieldContextMenu)
			{
				if (ImGui::MenuItem("Remove"))
				{
					*goPtr = nullptr;
					out = true;
				}
				ImGui::EndPopup();
			}
		}

		return out;
		});
}

// Base function to display a Component reference in the editor inspector
template<>
bool GUI::DrawRef<Component>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData)
{
	GameObject* draggedGameObject = EditorContext::Instance().dragManager->GetDraggedGameObject();

	bool isCompatible = false;
	Component* targetedComponent = nullptr;

	if (draggedGameObject != nullptr)
	{
		rfk::Class const* fieldClass = rfk::classCast(fieldArch);

		// Check if the dragged component is compatible with the field reference
		for (Component* c : draggedGameObject->GetRawComponents())
		{
			if (c->getArchetype().isBaseOf(*fieldClass) ||
				c->getArchetype().isSubclassOf(*fieldClass))
			{
				isCompatible = true;
				targetedComponent = c;

				break;
			}
		}
	}

	// Draw the field
	return DrawFieldCustom(fieldName, "###COMPONENT_", propertiesData, [&]() {

		bool out = false;

		float width = ImGui::GetContentRegionAvail().x;

		std::string text = "none";

		Component** compRefPointer = static_cast<Component**>(fieldData);
		GameObject* fieldGameObject = *compRefPointer ? &(*compRefPointer)->gameObject : nullptr;

		if (fieldGameObject) text = fieldGameObject->GetName();

		Color4 borderColor = EditorContext::Instance().theme->color_borders.defaultColor;

		if (isCompatible) borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;

		if (EditorContext::Instance().dragManager->IsDragging())
		{
			if (isCompatible) borderColor = EditorContext::Instance().theme->color_interactives.defaultColor;

			else borderColor = Color4::DarkRed;
		}

		ImVec2 savedPos = ImGui::GetCursorPos();
		ImVec2 savedDim = ImVec2(ImGui::GetContentRegionAvail().x, 0);

		GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
		GUI::PushColor(ImGuiCol_Border, borderColor);
		ImGui::Button(text.c_str(), savedDim);
		GUI::PopColor();
		GUI::PopStyle();

		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload("DND_GameObject"))
			{
				if (isCompatible)
				{
					*compRefPointer = targetedComponent;
					out = true;
				}

				EditorContext::Instance().dragManager->EndDrag();
			}
			ImGui::EndDragDropTarget();

			if (isCompatible)
			{
				ImGui::SetCursorPos(savedPos);
				ImGui::SetItemAllowOverlap();

				GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 2.f);
				GUI::PushColor(ImGuiCol_Border, EditorContext::Instance().theme->color_interactives.activeColor);;
				ImGui::Button(text.c_str(), savedDim);
				GUI::PopColor();
				GUI::PopStyle();
			}

			GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
			bool componentFieldContextMenu = ImGui::BeginPopupContextItem("###COMPONENT_FIELD_CONTEXTMENU");
			GUI::PopStyle();

			if (componentFieldContextMenu)
			{
				if (ImGui::MenuItem("Remove"))
				{
					*compRefPointer = nullptr;
					out = true;
				}
				ImGui::EndPopup();
			}
		}

		return out;
		});
}


//	FIELDS

// Base function to display a fundamental field in the editor inspector
template <typename Type>
bool GUI::DrawFundamental(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	Logger::Warning("GUI - The field you tried to display is not supported");
	return false;
}

// Base function to display a bool in the editor inspector
template <>
bool GUI::DrawFundamental<bool>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###BOOL_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		return ImGui::Checkbox("", static_cast<bool*>(data));
		});
}

// Base function to display a char in the editor inspector
template <>
bool GUI::DrawFundamental<char>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###CHAR_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		char& charData = *static_cast<char*>(data);
		char c[] = { charData, '\0' };

		bool out = ImGui::InputText("", c, 2);

		if (out) charData = c[0];

		return out;
		});
}

// Base function to display a short in the editor inspector
template <>
bool GUI::DrawFundamental<short>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###SHORT_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		short min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<short>::lowest(), std::numeric_limits<short>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_S16, static_cast<short*>(data), integerStep, &min, &max, "%d", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a unsigned short in the editor inspector
template <>
bool GUI::DrawFundamental<unsigned short>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###USHORT_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		unsigned short min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<unsigned short>::lowest(), std::numeric_limits<unsigned short>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_U16, static_cast<unsigned short*>(data), integerStep, &min, &max, "%u", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display an int in the editor inspector
template <>
bool GUI::DrawFundamental<int>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###INT_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		int min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_S32, static_cast<int*>(data), integerStep, &min, &max, "%d", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display an unsigned int in the editor inspector
template <>
bool GUI::DrawFundamental<unsigned int>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###UINT_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		unsigned int min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<unsigned int>::lowest(), std::numeric_limits<unsigned int>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_U32, static_cast<unsigned int*>(data), integerStep, &min, &max, "%u", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a long long in the editor inspector
template <>
bool GUI::DrawFundamental<long long>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###LONGLONG_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		long long min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<long long>::lowest(), std::numeric_limits<long long>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_S64, static_cast<long long*>(data), integerStep, &min, &max, "%d", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a unsigned long long in the editor inspector
template <>
bool GUI::DrawFundamental<unsigned long long>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###ULONGLONG_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		unsigned long long min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<unsigned long long>::lowest(), std::numeric_limits<unsigned long long>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_U64, static_cast<unsigned long long*>(data), integerStep, &min, &max, "%u", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a float in the editor inspector
template <>
bool GUI::DrawFundamental<float>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###FLOAT_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		float min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_Float, static_cast<float*>(data), decimalStep, &min, &max, "%.5f", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a double in the editor inspector
template <>
bool GUI::DrawFundamental<double>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###DOUBLE_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		double min, max;
		SetMinMaxFromProperties(min, max, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), propertiesData);

		return ImGui::DragScalar("", ImGuiDataType_Double, static_cast<double*>(data), decimalStep, &min, &max, "%.5f", ImGuiSliderFlags_AlwaysClamp);
		});
}

// Base function to display a HyString in the editor inspector
template <>
bool GUI::DrawFundamental<HyString>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###HYSTRING_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		HyString& str = *static_cast<HyString*>(data);
		char buffer[String_Max_Buffer_Size] = "\0";

		std::string str2;
		memcpy(buffer, str.data(), str.size());

		bool hasInput = ImGui::InputText("", buffer, String_Max_Buffer_Size);
		if (hasInput)
			str = HyString(buffer);

		return hasInput;
		});
}

// Base function to display a Color3 in the editor inspector
template <>
bool GUI::DrawFundamental<Color3>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###COLOR3_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		return ImGui::ColorEdit3("", static_cast<float*>(data));
		});
}

// Base function to display a Color4 in the editor inspector
template <>
bool GUI::DrawFundamental<Color4>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###COLOR4_", propertiesData, [&]() {

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		return ImGui::ColorEdit4("", static_cast<float*>(data));
		});
}

// Base function to display a BoolVector3 in the editor inspector
template <>
bool GUI::DrawFundamental<BoolVector3>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###AXISBOOL_", propertiesData, [&]() {

		bool out = false;

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x);
		{
			BoolVector3& vect = *static_cast<BoolVector3*>(data);

			out |= ImGui::Checkbox("X", &(vect.X));
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::Checkbox("Y", &(vect.Y));
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::Checkbox("Z", &(vect.Z));
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Vector2 in the editor inspector
template <>
bool GUI::DrawFundamental<Vector2>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###VECTOR2_", propertiesData, [&]() {

		bool out = false;

		ImGui::PushMultiItemsWidths(2, ImGui::GetContentRegionAvail().x);
		{
			Vector2& vect = *static_cast<Vector2*>(data);

			out |= ColoredFloatDrag("###X", vect.x, EditorContext::Instance().theme->m_colorX, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Y", vect.y, EditorContext::Instance().theme->m_colorY, propertiesData);
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Vector3 in the editor inspector
template <>
bool GUI::DrawFundamental<Vector3>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###VECTOR3_", propertiesData, [&]() {

		bool out = false;

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x);
		{
			Vector3& vect = *static_cast<Vector3*>(data);

			out |= ColoredFloatDrag("###X", vect.x, EditorContext::Instance().theme->m_colorX, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Y", vect.y, EditorContext::Instance().theme->m_colorY, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Z", vect.z, EditorContext::Instance().theme->m_colorZ, propertiesData);
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Vector4 in the editor inspector
template <>
bool GUI::DrawFundamental<Vector4>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###VECTOR4_", propertiesData, [&]() {

		bool out = false;

		ImGui::PushMultiItemsWidths(4, ImGui::GetContentRegionAvail().x);
		{
			Vector4& vect = *static_cast<Vector4*>(data);
			float x = ImGui::GetContentRegionAvail().x;

			out |= ColoredFloatDrag("###X", vect.x, EditorContext::Instance().theme->m_colorX, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Y", vect.y, EditorContext::Instance().theme->m_colorY, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Z", vect.z, EditorContext::Instance().theme->m_colorZ, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###W", vect.w, EditorContext::Instance().theme->m_colorW, propertiesData);
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Quaternion in the editor inspector
template <>
bool GUI::DrawFundamental<Quaternion>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###QUATERNION_", propertiesData, [&]() {

		bool out = false;

		ImGui::PushMultiItemsWidths(4, ImGui::GetContentRegionAvail().x);
		{
			Quaternion& quat = *static_cast<Quaternion*>(data);

			out |= ColoredFloatDrag("###X", quat.x, EditorContext::Instance().theme->m_colorX, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Y", quat.y, EditorContext::Instance().theme->m_colorY, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###Z", quat.z, EditorContext::Instance().theme->m_colorZ, propertiesData);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ColoredFloatDrag("###W", quat.w, EditorContext::Instance().theme->m_colorW, propertiesData);
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Matrix3 in the editor inspector
template <>
bool GUI::DrawFundamental<Matrix3>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###MATRIX3_", propertiesData, [&]() {

		bool out = false;

		float width = ImGui::GetContentRegionAvail().x;
		Matrix3& mat = *static_cast<Matrix3*>(data);

		std::string ID = "###Matrix3";

		for (int i = 0; i < 9; i += 3)
		{
			ImGui::PushMultiItemsWidths(3, width);

			out |= ImGui::DragFloat((ID + std::to_string(i + 0)).c_str(), &mat.elements[i + 0], decimalStep);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::DragFloat((ID + std::to_string(i + 1)).c_str(), &mat.elements[i + 1], decimalStep);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::DragFloat((ID + std::to_string(i + 2)).c_str(), &mat.elements[i + 2], decimalStep);
			ImGui::PopItemWidth();
		}

		return out;
		});
}

// Base function to display a Matrix4 in the editor inspector
template <>
bool GUI::DrawFundamental<Matrix4>(const char* label, void* data, GUI::InspectPropertiesData const& propertiesData)
{
	return DrawFieldCustom(label, "###MATRIX4_", propertiesData, [&]() {

		bool out = false;

		float width = ImGui::GetContentRegionAvail().x;
		Matrix4& mat = *static_cast<Matrix4*>(data);

		std::string ID = "###Matrix4";

		for (int i = 0; i < 16; i += 4)
		{
			ImGui::PushMultiItemsWidths(4, width);
			ImGui::PushID(i);

			out |= ImGui::DragFloat((ID + std::to_string(i + 0)).c_str(), &mat.elements[i + 0], decimalStep);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::DragFloat((ID + std::to_string(i + 1)).c_str(), &mat.elements[i + 1], decimalStep);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::DragFloat((ID + std::to_string(i + 2)).c_str(), &mat.elements[i + 2], decimalStep);
			ImGui::PopItemWidth();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			out |= ImGui::DragFloat((ID + std::to_string(i + 3)).c_str(), &mat.elements[i + 3], decimalStep);
			ImGui::PopItemWidth();

			ImGui::PopID();
		}

		return out;
		});
}


void GUI::LaunchGUI(Window* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigViewportsNoDecoration = false;

	ImGui_ImplGlfw_InitForOpenGL(window->GetInstance(), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}


void GUI::ShutDownGUI()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();

	ImGui::DestroyContext();
}


void GUI::NewFrameGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void GUI::RenderGUI()
{
	ImGui::Render(); 
	
	glViewport(0, 0, EditorContext::Instance().window->GetWidth(), EditorContext::Instance().window->GetHeight());
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}


void GUI::PushColor(const int target, const Color4& color)
{
	ImGui::PushStyleColor(target, { color.r,color.g,color.b,color.a });
}


void GUI::PopColor(const int count)
{
	ImGui::PopStyleColor(count);
}


void GUI::PushStyle(const int target, const float value)
{
	ImGui::PushStyleVar(target, value);
}


void GUI::PushStyle(const int target, const ImVec2& value)
{
	ImGui::PushStyleVar(target, value);
}


void GUI::PopStyle(const int count)
{
	ImGui::PopStyleVar(count);
}


void* GUI::CastTextureID(const unsigned int textureID)
{
	return reinterpret_cast<void*>(static_cast<size_t>(textureID));
}


bool GUI::ColoredFloatDrag(const char* ID, float& outValue, const Color4& color, GUI::InspectPropertiesData const& propertiesData)
{
	bool out = false;

	//	Get Format

	float min, max;
	SetMinMaxFromProperties(min, max, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), propertiesData);

	int n = 0;
	int maxNum = 5;

	float number = fabsf(outValue);
	number = number - static_cast<int>(number);

	{
		float power = powf(10.f, static_cast<float>(maxNum));
		number = roundf(number * power) / power;
	}

	while (number != 0 && n < maxNum)
	{
		number *= 10;
		number -= (int)number;

		float power = powf(10.f, static_cast<float>(maxNum - n));

		//	Cut last decimals
		number = roundf(number * power) / (power);

		n++;
	}

	std::string format = "%.1f";
	if (n > 1)
	{
		int checkpoint = 0;
		format = "%." + std::to_string(n) + "f";
	}

	{
		GUI::PushColor(ImGuiCol_Border, color);

		out |= ImGui::DragScalar(ID, ImGuiDataType_Float, &outValue, decimalStep, &min, &max, format.c_str(), ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);

		GUI::PopColor(1);
	}

	return out;
}


void GUI::DelayedTooltip(const char* text, float delay)
{
	if (ImGui::IsItemHovered() && ImGui::GetCurrentContext()->HoveredIdTimer > delay)
	{
		ImGui::BeginTooltip();

		ImGui::Text(text);

		ImGui::EndTooltip();
	}
}