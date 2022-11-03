#include "io/Window.hpp"

#include <iostream>
#include <wtypes.h>
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window(int windowWidth, int windowHeight, const char* windowName) : m_contentWidth(windowWidth), m_contentHeight(windowHeight), m_name(windowName)
{
    //  GLFW initialization

    if (!glfwInit())
    {
        std::cout << "glfwInit failed" << std::endl;
        return;
    }

    // Target OpenGL 4.5 API in Compatibility mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //  Window Creation

    m_GLFWInstance = glfwCreateWindow(m_contentWidth, m_contentHeight, m_name, nullptr, nullptr);

    if (m_GLFWInstance == nullptr)
    {
        std::cout << "glfwCreateWindow failed" << std::endl;
        return;
    }

    //  Load GL

    glfwMakeContextCurrent(m_GLFWInstance);

    if (gladLoaderLoadGL() == 0)
    {
        std::cout << "gladLoaderLoadGL failed" << std::endl;
        return;
    }

    Initialize();
}



Window::Window(const char* windowName) : m_name(windowName)
{
    //  GLFW initialization

    if (!glfwInit())
    {
        std::cout << "glfwInit failed" << std::endl;
        return;
    }

    // Target OpenGL 4.5 API in Compatibility mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    
    //  Window Creation

    GetMonitorDimension(m_contentWidth, m_contentHeight);
    m_GLFWInstance = glfwCreateWindow(m_contentWidth, m_contentHeight, m_name, nullptr, nullptr);
    glfwMaximizeWindow(m_GLFWInstance);

    if (m_GLFWInstance == nullptr)
    {
        std::cout << "glfwCreateWindow failed" << std::endl;
        return;
    }

    //  Load GL

    glfwMakeContextCurrent(m_GLFWInstance);

    if (gladLoaderLoadGL() == 0)
    {
        std::cout << "gladLoaderLoadGL failed" << std::endl;
        return;
    }
    
    Initialize();
}

Window::~Window()
{
    if (m_GLFWInstance)
    {
        glfwDestroyWindow(m_GLFWInstance);
    }

    glfwTerminate();
}



void Window::Initialize()
{
    glfwSetWindowUserPointer(m_GLFWInstance, this);
    glfwGetWindowPos(m_GLFWInstance, &m_posX, &m_posY);
    glfwGetWindowSize(m_GLFWInstance, &m_contentWidth, &m_contentHeight);
    
    InitializeCallbacks();

    RECT windowRect;
    HWND win = glfwGetWin32Window(m_GLFWInstance);

    GetWindowRect(win, &windowRect);
    m_width = windowRect.right - windowRect.left;
    m_height = windowRect.bottom - windowRect.top;

    m_titleBarPadding = m_height - m_contentHeight;

    glfwSwapInterval(0);
}


bool Window::ShouldWindowLoop()
{
    return !glfwWindowShouldClose(m_GLFWInstance);
}


void Window::FrameStart()
{
    glfwPollEvents();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Window::FrameEnd()
{
    glfwSwapBuffers(m_GLFWInstance);
}


void Window::GetMonitorDimension(int& outWidth, int& outHeight)
{
    RECT desktop;

    const HWND hDesktop = GetDesktopWindow();

    GetWindowRect(hDesktop, &desktop);

    outWidth = desktop.right;
    outHeight = desktop.bottom;
}


void Window::InitializeCallbacks()
{

#pragma region window callbacks

    //  Window resize callback
    //glfwSetWindowSizeCallback(m_GLFWInstance, Window::CallbackFrameBufferResize);
    glfwSetFramebufferSizeCallback(m_GLFWInstance, Window::CallbackFrameBufferResize);
    
    //  Window move callback
    glfwSetWindowPosCallback(m_GLFWInstance, Window::CallbackWindowMove);

    //  Window refresh callback
    glfwSetWindowRefreshCallback(m_GLFWInstance, Window::CallbackWindowRefresh);

    //  Window focus callback
    glfwSetWindowFocusCallback(m_GLFWInstance, Window::CallbackWindowFocus);

    //  Window close callback
    glfwSetWindowCloseCallback(m_GLFWInstance, Window::CallbackWindowClose);

    //  Window content scale callback
    glfwSetWindowContentScaleCallback(m_GLFWInstance, Window::CallbackWindowContentScale);

    //  Window iconify callback
    glfwSetWindowIconifyCallback(m_GLFWInstance, Window::CallbackWindowIconify);

    //  Window iconify callback
    glfwSetWindowMaximizeCallback(m_GLFWInstance, Window::CallbackWindowMaximize);

#pragma endregion

#pragma region input callbacks

    //  INPUT CALLBACKS
    
    //  Key callback
    glfwSetKeyCallback(m_GLFWInstance, Window::CallbackKey);

    //  Mouse button callback
    glfwSetMouseButtonCallback(m_GLFWInstance, Window::CallbackMouseButton);

    //  Mouse Scroll callback
    glfwSetScrollCallback(m_GLFWInstance, Window::CallbackScroll);

    //  Mouse Pos callback
    glfwSetCursorPosCallback(m_GLFWInstance, Window::CallbackMousePos);

    //  Cursor Enter callback
    glfwSetCursorEnterCallback(m_GLFWInstance, Window::CallbackCursorEnter);

#pragma endregion
}


bool Window::IsWindowOpen(const char* windowName)
{
    return FindWindowA(NULL, windowName) != NULL;
}

void Window::CloseWindow()
{
    glfwSetWindowShouldClose(m_GLFWInstance, GLFW_TRUE);
}

//  Callbacks functions
//  -------------------


#pragma region window callbacks

void Window::CallbackFrameBufferResize(GLFWwindow* windowInstance, int width, int height)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->resized  = true;
    window->m_contentWidth  = width;
    window->m_contentHeight = height;

    RECT windowRect;
    HWND win = glfwGetWin32Window(windowInstance);

    GetWindowRect(win, &windowRect);
    window->m_width  = windowRect.right - windowRect.left;
    window->m_height = windowRect.bottom - windowRect.top;

    window->m_titleBarPadding = window->m_height - window->m_contentWidth;

    window->resizeEvent.Invoke();
}

void Window::CallbackWindowMove(GLFWwindow* windowInstance, int x, int y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->m_posX = x;
    window->m_posY = y;

    window->moveEvent.Invoke();
}

void Window::CallbackWindowRefresh(GLFWwindow* windowInstance)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->refreshEvent.Invoke();
}

void Window::CallbackWindowFocus(GLFWwindow* windowInstance, int focus)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));


    if (focus) window->focusEvent.Invoke();
    else window->unfocusEvent.Invoke();

    window->m_focused = focus;
}

void Window::CallbackWindowClose(GLFWwindow* windowInstance)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->closeEvent.Invoke();
}

void Window::CallbackWindowContentScale(GLFWwindow* windowInstance, float x, float y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
}

void Window::CallbackWindowIconify(GLFWwindow* windowInstance, int iconified)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->minimized = static_cast<bool>(iconified);

    window->iconifyEvent.Invoke();
}

void Window::CallbackWindowMaximize(GLFWwindow* windowInstance, int maximized)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));

    window->maximizeEvent.Invoke();
}

#pragma endregion


#pragma region input callbacks

void Window::CallbackKey(GLFWwindow* windowInstance, const int key, const  int scancode, const  int action, const  int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
    window->keyEvent.Invoke(key, action);
}

void Window::CallbackMouseButton(GLFWwindow* windowInstance, const int button, const  int action, const  int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
    window->mouseButtonEvent.Invoke(button, action);
}

void Window::CallbackMousePos(struct GLFWwindow* windowInstance, const double x, const  double y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
    window->mousePosEvent.Invoke(static_cast<float>(x), static_cast<float>(y));
}

void Window::CallbackScroll(GLFWwindow* windowInstance, const double x, const  double y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
    window->mouseScrollEvent.Invoke(static_cast<float>(x), static_cast<float>(y));
}

void Window::CallbackCursorEnter(GLFWwindow* windowInstance, const int enter)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(windowInstance));
}

#pragma endregion