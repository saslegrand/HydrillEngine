#pragma once

#include "ECS/Gameobject.hpp"
#include "Types/Color.hpp"
#include "Renderer/RenderObjects/Light.hpp"

#include "Generated/LightComponent.rfkh.h"

class SubRenderPipeline;
struct GPULightData;

class HY_CLASS() ENGINE_API LightComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<LightComponent> defaultInstantiator(GameObject & GO, const HYGUID & uid);

//	Variables

public:

	HY_FIELD() LightType type = LightType::Point;

	HY_FIELD() bool enabled = true;
	HY_FIELD() bool castShadows = true;

	HY_FIELD(PHeader("Colors")) 
			   Color3 diffuse = Color3::White;
	HY_FIELD() Color3 ambient = { 0.1f,0.1f,0.1f };
	HY_FIELD() Color3 specular = Color3::White;


	HY_FIELD(PHeader("Values"), PRange(0.0f, 100.f)) 
									   float intensity = 1.f;
	HY_FIELD(PRange(0.0f, 500.f))      float radius = 5.f;
	HY_FIELD(PRange(0.0f, Maths::PI2)) float innerCutoff = 0.91f;
	HY_FIELD(PRange(0.0f, Maths::PI2)) float outerCutoff = 0.82f;

//	Constructors & Destructors

public:
	LightComponent(GameObject& gameObject, const HYGUID& uid);
	LightComponent(LightComponent const& cam) = delete;
	LightComponent(LightComponent&& cam) = default;

	LightComponent& operator=(LightComponent const& cam) = delete;
	LightComponent& operator=(LightComponent && cam) = default;

	~LightComponent();

	LightComponent_GENERATED
};

File_LightComponent_GENERATED