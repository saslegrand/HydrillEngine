#include "EditorApp.hpp"

#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(197274);

	{
		EditorApp app;

		app.Initialize();
		app.Loop();
		app.Shutdown();
	}

	return 0;
}
