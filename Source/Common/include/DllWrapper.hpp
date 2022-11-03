#pragma once

#define _AMD64_
#include <minwindef.h>

#include <string>
#include <memory>

#include <Tools/ReflectedSTD.hpp>

namespace std::filesystem
{
	class path;
}

/**
Base class to wrap and load a dynamic library.
The goal is to create a copy of the dll to allow reloading it in run time.
The copy is always created in the same directory as the given dll.
*/
class DllWrapper
{
private:
	// Module of the target library
	HMODULE m_handleModule;

	std::unique_ptr<std::filesystem::path> m_dllSrc;
	std::unique_ptr<std::filesystem::path> m_dllDir;

public:
	DllWrapper(std::string_view dllSrc);
	~DllWrapper();

public:
	/**
	Load a dll from its path

	@return bool : True if load succeed, false otherwise
	*/
	bool Load();

	/**
	Free the dll module if already loaded and reload the dll

	@return bool : True if reload succeed, false otherwise
	*/
	bool Reload();
};