#pragma once

#include <Renderer/RenderpassParameters.hpp>

#include "Widgets/RenderWidget.hpp"

/**
@brief This is a render widget which will display the game
*/
class RenderGameWidget : public RenderWidget
{

//	Variables

private:

	float m_width  = 1920.f;
	float m_height = 1024.f;
	float m_ratio = 0.0f;

	RenderpassParameters m_gameRenderParameters;


//	Constructor(s) & Destructor(s)
	
public:

	RenderGameWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);

//	Functions

public:

	/**
	@brief Only called if the widget is active and then call Update framebuffer
	*/
	void Update() override;

	/**
	@brief Widget display function
	*/
	void Display() override;

	/**
	@brief Update the framebuffer and render the scene inside
	*/
	void RenderFramebuffer() override;

};