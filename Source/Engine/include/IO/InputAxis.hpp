#pragma once

#include <string>

#include "EngineDll.hpp"

namespace Input
{
	struct Button;

	struct Axis
	{
	public:
		int buttonIdNeg;
		int buttonIdPos;

		float amplitude = 1.f;
		float timeMultiplier = 1.f;
		
	private:
		float timeToLerp = 0.2f;
		float elapsedTimeChanged = 0.f;

	public:
		float value	   = 0.f;
		float rawValue = 0.f;

		ENGINE_API Axis(int idNeg, int idPos);

		void Compute(float deltaTime);
	};


	/**
	Structure to store a named axis data
	*/
	struct NamedAxis
	{
		std::string name;
		std::string idNameNeg;
		std::string idNamePos;
		Axis axis;
	};
}
