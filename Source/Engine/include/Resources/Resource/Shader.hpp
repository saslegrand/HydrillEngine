#pragma once

#include <string>

#include "EngineDll.hpp"

/**
@brief Shader
*/
class ENGINE_API Shader
{
protected:

	// Disable export warning (In case the std version is different where the engine dll is used)
#pragma warning(disable:4251)
	std::string m_filePath = "";
#pragma warning(default:4251)

public:
	unsigned int ID = 0;

public:
	Shader() = default;
	Shader(Shader const& sCpy) = delete;
	Shader(Shader&& sMove) noexcept;

	Shader& operator=(Shader const& sCpy) = delete;
	Shader& operator=(Shader&& sMove) noexcept;

	~Shader();

	bool Create(const std::string& filePath, std::string& shaderStr);
	void Generate(const char* content, unsigned int type);

};
