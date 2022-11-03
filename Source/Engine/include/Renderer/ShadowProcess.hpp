#pragma once

#include "Renderer/Primitives/ShaderProgram.hpp"
#include "Renderer/Primitives/FrameBuffer.hpp"
#include "Renderer/RenderConfig.hpp"
#include "Maths/Matrix4.hpp"

class LightComponent;

class ShadowProcess
{

//	Variables

private:

	ShaderProgram m_meshShadowmapShader;
	ShaderProgram m_skeletalMeshShadowmapShader;

	ShaderProgram m_meshCascadeShadowmapShader;
	ShaderProgram m_skeletalMeshCascadeShadowmapShader;

	ShaderProgram m_meshShadowCubemapShader;
	ShaderProgram m_skeletalMeshShadowCubemapShader;

	unsigned int m_depthMapFBO = 0;
	unsigned int m_depthCubeMapFBO = 0;
	unsigned int m_cascadeDepthMapFBO = 0;

	Matrix4 m_viewProjSpotLight[Max_Lights_Count];
	Matrix4 m_viewProjPointLight[Max_Lights_Count][6] ;
	Matrix4 m_viewProjDirectional[Max_Lights_Count][Cascade_Count];
	float   m_cascadeDistances[Cascade_Count];
	
	unsigned int m_ShadowMapUBO = 0;

//	Constructor(s) & Destructor(s)

public:

	ShadowProcess();
	~ShadowProcess();

//	Functions

private:

	/**
	@brief Generate spot light depthmap
	
	@param lightViewProj : view projection matrix of the light
	@param index : current light index
	*/
	void GenerateSpotLightDepthMap(const Matrix4& lightViewProj, int index);

	/**
	@brief Render scene from spot light view

	@param lightViewProj : view projection matrix of the light
	*/
	void RenderSpotLightScene(const Matrix4& lightViewProj);

	/**
	@brief Compute spot light view projection matrix

	@param lightViewProj : view projection matrix of the light to compute
	@param light : current light to compute view proj matrix
	*/
	void GetSpotLightViewProj(Matrix4& lightViewProj, const LightComponent& light);


	/**
	@brief Generate point light depthCubeMap

	@param lightViewProj : view projection matrix of the light
	@param index : current light index
	*/
	void GeneratePointLightDepthCubeMap(const Matrix4 lightViewProj[6], int index);

	/**
	@brief Render scene from point light view

	@param lightViewProj : view projection matrix of the light
	*/
	void RenderPointLightScene(const Matrix4 lightViewProj[6], int index);

	/**
	@brief Compute point light view projection matrices

	@param lightViewProj : view projection matrices of the point light to compute
	@param light : current light to compute view proj matrix
	*/
	void GetPointLightViewProj(Matrix4 lightViewProj[6], const LightComponent& light);


	/**
	@brief Generate directional light depthmaps

	@param lightViewProj : view projection matrix of the light
	@param index : current light index
	*/
	void GenerateDirectionalLightCascadeDepthMap(const Matrix4 lightViewProj[Cascade_Count], int index);

	/**
	@brief Render scene from directional light view

	@param lightViewProj : view projection matrix of the light
	*/
	void RenderDirectionalLightScene(const Matrix4 lightViewProj[Cascade_Count], int index);

	/**
	@brief Compute directional light view projection matrices

	@param lightViewProj : view projection matrices of the directional light to compute
	@param light : current light to compute view proj matrix
	*/
	void GetDirectionalLightViewProj(Matrix4 lightViewProj[Cascade_Count], const LightComponent& light);

public:

	/**
	@brief Initialize shadow process
	*/
	void Initialize();

	/**
	@brief Shadow process function
	*/
	void ProcessShadowMaps();
};