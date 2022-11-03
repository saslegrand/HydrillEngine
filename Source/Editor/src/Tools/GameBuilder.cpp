#include "Tools/GameBuilder.hpp"

#include <filesystem>

#include <Resources/Loaders/ResourcesLoader.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Tools/StringHelper.hpp>
#include <EngineContext.hpp>
#include <Core/Logger.hpp>

#include "PathConfig.hpp"
#include "Windows/BuildSettingsWindow.hpp"
#include "Windows/WindowManager.hpp"
#include "EditorContext.hpp"

// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces 
//              to create and store a shortcut to the specified object. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the 
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the 
//                Shell link, stored in the Comment field of the link
//                properties.

#include <objidl.h>
#include <winnls.h>
#include <Windows.h>
#include <shlguid.h>
#include <atlbase.h>
#include <objbase.h>
#include <shobjidl.h>

namespace fs = std::filesystem;

std::wstring StringToWstring(std::string_view str)
{
	return std::wstring(str.begin(), str.end());
}

HRESULT CreateShortcut(/*in*/ LPCTSTR lpszFileName,
	/*in*/ LPCTSTR lpszDesc,
	/*in*/ LPCTSTR lpszShortcutPath,
	/*in*/ LPCTSTR lpszWorkingDir,
	/*in*/ LPCWSTR lpszIconLocation)
{
	HRESULT hRes = E_FAIL;
	DWORD dwRet = 0;
	CComPtr<IShellLink> ipShellLink;
	// buffer that receives the null-terminated string 
	// for the drive and path
	TCHAR szPath[MAX_PATH];
	// buffer that receives the address of the final 
	//file name component in the path
	LPTSTR lpszFilePart;
	WCHAR wszTemp[MAX_PATH];

	// Retrieve the full path and file name of a specified file
	dwRet = GetFullPathName(lpszFileName,
		sizeof(szPath) / sizeof(TCHAR),
		szPath, &lpszFilePart);
	if (!dwRet)
		return hRes;

	// Get a pointer to the IShellLink interface
	hRes = CoCreateInstance(CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		(void**)&ipShellLink);

	if (SUCCEEDED(hRes))
	{
		// Get a pointer to the IPersistFile interface
		CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

		// Set the path to the shortcut target and add the description
		hRes = ipShellLink->SetPath(szPath);
		if (FAILED(hRes))
			return hRes;

		hRes = ipShellLink->SetWorkingDirectory(lpszWorkingDir);
		if (FAILED(hRes))
			return hRes;

		hRes = ipShellLink->SetDescription(lpszDesc);
		if (FAILED(hRes))
			return hRes;

		hRes = ipShellLink->SetIconLocation(lpszIconLocation, 0);
		if (FAILED(hRes))
			return hRes;

		// IPersistFile is using LPCOLESTR, so make sure 
				// that the string is Unicode
#if !defined _UNICODE
		MultiByteToWideChar(CP_ACP, 0,
			lpszShortcutPath, -1, wszTemp, MAX_PATH);
#else
		wcsncpy_s(wszTemp, lpszShortcutPath, MAX_PATH);
#endif

		// Write the shortcut to disk
		hRes = ipPersistFile->Save(wszTemp, TRUE);
	}

	return hRes;
}

bool GameBuilder::CopyFileFromFilename(const std::filesystem::path& src, const std::filesystem::path& dest)
{
	// Copy a file from src to dest
	fs::path newPath = dest;
	newPath.append(src.filename().string());

	try {
		return std::filesystem::copy_file(src, newPath);
	}
	catch (std::filesystem::filesystem_error const& ex) {
		Logger::Error("GameBuilder - " + std::string(ex.what()));
		return false;
	}
}

bool GameBuilder::CopyDirectory(const std::filesystem::path& srcDir, const std::filesystem::path& destDir, std::filesystem::copy_options options)
{
	try {
		std::filesystem::copy(srcDir, destDir, options);
		return true;
	}
	catch (std::filesystem::filesystem_error const& ex) {
		Logger::Error("GameBuilder - " + std::string(ex.what()));
		return false;
	}
}

bool GameBuilder::CreateBuildFolder(const std::filesystem::path& buildPath, const BuildData& buildData)
{
	Logger* logger = EngineContext::Instance().logger;

	if (buildPath.empty())
		return false;

	logger->Info("GameBuilder - Create build directory");

	// Check if the folder is already created (delete it)
	if (fs::exists(buildPath))
	{
		// Empty folder
		logger->Warning("GameBuilder - A folder with name '" + buildData.folderName + "' at '" + buildData.buildPath + "' already exists");
		logger->Info("GameBuilder - Folder '" + buildData.folderName + "' will be deleted to write the new files");

		uintmax_t count = fs::remove_all(buildPath);
		logger->Info("GameBuilder - Deleted " + std::to_string(count) + " files or directories");
	}

	// Create the build directory
	if (!fs::create_directories(buildPath))
	{
		logger->Error("GameBuilder - Failed to create '" + buildPath.string() + "' directory");
		return false;
	}

	return true;
}

bool GameBuilder::ExportBinaries(const std::filesystem::path& buildPath, const BuildData& buildData)
{
	Logger* logger = EngineContext::Instance().logger;

	logger->Info("GameBuilder - Copy binary files");

	// Create binaries build path
	fs::path buildBinPath = buildPath;
#ifdef PUBLISHED
	buildBinPath.append("Binaries");
#else
	buildBinPath.append(R"(NotPublished\NotPublished\Binaries)");
#endif

	// Create Binaries directory
	{
		if (!fs::create_directories(buildBinPath))
		{
			logger->Error("GameBuilder - Failed to create '" + buildBinPath.string() + "' drectory");
			return false;
		}
	}

	fs::path editorBinPath = BINARIES_ROOT_RAW;
	// Copy all binary files
	{
		for (const auto& file : fs::directory_iterator(editorBinPath))
		{

			if (file.is_directory())
				continue;

			fs::path filePath = file.path();
			std::string extension = filePath.extension().generic_string();

			// Check if the file is a library
			if (extension.compare(".dll") != 0)
				continue;

			// Copy the file in the build folder
			if (!CopyFileFromFilename(filePath, buildBinPath))
				return false;
		}
	}

	logger->Info("GameBuilder - Copy game executable");
	
	// Copy the Game.exe file
	{
		// Export game.exe
		fs::path editorGamePath = editorBinPath;
		editorGamePath.append("Game.exe");

		// Try copy
		if (!CopyFileFromFilename(editorGamePath, buildBinPath))
			return false;
	}
	
	logger->Info("GameBuilder - Create game shortcut");

	// Create Game.exe shortcut at build root + move icon
	{
		fs::path newIconPath = buildPath;

		if (buildData.iconPath.empty())
		{
			newIconPath.append(R"(Internal\Resources\Engine\Images\HyDrill_Light_Ico.ico)");
		}
		else
		{
			fs::path iconPath = buildData.iconPath;
			newIconPath.append(R"(Internal\Resources\Engine\Images)");

			// Check if the icon file is already in the internal folder
			fs::path iconInternalPath = newIconPath.string();
			iconInternalPath.append(StringHelper::GetFileNameFromPath(buildData.iconPath));
			if (!fs::exists(iconInternalPath))
				CopyFileFromFilename(iconPath, newIconPath);

			newIconPath = iconInternalPath;
		}

		fs::path buildGamePath = buildBinPath;
		buildGamePath.append("Game.exe");

		fs::path buildLinkPath = buildPath;
		buildLinkPath.append("Game.lnk");

		CreateShortcut(buildGamePath.wstring().c_str(), StringToWstring("Game executable user shortcut").c_str(),
			buildLinkPath.wstring().c_str(), buildBinPath.wstring().c_str(), StringToWstring(newIconPath.string()).c_str());
	}

	logger->Info("GameBuilder - Create Game settings file");

	// Create build settings file
	{
		fs::path settingsPath = buildPath;
		settingsPath.append(R"(Internal\Resources\Engine\GameSettings.settings)");
		EditorContext::Instance().windowManager->SaveBuildSettingsForBuild(settingsPath.string());
	}

	logger->Info("GameBuilder - Successfully export binary files");

	return true;
}

void GameBuilder::CopyResourceDirectoryRecurse(const fs::path& resourcesPathSrc, const fs::path& resourcesPathDest)
{
	Logger* logger = EngineContext::Instance().logger;

	if (!fs::create_directories(resourcesPathDest))
	{
		logger->Error("GameBuilder - Failed to create '" + resourcesPathDest.string() + "' drectory");
		return;
	}

	for (const auto& file : fs::directory_iterator(resourcesPathSrc))
	{
		fs::path filePath = file.path();

		// Recurse on directory
		if (file.is_directory())
		{
			std::string filename = filePath.filename().string();

			// Update directories
			fs::path newResourcesPathSrc = resourcesPathSrc;
			newResourcesPathSrc.append(filename);
			fs::path newResourcesPathDest = resourcesPathDest;
			newResourcesPathDest.append(filename);

			CopyResourceDirectoryRecurse(newResourcesPathSrc, newResourcesPathDest);

			continue;
		}

		std::string extension = filePath.extension().generic_string();
		if (extension == "")
			extension = filePath.filename().string();

		// Remove textures
		if (ResourcesLoader::GetExtensionType(extension) != ExtensionType::META &&
			ResourcesLoader::GetExtensionType(extension) != ExtensionType::TEXTURE &&
			ResourcesLoader::GetExtensionType(extension) != ExtensionType::RESOURCE)
			continue;

		// Copy the file in the build folder
		CopyFileFromFilename(filePath, resourcesPathDest);
	}
}

bool GameBuilder::ExportAssets(const std::filesystem::path& buildPath)
{
	Logger* logger = EngineContext::Instance().logger;

	logger->Info("GameBuilder - Copy asset files");

	// Copy Assets folder
	{
		fs::path buildAssetsPath = buildPath;
		buildAssetsPath.append("Assets");

		CopyResourceDirectoryRecurse(ASSETS_ROOT_RAW, buildAssetsPath);
	}

	// Copy Internal resources folder
	{
		// Create internal resources build folder
		fs::path buildInternalRes = buildPath;
		buildInternalRes.append(R"(Internal\Resources\Engine)");

		CopyResourceDirectoryRecurse(INTERNAL_ENGINE_RESOURCES_ROOT_RAW, buildInternalRes);
	}

	logger->Info("GameBuilder - Successfully export asset files");

	return true;
}

bool GameBuilder::BuildGame(const BuildData& buildData)
{
	Logger* logger = EngineContext::Instance().logger;

	if (buildData.startSceneUId.empty() ||
		EngineContext::Instance().resourcesManager->GetResource(HYGUID(buildData.startSceneUId)) == nullptr)
	{
		Logger::Warning("BuildGame - Start scene is empty, discard build");
		return false;
	}

	logger->Info("GameBuilder - Start building '" + buildData.folderName + "' at '" + buildData.buildPath + "'");

	fs::path buildFullPath = buildData.buildPath;
	buildFullPath.append(buildData.folderName);

	if (!CreateBuildFolder(buildFullPath, buildData) ||
		!ExportAssets(buildFullPath) ||
		!ExportBinaries(buildFullPath, buildData))
	{
		logger->Error("GameBuilder - Build '" + buildData.folderName + "' failed");
		return false;
	}

	logger->Success("GameBuilder - Build successful");

	return true;
}

void GameBuilder::BuildAndRunGame(const BuildData& buildData)
{
	// Build the game
	if (!BuildGame(buildData))
		return;

	// Prepare run game executable system command
	std::string buildFullPath = buildData.buildPath + buildData.folderName + R"(\Game.lnk)";
	std::string command = "start " + buildFullPath;

	// Run game executable
	system(command.c_str());
}