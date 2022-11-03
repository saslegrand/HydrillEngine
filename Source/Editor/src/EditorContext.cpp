#include "EditorContext.hpp"

#include "IO/Window.hpp"

EditorContext* EditorContext::instance = nullptr;

EditorContext& EditorContext::Instance()
{
	return *instance;
}

int EditorContext::GetWindowWidth() const
{
	return instance->window->GetWidth();
}

int EditorContext::GetWindowHeight() const
{
	return instance->window->GetWidth();
}