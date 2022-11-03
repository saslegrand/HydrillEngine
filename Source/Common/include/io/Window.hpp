#pragma once
#include <Tools/Event.hpp>

class Window
{

//	Variables

private:

	int m_contentWidth = 1200;
	int m_contentHeight = 800;

	int m_width  = 0;
	int m_height = 0;

	int m_posX = 0;
	int m_posY = 0;

	int m_titleBarPadding;

	const char* m_name;

	struct GLFWwindow* m_GLFWInstance = nullptr;

	bool m_focused = false;

public:

	bool resized = false;
	bool minimized = false;

	Event<int, int> keyEvent;

	Event<int, int>     mouseButtonEvent;
	Event<float, float> mousePosEvent;
	Event<float, float> mouseScrollEvent;

	Event<> resizeEvent;
	Event<> moveEvent;
	Event<> refreshEvent;
	Event<> focusEvent;
	Event<> unfocusEvent;
	Event<> closeEvent;
	Event<> iconifyEvent;
	Event<> maximizeEvent;
	

//	Constructor(s) & Destructor(s)

public:

	Window(int windowWidth = 1200, int windowHeight = 800, const char* windowName = "GLFW window");
	Window(const char* windowName = "GLFW window");
	~Window();

//	Functions

private:

	/**
	* @brief Initialize the window
	*/
	void Initialize();

	/**
	* @brief Initialize the callbacks
	*/
	void InitializeCallbacks();

	//	Callback functions

	//	Window

	static void CallbackFrameBufferResize(struct GLFWwindow* windowInstance, int width, int height);
	static void CallbackWindowMove(struct GLFWwindow* windowInstance, int x, int y);
	static void CallbackWindowRefresh(struct GLFWwindow* windowInstance);
	static void CallbackWindowFocus(struct GLFWwindow* windowInstance, int focus);
	static void CallbackWindowClose(struct GLFWwindow* windowInstance);
	static void CallbackWindowContentScale(struct GLFWwindow* windowInstance, float x, float y);
	static void CallbackWindowIconify(struct GLFWwindow* windowInstance, int iconified);
	static void CallbackWindowMaximize(struct GLFWwindow* windowInstance, int maximized);

	//	Inputs

	static void CallbackKey(struct GLFWwindow* windowInstance, const int key, const  int scancode, const  int action, const  int mods);
	static void CallbackMouseButton(struct GLFWwindow* windowInstance, const int button, const  int action, const  int mods);
	static void CallbackMousePos(struct GLFWwindow* windowInstance, const double x, const  double y);
	static void CallbackScroll(struct GLFWwindow* windowInstance, const double x, const  double y);
	static void CallbackCursorEnter(struct GLFWwindow* windowInstance, const int enter);

public:

	/**
	@brief Is the window named with the given name is open or not ?

	@param windowName : name of the window we want to check if it's opened or not

	@return wether it is opened or not
	*/
	static bool IsWindowOpen(const char* windowName);

	/**
	@brief Force closing the current window
	*/
	void CloseWindow();

	/**
	* @brief Return if the GLFW should keep looping
	*/
	bool ShouldWindowLoop();

	/**
	* @brief Frame start function, must be placed at the start of the loop
	* Clear the screen and update stocked window dimension if changed
	*/
	void FrameStart();

	/**
	* @brief Frame end function, must be placed at the end of the loop
	*/
	void FrameEnd();

	/**
	* @brief Get monitor dimension in the enterred values
	* @param outWidth : Width to get
	* @param outHeight : Height to get
	*/
	void GetMonitorDimension(int& outWidth, int& outHeight);

	//	Getter functions

	/**
	* @brief Get GLFW window instance
	*/
	struct GLFWwindow* GetInstance() { return m_GLFWInstance; }

	/**
	* @brief Get window width
	*/
	int GetWidth() const { return m_contentWidth; }

	/**
	* @brief Get window height
	*/
	int GetHeight() const { return m_contentHeight; }


	/**
	* @brief Get window X coordinates
	*/
	int GetPosX() const { return m_posX; }

	/**
	* @brief Get window Y coordinates
	*/
	int GetPosY() const { return m_posY; }

	/**
	* @brief Get window name
	*/
	const char* GetName() const { return m_name; }
};
