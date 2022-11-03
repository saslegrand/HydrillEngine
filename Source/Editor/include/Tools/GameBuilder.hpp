#pragma once

#include <string_view>

struct BuildData;

namespace std::filesystem
{
	class path;
	enum class copy_options;
}

/**
@brief Util class to build the game from the editor
*/
class GameBuilder
{
private:
	/**
	@brief Export assets project files and internal resources on build directory

	@param buildPath : Path of the build
	*/
	static bool ExportAssets(const std::filesystem::path& buildPath);

	/**
	@brief Export binaries project files and create Game.exe shortcut at the root

	@param buildPath : Path of the build
	@param buildData : Build settings
	*/
	static bool ExportBinaries(const std::filesystem::path& buildPath, const BuildData& buildInfo);

	/**
	@brief Create the root build folder from build settings

	@param buildPath : Path of the build
	@param buildData : Build settings
	*/
	static bool CreateBuildFolder(const std::filesystem::path& buildPath, const BuildData& buildInfo);

	/**
	@brief Copy a file (srcFile) at a given destination, automatically add filename from srcFile to destir

	@param srcFile : File to copy
	@param destDir : Directory to copy the file to
	*/
	static bool CopyFileFromFilename(const std::filesystem::path& srcFile, const std::filesystem::path& destDir);

	/**
	@brief Copy a directory (srcDir) at a given destination (destDir)

	@param srcDir : Directory to copy
	@param destDir : Directory to copy the file to
	*/
	static bool CopyDirectory(const std::filesystem::path& srcDir, const std::filesystem::path& destDir, std::filesystem::copy_options options);

	/**
	@brief Copy recursively a resource directory

	@param srcDir : Directory to copy
	@param destDir : Directory to copy the file to
	*/
	static void CopyResourceDirectoryRecurse(const std::filesystem::path& resourcesPathSrc, const std::filesystem::path& resourcesPathDest);

public:
	/**
	@brief Build a game from build settings

	@param buildData : Build settings
	*/
	static bool BuildGame(const BuildData& buildData);

	/**
	@brief Build a game from settings and run it

	@param buildData : Build settings
	*/
	static void BuildAndRunGame(const BuildData& buildData);
};