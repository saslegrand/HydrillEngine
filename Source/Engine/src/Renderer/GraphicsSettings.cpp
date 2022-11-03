#include <nlohmann/json.hpp>

#include <glad/gl.h>

#include "Tools/SerializationUtils.hpp"
#include "Renderer/GraphicsSettings.hpp"
#include "Renderer/RenderConfig.hpp"
#include "EngineDLL.hpp"

void GraphicsSettings::Initialize()
{
	glCreateBuffers(UBOIndex_Graphics_Settings, &graphicSettingUBO);
	glNamedBufferData(graphicSettingUBO, sizeof(GraphicsSettings::GPUSettings), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, UBOIndex_Graphics_Settings, graphicSettingUBO, 0, sizeof(GraphicsSettings::GPUSettings));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	SendGPUSettings();
}


void GraphicsSettings::SendGPUSettings()
{
	glNamedBufferSubData(graphicSettingUBO, 0, sizeof(GraphicsSettings::GPUSettings), &GPUSettings);
}


void GraphicsSettings::Save(nlohmann::json& jsonField)
{
	nlohmann::json& jsonGraphics = jsonField["Graphics"];

	jsonGraphics["bloomEnabled"] = bloomEnabled;
	jsonGraphics["bloomThreshold"] = bloomThreshold;
	jsonGraphics["autoExposure"] = autoExposure;
	jsonGraphics["exposureMultiplier"] = exposureMultiplier;
	jsonGraphics["exposureSpeed"] = exposureSpeed;
	jsonGraphics["minExposure"] = minExposure;
	jsonGraphics["maxExposure"] = maxExposure; 
	jsonGraphics["constantExposure"] = constantExposure;
	jsonGraphics["shadows"] = GPUSettings.shadows;
}

void GraphicsSettings::Load(nlohmann::json& jsonField)
{
	nlohmann::json& jsonGraphics = jsonField["Graphics"];

	Serialization::TryGetValue(jsonGraphics, "bloomEnabled", bloomEnabled);
	Serialization::TryGetValue(jsonGraphics, "bloomThreshold", bloomThreshold);
	Serialization::TryGetValue(jsonGraphics, "autoExposure", autoExposure);
	Serialization::TryGetValue(jsonGraphics, "exposureMultiplier", exposureMultiplier);
	Serialization::TryGetValue(jsonGraphics, "exposureSpeed", exposureSpeed);
	Serialization::TryGetValue(jsonGraphics, "minExposure", minExposure);
	Serialization::TryGetValue(jsonGraphics, "maxExposure", maxExposure);
	Serialization::TryGetValue(jsonGraphics, "constantExposure", constantExposure);
	Serialization::TryGetValue(jsonGraphics, "shadows", GPUSettings.shadows);
}