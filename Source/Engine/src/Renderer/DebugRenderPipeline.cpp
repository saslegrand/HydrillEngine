#include <glad/gl.h>

#include "Renderer/DebugRenderPipeline.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "ECS/MeshComponent.hpp"
#include "Maths/Matrix4.hpp"

#pragma region DEBUG SHADER REGION

static const char* DebugVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};

//	Uniform variables

uniform mat4 uModel;

//  Functions

void main()
{    
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
})GLSL";

static const char* DebugFragmentShaderStr = R"GLSL(
#version 450 core

//  Uniforms

uniform vec4 uColor;

//  Shader Output

out vec4 oColor;

//  Functions

void main()
{
	vec3 toLinear = vec3(2.2);
    vec4 color = vec4(pow(uColor.rgb, toLinear),uColor.a);

    oColor = color;
})GLSL";

#pragma endregion


DebugRenderPipeline::PrimitiveInstance::PrimitiveInstance(const float* vertices, unsigned int pointCount, const std::vector<unsigned int>& indices, float width, const Color3& color)
	: wireframecolor(color), wireframeWidth(width), indices(indices)
{
	// Create array and buffer
	GLuint vao = VAO.GetID();
	GLuint vbo = VBO.GetID();

	size = pointCount;

	// Allocate vertex buffer storage (empty)
	glNamedBufferStorage(vbo, size * sizeof(Vector3), vertices, GL_MAP_READ_BIT);

	// Link array and buffer (binding 0)
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vector3));

	// Enable attributes
	glEnableVertexArrayAttrib(vao, 0);

	// Bind vao attributes to the vbo (from binding index)
	glVertexArrayAttribBinding(vao, 0, 0);

	// Set attributes format
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
}



DebugRenderPipeline::PrimitiveInstance::PrimitiveInstance(const float* vertices, unsigned int pointCount, const std::vector<unsigned int>& indices, const Matrix4& model, float width, const Color3& color)
	: PrimitiveInstance(vertices, pointCount, indices, width,color)
{
	globaleModel = model;
}

DebugRenderPipeline::DebugRenderPipeline()
{
}


DebugRenderPipeline::~DebugRenderPipeline()
{
}


void DebugRenderPipeline::Initialize()
{
	//  Load Shader

	m_shader.Generate(DebugVertexShaderStr, DebugFragmentShaderStr);
}


void DebugRenderPipeline::Render()
{
	m_shader.Bind();

	for (PrimitiveInstance& instance : m_instances)
	{
		m_shader.SendUniform("uModel", instance.globaleModel.elements);
		m_shader.SendUniform("uColor", &instance.wireframecolor.r);

		glLineWidth(instance.wireframeWidth);

		GLsizei indicesSize = static_cast<GLsizei>(instance.indices.size());

		instance.VAO.Bind();
		glDrawElements(
			GL_LINES,
			indicesSize,
			GL_UNSIGNED_INT,
			instance.indices.data()
		);
	}

	glBindVertexArray(0);

	m_shader.Unbind();
}


void DebugRenderPipeline::ClearPrimitives()
{
	m_instances.clear();
}


void DebugRenderPipeline::AddLine(const Vector3& startWorldPosition, const Vector3& endWorldPosition, float width, const Color3& color)
{
	float vertices[] =
	{
		startWorldPosition.x,   startWorldPosition.y,   startWorldPosition.z,
		endWorldPosition.x,     endWorldPosition.y,     endWorldPosition.z
	};

	std::vector<unsigned int> indices = { 0,1 };

	m_instances.emplace_back(
		vertices,
		2,
		indices,
		width,
		color
	);
}


void DebugRenderPipeline::AddLine(const Matrix4& model, const Vector3& startLocalPosition, const Vector3& endLocalPosition, float width, const Color3& color)
{
	float vertices[] =
	{
		startLocalPosition.x,   startLocalPosition.y,   startLocalPosition.z,
		endLocalPosition.x,     endLocalPosition.y,     endLocalPosition.z
	};

	std::vector<unsigned int> indices = { 0,1 };

	m_instances.emplace_back(
		vertices,
		2,
		indices,
		model,
		width,
		color
	);
}


void DebugRenderPipeline::AddSphere(const Matrix4& model, float radius, float width, const Color3& color)
{
	AddCircle(model * Matrix4::RotateX(Maths::PIHalf), radius, 32, width, color);
	AddCircle(model * Matrix4::RotateZ(Maths::PIHalf), radius, 32, width, color);
	AddCircle(model, radius, 32, width, color);
}

void DebugRenderPipeline::AddCone(const Matrix4& model, float baseRadius, float length, float width, const Color3& color)
{
	AddCircle(model * Matrix4::Translate(Vector3::Down * length), baseRadius, 32, width, color);

	AddLine(model, Vector3::Zero, Vector3::Down * length + Vector3::Right * baseRadius, width, color);
	AddLine(model, Vector3::Zero, Vector3::Down * length + Vector3::Left * baseRadius, width, color);
	AddLine(model, Vector3::Zero, Vector3::Down * length + Vector3::Forward * baseRadius, width, color);
	AddLine(model, Vector3::Zero, Vector3::Down * length + Vector3::Back * baseRadius, width, color);
}


void DebugRenderPipeline::AddCircle(const Matrix4& model, float radius, unsigned int pointCount, float width, const Color3& color)
{
	//	Circle Mesh generation

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float angleStep = Maths::PI2 / static_cast<float>(pointCount);


	for (unsigned int i = 0; i < pointCount; i++)
	{
		vertices.push_back(radius* Maths::Cos(static_cast<float>(i) * angleStep));
		vertices.push_back(0.0f);
		vertices.push_back(radius* Maths::Sin(static_cast<float>(i) * angleStep));
	}

	for (unsigned int i = 0; i < pointCount; i++)
	{
		indices.push_back(i);
		indices.push_back((i+1) % pointCount);
	}

	m_instances.emplace_back(
		&vertices[0],
		static_cast<unsigned int>(vertices.size() / 3),
		indices,
		model,
		width,
		color
	);
}


void DebugRenderPipeline::AddCylinder(const Matrix4& model, float radius, float height, float width, const Color3& color)
{
	Matrix4 m = model;
	Vector3 shapeScale = { radius, height, radius };

	m *= Matrix4::Scale(shapeScale);

	float top = height * 0.5f;
	float bot =-height * 0.5f;

	AddCircle(model * Matrix4::Translate({ 0.f, top, 0.f }), radius, 32, width, color);
	AddCircle(model * Matrix4::Translate({ 0.f, bot, 0.f }), radius, 32, width, color);

	{
		Vector3 a = { radius, bot, 0.0f };
		Vector3 b = { radius, top, 0.0f };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = {-radius, bot, 0.0f };
		Vector3 b = {-radius, top, 0.0f };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = { 0.0f, bot, radius };
		Vector3 b = { 0.0f, top, radius };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = { 0.0f, bot,-radius };
		Vector3 b = { 0.0f, top,-radius };
		AddLine(model, a, b, width, color);
	}
}


void DebugRenderPipeline::AddCapsule(const Matrix4& model, float radius, float height, float width, const Color3& color)
{
	Matrix4 m = model;
	Vector3 shapeScale = { radius, height, radius };

	m *= Matrix4::Scale(shapeScale);

	float top = height * 0.5f;
	float bot = -height * 0.5f;

	AddSphere(model * Matrix4::Translate({ 0.f, top, 0.f }), radius, width, color);
	AddSphere(model * Matrix4::Translate({ 0.f, bot, 0.f }), radius, width, color);

	{
		Vector3 a = { radius, bot, 0.0f };
		Vector3 b = { radius, top, 0.0f };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = { -radius, bot, 0.0f };
		Vector3 b = { -radius, top, 0.0f };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = { 0.0f, bot, radius };
		Vector3 b = { 0.0f, top, radius };
		AddLine(model, a, b, width, color);
	}

	{
		Vector3 a = { 0.0f, bot,-radius };
		Vector3 b = { 0.0f, top,-radius };
		AddLine(model, a, b, width, color);
	}
}


void DebugRenderPipeline::AddBox(const Matrix4& model, float boxWidth, float boxHeight, float boxLength, float width, const Color3& color)
{
	Vector3 a = {-boxWidth,-boxHeight,-boxLength };
	Vector3 b = { boxWidth,-boxHeight,-boxLength };
	Vector3 c = { boxWidth,-boxHeight, boxLength };
	Vector3 d = {-boxWidth,-boxHeight, boxLength };

	Vector3 e = {-boxWidth, boxHeight,-boxLength };
	Vector3 f = { boxWidth, boxHeight,-boxLength };
	Vector3 g = { boxWidth, boxHeight, boxLength };
	Vector3 h = {-boxWidth, boxHeight, boxLength };

	AddLine(model, a, b, width, color);
	AddLine(model, b, c, width, color);
	AddLine(model, c, d, width, color);
	AddLine(model, d, a, width, color);

	AddLine(model, e, f, width, color);
	AddLine(model, f, g, width, color);
	AddLine(model, g, h, width, color);
	AddLine(model, h, e, width, color);

	AddLine(model, a, e, width, color);
	AddLine(model, b, f, width, color);
	AddLine(model, c, g, width, color);
	AddLine(model, d, h, width, color);
}