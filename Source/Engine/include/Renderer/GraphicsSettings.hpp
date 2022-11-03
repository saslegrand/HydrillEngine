#pragma once

#include <nlohmann/json_fwd.hpp>

#include "EngineDLL.hpp"

struct ENGINE_API GraphicsSettings
{
	struct GraphicSettingsUBO
	{
		bool  shadows = true;
	}GPUSettings;

	unsigned int graphicSettingUBO = 0;
	
	bool  bloomEnabled = true;
	float bloomThreshold = 1.0f;
	
	bool  autoExposure = true;

	float brightnessCalculFrequency = 1.f;
	float exposureMultiplier = 1.f;
	float exposureSpeed = 0.5f;
	float minExposure = 0.01f;
	float maxExposure = 10.0f;
	float constantExposure = 1.f;

	void Save(nlohmann::json& jsonField);
	void Load(nlohmann::json& jsonField);

	void Initialize();
	void SendGPUSettings();

};