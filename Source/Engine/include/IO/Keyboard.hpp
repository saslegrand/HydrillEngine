#pragma once

#include <unordered_map>

#include <Refureku/Refureku.h>
#include <nlohmann/json_fwd.hpp>

#include "Tools/Event.hpp"
#include "IO/Inputs.hpp"
#include "IO/Button.hpp"
#include "IO/InputAxis.hpp"
#include "EngineDLL.hpp"

#define MAX_KEYQUEUE 50

class Window;
class InputSystem;

namespace Input
{

	/**
	@brief Keyboard system managing all of the keyboards inputs
	*/
	class Keyboard
	{

	//	Only Inputs system and Axis classes can access private functions and variables from her
		
		friend InputSystem;
		friend Axis;

	//	Variables

	private:

		ENGINE_API static Keyboard* m_instance;

		//	Button Management
		std::unordered_map<int, Button> keyList;

		std::vector<NamedButton> namedButtonList;
		std::vector<NamedAxis> namedAxesList;

		Button* keyQueue[MAX_KEYQUEUE] = {};
		unsigned int		queueSize = 0;

	//	Constructor(s) & Destructor(s)

	private:

		//	only callable from InputSystem
		Keyboard(Window* window);

		//	Remove copies

		Keyboard(const Keyboard&) = delete;
		void operator=(const Keyboard&) = delete;

	//	Functions

	private:

		/**
		@brief Call this function at the beginning of each frame before Window callbacks.
		This function reset the 'one frame' states of the previously updated keys in the precedent frame
		- Only callable from InputSystem
		*/
		void Refresh();

		/**
		@brief This function subscribe to GLFW key callback
		*/
		void OnKeyEvent(const int key, const int action);

		template <class TNamedObject>
		inline bool FindInNamedList(std::vector<TNamedObject>& namedList, const std::string& keyName, int& index);

		/**
		@brief Return a Button given a name

		@param keyName : Name of the Button registered

		@return Button* : Key button given a name
		*/
		inline static Button* GetKey(const std::string& keyName);

		/**
		@brief Return a Button given a name

		@param keyCode : Id of the Button registered

		@return Button* : Key button given a name
		*/
		inline static Button& GetKey(Key keyCode);

		/**
		@brief Return an Axis given a name

		@param axisName : Name of the Axis registered

		@return Axis* : Axis given a name
		*/
		inline static Axis* GetAxis(const std::string& axisName);


	public:

		/**
		Save named buttons and named axes (for editor and build)

		@param jsonFile : Save keys data in json
		*/
		ENGINE_API static void SaveKeys(nlohmann::json& jsonFile);

		/**
		Load named buttons and named axes (for editor and build)

		@param jsonFile : Load key data from json
		*/
		ENGINE_API static void LoadKeys(nlohmann::json& jsonFile);

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
		@brief Add a new named key to the system

		@param keyName : name of the key
		*/
		inline static NamedButton&	AddKey(const std::string& keyName);

		/**
		@brief Add a new named axis to the system

		@param axisName : name of the axis
		*/
		inline static NamedAxis&	AddAxis(const std::string& axisName);

		/**
		@brief Told if a given key (from enum) has been pressed

		@param keyId : Id of the key

		@return bool : true if pressed, false otherwise
		*/
		inline static bool	GetKeyPressed(Key keyId);

		/**
		@brief Told if a given key (from name) has been pressed

		@param keyName : Name of the key

		@return bool : true if pressed, false otherwise
		*/
		inline static bool	GetKeyPressed(const std::string& keyName);

		/**
		@brief Told if a given key (from enum) has been released

		@param keyId : Id of the key

		@return bool : true if released, false otherwise
		*/
		inline static bool	GetKeyUnpressed(Key keyId);

		/**
		@brief Told if a given key (from name) has been pressed

		@param keyName : Name of the key

		@return bool : true if released, false otherwise
		*/
		inline static bool	GetKeyUnpressed(const std::string& keyName);

		/**
		@brief Told if a given key (from enum) is currently down

		@param keyId : Id of the key

		@return bool : true if down, false otherwise
		*/
		inline static bool	GetKeyDown(Key keyId);

		/**
		@brief Told if a given key (from name) is currently down

		@param keyName : Name of the key

		@return bool : true if down, false otherwise
		*/
		inline static bool	GetKeyDown(const std::string& keyName);

		/**
		@brief Told if a given key (from enum) is currently up

		@param keyId : Id of the key

		@return bool : true if up, false otherwise
		*/
		inline static bool	GetKeyUp(Key keyId);

		/**
		@brief Told if a given key (from name) is currently up

		@param keyName : Name of the key

		@return bool : true if up, false otherwise
		*/
		inline static bool	GetKeyUp(const std::string& keyName);

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

	};
}

#include "io/Keyboard.inl"
