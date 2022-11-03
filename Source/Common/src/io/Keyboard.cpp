#include <io/Keyboard.hpp>
#include <io/Window.hpp>

Input::Keyboard* Input::Keyboard::m_instance = nullptr;

Input::Keyboard::Keyboard(Window* window)
{
	if (m_instance == nullptr)
	{
		m_instance = this;

		window->keyEvent.AddCallback(&Keyboard::OnKeyEvent, this);
	}
}


void Input::Keyboard::OnKeyEvent(const int key, const int action)
{
	if (queueSize >= MAX_KEYQUEUE)
	{
		//	TODO : Warning Log Message here
		return;
	}

	keyList[key].Update(action);
	keyQueue[queueSize] = &keyList[key];

	queueSize++;
}


void Input::Keyboard::RefreshQueue()
{
	for (int i = 0; i < queueSize; i++)
	{
		keyQueue[i]->Refresh();
		keyQueue[i] = nullptr;
	}

	queueSize = 0;
}
