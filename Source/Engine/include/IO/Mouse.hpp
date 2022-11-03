#pragma once

#include <unordered_map>

#include <Refureku/Refureku.h>
#include <nlohmann/json_fwd.hpp>

#include "Maths/Vector2.hpp"
#include "Tools/Event.hpp"
#include "IO/Inputs.hpp"
#include "IO/Button.hpp"
#include "IO/InputAxis.hpp"
#include "EngineDLL.hpp"

#define MAX_MOUSEBUTTONQUEUE 8

class Window;
class InputSystem;

namespace Input
{
	/**
	* Mouse system managing all of the mouse inputs
	*/
	class Mouse
	{
	//	Only Inputs system can access private functions and variables from here
	
	friend InputSystem;
	friend Axis;
	
	//	Internal structures
	
	private:

	struct ClickInfo
	{
	private:

		float m_ClickDelta       = 0.25f;
		float m_doubleClickDelta = 0.25f;

		float m_elapsedTimeClicked   = 0.0f;
		float m_elapsedTimePressed = 0.0f;

		float m_squareDeltaPositionClick = 0.0f;
		float m_squareDeltaPositionDoubleClick = 0.0f;

		bool  m_wasClicked;
		bool  m_wasPressed;

		bool  m_grabbed;

		bool  m_isClick;
		bool  m_isDoubleClick;

		Vector2 m_singleClickSavedPosition;
		Vector2 m_doubleClickSavedPosition;

	public:

		void Update(float deltaTime, const Button& button);
		void UpdateClick(float deltaTime, const Button& button);
		void UpdateDoubleClick(float deltaTime, const Button& button);

		inline bool IsDoubleClicked(float acceptanceRadius) const;
		inline bool IsClicked(float acceptanceRadius) const;
		inline bool IsGrabbing() const;
	};
	
	//	Variables

	private:

		ENGINE_API static Mouse* m_instance;

		//	Button Management

		std::unordered_map<int, Button> buttonList;
		std::unordered_map<int, ClickInfo> clickInfos;

		std::vector<NamedButton> namedButtonList;
		std::vector<NamedAxis> namedAxesList;

		Button* buttonQueue[MAX_MOUSEBUTTONQUEUE] = {};
		int		queueSize = 0;

		//	Special Mouse datas

		bool m_moved    = false;
		bool m_scrolled = false;
		bool m_clicked  = false; // Wether a button was pressed or not

		Vector2 m_deltaScroll;
		Vector2 m_deltaPosition;

		Vector2 m_scroll;
		Vector2 m_position;
		Vector2 m_savedPosition;

		Window* m_window = nullptr;

	//	Constructors & Destructors

	private:

		//	only callable from InputSystem
		Mouse(Window* window);

		//	Remove copies

		Mouse(const Mouse&) = delete;
		void operator=(const Mouse&) = delete;

	//	Functions

	private:

		/**
		@brief Call this function at the beginning of each frame before Window callbacks.
		This function reset the 'one frame' states of the previously updated mouse buttons in the precedent frame
		- Only callable from InputSystem
		*/
		void Refresh();

		/**
		@brief Update function of Mouse class, used to check double clicks
		*/
		void Update(float deltaTime);

		/**
		@brief This function subscribe to GLFW mouse button callback
		*/
		void OnButtonEvent(const int button, const int action);

		/**
		@brief This function subscribe to GLFW mouse pos callback
		*/
		void OnMousePosEvent(const float x, const float y);

		/**
		@brief This function subscribe to GLFW mouse scroll callback
		*/
		void OnMouseScrollEvent(const float x, const float y);

		template <class TNamedObject>
		inline bool FindInNamedList(std::vector<TNamedObject>& namedList, const std::string& buttonName, int& index);

		/**
		@brief Return a button given a MouseButton ID

		@param buttonID : MouseButton ID from the Inputs enum

		@return Button : return a button
		*/
		inline static Button& GetButton(const MouseButton buttonID);

		/**
		@brief Return a button given a name

		@param mouseButtonName : Name of a registerd button

		@return Button* : return a button pointer given a name
		*/
		inline static Button* GetButton(const std::string& mouseButtonName);

		/**
		@brief Return an Axis given a name

		@param axisName : Name of the Axis registered

		@return Axis* : Axis pointer given a name
		*/
		inline static Axis* GetAxis(const std::string& axisName);

	public:

		/**
		Save named buttons and named axes (for editor and build)

		@param jsonFile : Save buttons data in json
		*/
		ENGINE_API static void SaveButtons(nlohmann::json& jsonFile);

		/**
		Load named buttons and named axes (for editor and build)

		@param jsonFile : Load button data from json
		*/
		ENGINE_API static void LoadButtons(nlohmann::json& jsonFile);

		//	Inlined

		/**
		@brief Return the named button list

		@param vector<NamedButton> : Button list
		*/
		inline static std::vector<NamedButton>& GetNamedButtonList();

		/**
		@brief Return the named axes list

		@param vector<NamedAxes> : Axes list
		*/
		inline static std::vector<NamedAxis>& GetNamedAxesList();

		/**
		@brief Add a new named button to the system

		@param buttonName : name of the button
		@param buttonCode : Button code of the button related
		*/
		inline static NamedButton& AddButton(const std::string& buttonName);

		/**
		@brief Add a new named axis to the system

		@param axisName : name of the axis
		@param buttonCodePos : Button code of the positive button related
		@param buttonCodeneg : Button code of the negative button related
		*/
		inline static NamedAxis& AddAxis(const std::string& axisName);

		/**
		@brief Told if a given button (from enum) has been pressed

		@param button Id : Id of the button

		@return bool : true if pressed, false otherwise
		*/
		inline static bool	GetButtonPressed(MouseButton buttonId);

		/**
		@brief Told if a given button (from name) has been pressed

		@param buttonName : Name of the button

		@return bool : true if pressed, false otherwise
		*/
		inline static bool	GetButtonPressed(const std::string& buttonName);

		/**
		@brief Told if a given button (from enum) has been released

		@param buttonId : Id of the button

		@return bool : true if released, false otherwise
		*/
		inline static bool	GetButtonUnpressed(MouseButton buttonId);

		/**
		@brief Told if a given button (from name) has been pressed

		@param buttonName : Name of the button

		@return bool : true if released, false otherwise
		*/
		inline static bool	GetButtonUnpressed(const std::string& buttonName);

		/**
		@brief Told if a given button (from enum) is currently down

		@param buttonId : Id of the button

		@return bool : true if down, false otherwise
		*/
		inline static bool	GetButtonDown(MouseButton buttonId);

		/**
		@brief Told if a given button (from name) is currently down

		@param buttonName : Name of the button

		@return bool : true if down, false otherwise
		*/
		inline static bool	GetButtonDown(const std::string& buttonName);

		/**
		@brief Told if a given button (from enum) is currently up

		@param buttonId : Id of the button

		@return bool : true if up, false otherwise
		*/
		inline static bool	GetButtonUp(MouseButton buttonId);

		/**
		@brief Told if a given button (from name) is currently up

		@param buttonName : Name of the button

		@return bool : true if up, false otherwise
		*/
		inline static bool	GetButtonUp(const std::string& buttonName);

		/**
		@brief Return wether a button was clicked (pressed + unpressed) in 0.25 sec

		@param buttonId : button id

		@return bool : was clicekd
		*/
		inline static bool GetButtonClicked(MouseButton buttonId);

		/**
		@brief Return wether a button was clicked (pressed + unpressed) in 0.25 sec and in the same zone (acceptanceRadius)

		@param buttonId : button id
		@param acceptanceRadius : accepted radius of the cursor displacement beween pressing and unpressing

		@return bool : was clicekd and was precise
		*/
		inline static bool GetButtonPreciseClicked(MouseButton buttonId, float acceptanceRadius = 25.f);

		/**
		@brief Return wether a button was doubleClicked (clicked + unpressed) in 0.25 sec

		@param buttonId : button id

		@return bool : was clicekd
		*/
		inline static bool GetButtonDoubleClicked(MouseButton buttonId);

		/**
		@brief Return wether a button was doubleClicked (clicked + unpressed) in 0.25 sec and in the same zone (acceptanceRadius)

		@param buttonId : button id
		@param acceptanceRadius : accepted radius of the cursor displacement beween pressing and unpressing

		@return bool : was double clicekd and was precise
		*/
		inline static bool GetButtonPreciseDoubleClicked(MouseButton buttonId, float acceptanceRadius = 25.f);

		/**
		@brief Is grabbing (when the button was down longer than the max time to be eligible of the clicks states)

		@param buttonName : Name of the button

		@return bool : true if grabbing
		*/
		inline static bool	GetButtonGrabbed(MouseButton buttonId);

		/**
		@brief Give the current raw value of an axis given by it's name

		@param axisName : Name of the axis

		@return float : Raw value of the axis
		*/
		inline static float	GetAxisRawValue(const std::string& axisName);

		/**
		@brief Give the current value of an axis given by it's name (smooth)

		@param axisName : Name of the axis

		@return float : Value of the axis
		*/
		inline static float GetAxisValue(const std::string& axisName);

		/**
		@brief Return mouse position (in screen coordinates relative to the upper-left corner of the content area of the window)

		@return Vector2 : Current position of the mouse cursor
		*/
		inline static Vector2 GetPosition();

		/**
		@brief Return mouse scroll value

		@return Vector2 : Current scroll values of the mouse
		*/
		inline static Vector2 GetScroll();

		/**
		@brief Return position differences between now and the previous frame

		@return Vector2 : Delta position of the mouse cursor
		*/
		inline static Vector2 GetDeltaPosition();

		/**
		@brief Return scroll differences between now and the previous frame

		@return Vector2 : Current scroll value of the mouse
		*/
		inline static Vector2 GetDeltaScroll();
		
		/**
		@brief Set/override mouse position (in screen coordinates relative to the upper-left corner of the content area of the window)

		@param newPosition : new position of the mouse cursor
		*/
		ENGINE_API static void SetPosition(const Vector2& newPosition);

		/**
		@brief Set/override mouse coordinate on x axis (in screen coordinates relative to the upper-left corner of the content area of the window)

		@param newPosition : new x coordinate of the mouse cursor
		*/
		ENGINE_API static void SetPositionX(float x);

		/**
		@brief Set/override mouse coordinate on y axis (in screen coordinates relative to the upper-left corner of the content area of the window)

		@param newPosition : new y coordinate of the mouse cursor
		*/
		ENGINE_API static void SetPositionY(float y);

		/**
		@brief Reset position to 0,0
		*/
		ENGINE_API static void ResetPosition();

		/**
		@brief Reset scroll to 0,0
		*/
		ENGINE_API static void ResetScroll();

		/**
		* @brief Was a mouse button clicked ?
		*/
		ENGINE_API inline static bool WasClicked();

		/**
		@brief Reset delta mouse pos
		*/
		ENGINE_API inline static void ResetDeltaPosition();

		// Non-Inlined

		/**
		@brief Return mouse input mode

		@return MouseMode : Get mouse cursor mode
		*/
		ENGINE_API static MouseMode GetMode();

		/**
		@brief Set mouse mode

		@param mode : new input mode
		*/
		ENGINE_API static void SetMode(MouseMode mode);

	};
}

#include "io/Mouse.inl"