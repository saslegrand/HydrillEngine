#include "Widgets/ConsoleWidget.hpp"

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include <EngineContext.hpp>
#include <EditorContext.hpp>
#include <Core/Logger.hpp>
#include <Tools/Event.hpp>
#include <IO/Mouse.hpp>
#include <Core/Time.hpp>

#include "GUITypes.hpp"
#include "GUI.hpp"

ConsoleWidget::ConsoleWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: EditorWidget(widgetName, widgetID, isActive, "###CONSOLE")
{
	m_flag |= ImGuiWindowFlags_NoScrollbar;
	m_maskType = static_cast<int>(MessageType::Success) | static_cast<int>(MessageType::Info) | static_cast<int>(MessageType::Error) | static_cast<int>(MessageType::Warning);
	m_printCallbackID = EngineContext::Instance().logger->onPrintEvent.AddCallback(&ConsoleWidget::LogPrint, *this);


	m_followingScrollModeTexture = EditorContext::Instance().images->LoadImage(R"(Icons\FollowingScroll.texture)");
	m_showDateTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Clock.texture)");
	m_clearTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Trash.texture)");
	m_infoTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Info.texture)");
	m_successTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Success.texture)");
	m_warningTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Warning.texture)");
	m_errorTexture = EditorContext::Instance().images->LoadImage(R"(Icons\Error.texture)");
}

ConsoleWidget::~ConsoleWidget()
{
	EngineContext::Instance().logger->onPrintEvent.RemoveCallback(m_printCallbackID);
}


void ConsoleWidget::Update()
{
	float deltaTime = EngineContext::Instance().time->GetUnscaledDeltaTime();
	m_elapsedTime += deltaTime;

	//	Update warning and error button colors

	//	If has warning make the color blink
	if (m_warningCount > 0)
	{
		float deltaLerp = 0.5f + Maths::Sin(m_elapsedTime * 2.5f) * 0.5f;

		m_warningButtonColor = static_cast<int>(MessageType::Warning) & m_maskType ?
			Color4::Lerp(Color4::DarkYellow, Color4::Yellow, deltaLerp) :
			Color4::Lerp(Color4::DarkYellow, m_inactiveButtonColor, deltaLerp);
	}
	else
	{
		m_warningButtonColor = static_cast<int>(MessageType::Warning) & m_maskType ? Color4::DarkYellow : m_inactiveButtonColor;
	}

	//	If has error make the color blink
	if (m_errorCount > 0)
	{
		float deltaLerp = 0.5f + Maths::Sin(m_elapsedTime * 5.f) * 0.5f;
		m_errorButtonColor = static_cast<int>(MessageType::Error) & m_maskType ?
			Color4::Lerp(Color4::DarkRed, Color4::LightRed, deltaLerp) :
			Color4::Lerp(Color4::DarkRed, m_inactiveButtonColor, deltaLerp);
	}
	else
	{
		m_errorButtonColor = static_cast<int>(MessageType::Error) & m_maskType ? Color4::DarkRed : m_inactiveButtonColor;
	}

	m_infoButtonColor = static_cast<int>(MessageType::Info) & m_maskType ? Color4::Blue : m_inactiveButtonColor;
	m_successButtonColor = static_cast<int>(MessageType::Success) & m_maskType ? Color4::Green : m_inactiveButtonColor;
}

void ConsoleWidget::Display()
{
	if (!isActive) return;

	if (BeginWidget())
	{
		Options();

		Content();
	}
	EndWidget();
}


void ConsoleWidget::MessageTypeButton(unsigned int textureID, const MessageType& type,float size, Color4 color)
{
	ImVec4 tint;
	memcpy(&tint, &color, sizeof(ImVec4));

	bool clicked = ImGui::ImageButton(GUI::CastTextureID(textureID), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), -1, ImVec4(0, 0, 0, 0), tint);

	if (clicked)
	{
		int intType = static_cast<int>(type);
		bool bInfo = (intType & m_maskType);

		m_maskType = !bInfo ? m_maskType | intType : m_maskType & ~intType;
	}
}

void ConsoleWidget::Options()
{
	if (ImGui::Button("Test"))
	{
		for (size_t i = 0; i < 10000; i++)
		{
			Logger::Error("This is an error");
			Logger::Warning("This is a warning");
			Logger::Success("This is a success");
			Logger::Info("This is an info");
		}
	}

	float height = ImGui::GetFontSize() * 1.5f;
	ImVec2 dim = ImVec2(height, height);
	ImVec4 backgroundImage = ImVec4(0, 0, 0, 0);

	GUI::PushColor(ImGuiCol_Button, Color4(0, 0, 0, 0));
	GUI::PushColor(ImGuiCol_ButtonHovered, Color4(0, 0, 0, 25));
	GUI::PushColor(ImGuiCol_ButtonActive, Color4(0, 0, 0, 50));

	{
		ImVec4 tint;
		if (m_lines.empty()) memcpy(&tint, &m_inactiveButtonColor, sizeof(ImVec4));
		else memcpy(&tint, &m_defaultButtonColor, sizeof(ImVec4));

		ImGui::SameLine(0.0f, 10.f);
		if (ImGui::ImageButton(GUI::CastTextureID(m_clearTexture), dim, ImVec2(0, 1), ImVec2(1, 0), -1, backgroundImage, tint))
		{
			m_lines.clear();
			m_warningCount = m_errorCount = 0;
		}
		GUI::DelayedTooltip("Clear the console of all the current messages");
	}

	{
		ImVec4 tint;
		if (!m_followingScrollMode) memcpy(&tint, &m_inactiveButtonColor, sizeof(ImVec4));
		else memcpy(&tint, &m_defaultButtonColor, sizeof(ImVec4));

		ImGui::SameLine(0.0f, 10.f);
		if (ImGui::ImageButton(GUI::CastTextureID(m_followingScrollModeTexture), dim, ImVec2(0, 1), ImVec2(1, 0), -1, backgroundImage, tint))
		{
			m_followingScrollMode = !m_followingScrollMode;
		}
		GUI::DelayedTooltip("Disable / enable scroll following,\nif you scroll to the bottom using your mouse scroll button,\n the following scroll mode will be enabled automatically");
	};

	{
		ImVec4 tint;
		if (!m_showDate) memcpy(&tint, &m_inactiveButtonColor, sizeof(ImVec4));
		else memcpy(&tint, &m_defaultButtonColor, sizeof(ImVec4));

		ImGui::SameLine(0.0f, 10.f);
		if (ImGui::ImageButton(GUI::CastTextureID(m_showDateTexture), dim, ImVec2(0, 1), ImVec2(1, 0), -1, backgroundImage, tint))
		{
			m_showDate = !m_showDate;
		}
		GUI::DelayedTooltip("Show / hide dates in the console messages");
	}

	ImGui::SameLine(0.0f, 50.f);

	{
		MessageTypeButton(m_infoTexture, MessageType::Info, dim.x, m_infoButtonColor);
		GUI::DelayedTooltip("Show / hide information messages in the console");

		ImGui::SameLine(0.0f, 10.f);
		MessageTypeButton(m_successTexture, MessageType::Success, dim.x, m_successButtonColor);
		GUI::DelayedTooltip("Show / hide success messages in the console");

		ImGui::SameLine(0.0f, 10.f);
		MessageTypeButton(m_warningTexture, MessageType::Warning, dim.x, m_warningButtonColor);
		GUI::DelayedTooltip("Show / hide warning messages in the console\nIf the warning button is blinking, it means warning messages were printed.");

		ImGui::SameLine(0.0f, 10.f);
		MessageTypeButton(m_errorTexture, MessageType::Error, dim.x, m_errorButtonColor);
		GUI::DelayedTooltip("Show / hide error messages in the console\nIf the error button is blinking, it means error messages were printed.");
	}
	GUI::PopColor(3);
}

void ConsoleWidget::Content()
{
	EditorContext::Instance().theme->consoleFont->UseRegular();

	GUI::PushColor(ImGuiCol_ChildBg, Color4(0, 0, 0, 50));
	ImGui::BeginChild("ConsoleDisplayer",ImVec2(0,0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
	GUI::PopColor();


	ImGuiListClipper clipper;

	int lineCount = static_cast<int>(m_lines.size());

	clipper.Begin(lineCount);

	bool overflowed = false;
	while (!overflowed && clipper.Step())
	{
		int currentLine = clipper.DisplayStart;
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			while (currentLine < lineCount && !(m_lines[currentLine].type & m_maskType))
			{
				currentLine++;
			}

			if (currentLine >= lineCount)
			{
				overflowed = true;
				break;
			}
			if (m_showDate)
			{
				GUI::PushColor(ImGuiCol_Text, Color4::Gray);
				ImGui::Text(m_lines[currentLine].date.c_str());
				GUI::PopColor(1);

				ImGui::SameLine();
			}

			GUI::PushColor(ImGuiCol_Text, m_lines[currentLine].color);
			ImGui::TextWrapped(m_lines[currentLine].text.c_str());
			GUI::PopColor(1);

			currentLine++;
		}
	}

	//	If scrolled
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && Input::Mouse::GetDeltaScroll().y != 0.0f)
	{
		//	If scrolled to the bottom then autoActivate scroll to botom else disable scrollToBottom
		m_followingScrollMode = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
	}
	if (m_followingScrollMode && m_updated)
	{
		ImGui::SetScrollHereY(1.f);
	}

	ImGui::EndChild();

	EditorContext::Instance().theme->consoleFont->EndFontStyle();
}

void ConsoleWidget::LogPrint(const std::string& date, const std::string& line, const int type)
{
	Color4 col = Color4::White;

	switch (type)
	{
	case static_cast<int>(MessageType::Success): col = Color4::DarkGreen; break;
	case static_cast<int>(MessageType::Warning): col = Color4::Orange; m_warningCount++; break;
	case static_cast<int>(MessageType::Error):   col = Color4::DarkRed; m_errorCount++; break;
	default: break;
	}

	/*if (m_lines.size() > 1000)
	{
		m_lines.erase(m_lines.begin(), m_lines.begin() + 100);
	}*/

	m_lines.emplace_back(ConsoleLine(date, line, col, type));
	m_updated = true;
}
