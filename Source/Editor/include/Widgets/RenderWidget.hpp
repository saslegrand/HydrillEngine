#pragma once

#include "Widgets/EditorWidget.hpp"
#include "Renderer/Primitives/FrameBuffer.hpp"

#include <Maths/Vector2.hpp>

/**
@brief This is the parent widget for all render widget (Scene and Game)
Manage the displaying of the image sent to it
*/
class RenderWidget : public EditorWidget
{

//	Variables

protected:

	FrameBuffer m_framebuffer;

	bool m_captured = false;
	bool m_firstFrameFocus = false;

//	Constructor(s) & Destructor(s)


public:

	RenderWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false, const char* typeID = "###RENDER");
	virtual ~RenderWidget() {};

//	Functions

private:

	/**
	@brief Update the framebuffer and render inside it
	*/
	virtual void RenderFramebuffer() = 0;

protected:

	/**
	@brief Usually called at the beginning of each widget display functions.
	This function manage the creation of the window
	WARNING : must place EndWidget at the end

	@return Return true if the widget is open
	*/
	virtual bool BeginWidget() override;

	/**
	@brief Usually called at the end of each widget display functions.
	WARNING : Must be placed when a BeginWidget has been placed
	*/
	virtual void ParameterBar() {};

	/**
	@brief Usually called at the beginning of each render widget display functions, just after BeginWidget function was called.
	WARNING : must place EndRenderChildWindow at the end

	@return Return true if the widget is open
	*/
	bool BeginRenderChildWindow();

	/**
	@brief Usually called at the end of render widget display functions.
	WARNING : Must be placed when a BeginRenderChildWindow has been placed
	*/
	void EndRenderChildWindow();

public:

	/**
	@brief Widget display function
	*/
	virtual void Update() override;
};