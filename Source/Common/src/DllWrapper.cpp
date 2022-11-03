#include "DllWrapper.hpp"

#include <filesystem>
#include <Windows.h>

#include "Core/Logger.hpp"
#include "Tools/StringHelper.hpp"

DllWrapper::DllWrapper(std::string_view dllSrc)
{
	m_dllSrc = std::make_unique<std::filesystem::path>(dllSrc);

	m_dllDir = std::make_unique<std::filesystem::path>(dllSrc);
	m_dllDir->replace_extension("copy.tmp");

	// Load the dll when created
	Load();
}

DllWrapper::~DllWrapper()
{
	// If the a dll was loaded, free it
	if (m_handleModule != nullptr)
		FreeLibrary(m_handleModule);
}

bool DllWrapper::Load()
{
	
	// Check if the given path is correct and associated to a dll file
	if (!std::filesystem::exists(*m_dllSrc))
	{
		Logger::Error("DllWrapper - The dll '" + m_dllSrc->string() + "' not exists");
		return false;
	}
	std::string srcString	= m_dllSrc->string();
	std::string dirString	= m_dllDir->string();
	
	// Copy the dll
	CopyFileA(srcString.c_str(), dirString.c_str(), false);

	// Load the copied dll
	m_handleModule = LoadLibraryA(dirString.c_str());
	// Check if the loading succeeded
	if (m_handleModule == nullptr)
	{
		Logger::Error("The loading of the dll '" + srcString + "' has failed");
		return false;
	}

	return true;
}

bool DllWrapper::Reload()
{
	if (m_handleModule != nullptr)
	{
		FreeLibrary(m_handleModule);
		m_handleModule = nullptr;
	}

	return Load();
}
