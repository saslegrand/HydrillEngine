#pragma once

#include <string>

namespace Input
{
	/**
	@brief Input button which contains states of inputs
	*/
	struct Button
	{
		friend class Mouse;
		friend class Keyboard;

	//	Variables

	public:

		/**
		@brief Is the key currently down ?
		*/
		bool down = false;
		
		/**
		@brief Is the key currently up
		*/
		bool up = true;

		/**
		@brief True during the frame it is pressed (only 1 frame)
		*/
		bool pressed = false;

		/**
		@brief True during the frame it is unpressed (only 1 frame)
		*/
		bool unpressed = false;

	//	Functions

	private:

		/**
		@brief Update the key given an action

		@param action : action the button is going through
		*/
		void Update(const int action);


		/**
		@brief Refresh the button state, this function is called only if pressed or unpressed states were true
		*/
		inline void Refresh();
	
	public:

		/**
		@brief Consume the pressed or unpressed state
		*/
		inline void Consume();

	//	Internal class

	private:

		/**
		@brief Actions type of the inputs
		*/
		enum class InputAction
		{
			Unpressed = 0,
			Pressed = 1,
			Repeated = 2,
		};
	};

	/**
	Structure to store a named button data
	*/
	struct NamedButton
	{
		std::string name;
		std::string idName;
		int id = -1;
	};

}

#include "io/Button.inl"