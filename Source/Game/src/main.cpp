#include "GameApp.hpp"

int main()
{
	{
		GameApp app;

		app.Initialize();
		app.Loop();
		app.Shutdown();
	}

	return 0;
}
