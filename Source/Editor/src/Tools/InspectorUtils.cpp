#include "Tools/InspectorUtils.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Core/BaseObject.hpp>
#include <Resources/Resource/Resource.hpp>
#include <Tools/ReflectedSTD.hpp>
#include <ECS/GameObject.hpp>

#include "EditorContext.hpp" 
#include "EditorTheme.hpp" 
#include "GUI.hpp"

bool InspectorMethods::DisplayEntity(BaseObject* object, rfk::Class const* rfkClass)
{
	// Retrieve all the object fields
	auto fields = rfkClass->getFieldsByPredicate([](rfk::Field const& field, void* data) {
		return true;
		}, nullptr, true, true);

	bool isModified = false;

	EditorTheme* theme = EditorContext::Instance().theme;

	for (auto field : fields)
	{
		// Check if the object has a Hide property attached (discard if true)
		if (field->getProperty(PHide::staticGetArchetype()))
			continue;

		// Draw a Header if attached
		if (rfk::Property const* prop = field->getProperty(PHeader::staticGetArchetype()))
		{
			PHeader const& header = *static_cast<PHeader const*>(prop);
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
			ImVec4 col; memcpy(&col, &theme->color_interactives.defaultColor, sizeof(Color4));
			theme->defaultFont->UseBold();
			ImGui::TextColored(col, header.GetTitle().c_str());
			theme->defaultFont->EndFontStyle();
			ImGui::Spacing();
		}

		// Get all the other object properties (handled in GUI.cpp file)
		size_t propertiesCount = field->getPropertiesCount();
		PropertyContainer properties(propertiesCount);
		for (size_t indexProp = 0; indexProp < propertiesCount; indexProp++)
		{
			rfk::Property const* prop = field->getPropertyAt(indexProp);
			properties.insert({ prop->getArchetype().getId(), prop });
		}

		// Create the field data structure
		GUI::InspectFieldData fieldData = {
			.ownerClass = rfkClass,
			.ownerObject = object,
			.name = field->getName(),
			.arch = field->getType().getArchetype(),
			.data = field->getPtr(*object),
			.isPointer = field->getType().isPointer()
		};

		// Display the field object
		isModified |= InspectorMethods::DisplayObject(fieldData, properties);
	}

	return isModified;
}

bool InspectorMethods::DisplayObject(GUI::InspectFieldData& fieldData, PropertyContainer& properties)
{
	// Check if the archetype is valid (if the field is reflected)
	if (fieldData.arch == nullptr)
		return false;

	// Check if the value is a fundamental type
	bool fieldModified = false;
	if (RFKInspectObject::TryInspectFundamental(fieldData, properties, fieldModified))
		return fieldModified;

	// Retrieve the class archetype from its id (bug with template entities, the archetype is invalid)
	rfk::Archetype const* classArch = rfk::getDatabase().getClassById(fieldData.arch->getId());

	// Check if the field is a template class
	rfk::ClassTemplateInstantiation const* cTempInstance = rfk::classTemplateInstantiationCast(classArch);
	if (cTempInstance)
		return InspectorMethods::DisplayTemplateObject(fieldData, cTempInstance, properties);

	// Check if the field is an enum
	if (RFKInspectObject::TryInspectEnum(fieldData, properties, fieldModified))
		return fieldModified;

	// Check if the field is a class/struct
	rfk::Class const* fieldClass = rfk::classCast(fieldData.arch);
	if (fieldClass == nullptr)
	{
		fieldClass = rfk::structCast(fieldData.arch);
		if (fieldClass == nullptr)
			return false;
	}

	// If not a pointer, inspect the inside of the field
	// Else check for a potential SceneObject reference
	if (!fieldData.isPointer)
		return RFKInspectObject::TryDisplayEntity(fieldData, fieldClass, properties);

	// Check if the field is a SceneObject reference
	if (RFKInspectObject::TryInspectReference(fieldData, fieldClass, properties, fieldModified))
		return fieldModified;

	// Class not supported
	return false;
}

bool InspectorMethods::DisplayTemplateObject(GUI::InspectFieldData& fieldData, rfk::ClassTemplateInstantiation const* cTempInstance, PropertyContainer& properties)
{
	// Display a vector
	if (cTempInstance->hasSameName("HyVector"))
	{
		// Find type and archetype of the template instantiation
		rfk::TemplateArgument const& tempArg = cTempInstance->getTemplateArgumentAt(0);
		rfk::TypeTemplateArgument const& typeTempArg = *static_cast<const rfk::TypeTemplateArgument*>(&tempArg);
		rfk::Type const& typeTemp = typeTempArg.getType();
		rfk::Archetype const* valueTypeArch = typeTemp.getArchetype();

		// Check if the template type is correctly reflected
		if (valueTypeArch == nullptr)
			return false;

		// Get vetor utils methods from the template archetype
		std::size_t vecSize = cTempInstance->getMethodByName("size")->invokeUnsafe<std::size_t>(fieldData.data);
		rfk::Method const* getMethod = cTempInstance->getMethodByName("GetElementAt");

		bool isModified = false;
		bool opened = false;

		if (GUI::BeginField("###HYVECTOR_FIELD"))
		{
			GUI::FieldLabel(fieldData.name.data());

			GUI::BeginFieldContent();

			if (ImGui::TreeNodeEx("Elements"))
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

			// Display vector elements
			for (std::size_t vecIndex = 0; vecIndex < vecSize; vecIndex++)
			{
				// Get the element ptr at vecIndex with the archetype method
				void* value = getMethod->invokeUnsafe<void*>(fieldData.data, static_cast<std::size_t>(vecIndex));
				std::string name = "Element" + std::to_string(vecIndex);

				// Create the element field data
				GUI::InspectFieldData vecFieldData = {
					.ownerClass = fieldData.ownerClass,
					.ownerObject = fieldData.ownerObject,
					.name = name,
					.arch = valueTypeArch,
					.data = value,
					.isPointer = typeTemp.isPointer()
				};

				// Display the element
				isModified |= InspectorMethods::DisplayObject(vecFieldData, properties);

				std::string selfName = fieldData.name.data() + name;
				ImGui::PushID(selfName.c_str());

				ImGui::BeginDisabled(vecIndex == 0);
				if (ImGui::ArrowButton("###SWAP_UP", ImGuiDir_Up))
				{
					// Swap with the previous element
					rfk::Method const* swapMethod = cTempInstance->getMethodByName("SwapSigned");
					swapMethod->invokeUnsafe<void>(fieldData.data, std::move(vecIndex), -1);
					isModified = true;
				}
				ImGui::EndDisabled();

				ImGui::SameLine();

				ImGui::BeginDisabled(vecIndex == vecSize - 1);
				if (ImGui::ArrowButton("###SWAP_DOWN", ImGuiDir_Down))
				{
					// Swap with the next element
					rfk::Method const* swapMethod = cTempInstance->getMethodByName("SwapSigned");
					swapMethod->invokeUnsafe<void>(fieldData.data, std::move(vecIndex), 1);
					isModified = true;
				}
				ImGui::EndDisabled();

				ImGui::SameLine();

				ImGui::SetCursorPosX(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x -
					ImGui::CalcTextSize("Remove").x - ImGui::GetStyle().WindowPadding.x
				);

				if (ImGui::Button("Remove"))
				{
					// Remove the element from the vector
					rfk::Method const* eraseMethod = cTempInstance->getMethodByName("EraseAt");
					eraseMethod->invokeUnsafe<void>(fieldData.data, std::move(vecIndex));
					isModified = true;

					vecIndex--;
					vecSize--;
				}

				ImGui::PopID();

				ImGui::Separator();
			}

			ImGui::Spacing();

			if (ImGui::Button("New element"))
			{
				// Add an element to the vector
				rfk::Method const* getMethod = cTempInstance->getMethodByName("AddNewElement");
				void* value = getMethod->invokeUnsafe<void*>(fieldData.data);
				isModified = true;
			}

			ImGui::EndGroup();
		}

		ImGui::Spacing();

		return isModified;
	}

	// Display a flag
	else if (cTempInstance->hasSameName("Flags"))
	{
		// Draw the 
		return GUI::DrawField<int64_t>(fieldData, properties, GUI::EFieldInspectType::FLAG);
	}

	// Other template (list, array)
	// For amelioration, this should be possible with a custom editor class

	return false;
}
