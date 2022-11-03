#pragma once

#include <string>

/**
@brief Util class to manage all scripts
*/
class ScriptBuilder
{
	/** Name of the macro in the remplate file */
	static const std::string scriptNameToReplace;

	/** Path without extension of the template file */
	static const std::string templateFileAccess;

	/**
	Copy the script template file and replace all name macro. A source file is created in the scripting directory.
	It will also add the file in the scripting visual studio solution.

	@param scriptName		: Name use for creating the files and the class
	@param subDir			: Sub directory where the file need to be created (include/src)
	@param fileExtension	: Extension of the file to create (.hpp/.cpp)
	@param groupTarget		: Item group include type for scripting vcxproj (ClInclude/ClCompile)
	*/
	static void ConstructFile(const std::string& scriptName, const std::string& subDir, const std::string& fileExtension, const std::string& groupTarget);

	/**
	Destruct a script file given a name. Item group include in the scripting projet is also removed.

	@param scriptName		: Name of the script to delete
	@param subDir			: Sub directory where the file is located (include/src)
	@param fileExtension	: Extension of the current file (.hpp/.cpp)
	*/
	static void DestructFile(const std::string& scriptName, const std::string& subDir, const std::string& fileExtension);

	/**
	Copy the scripting project file and add the item group include line.
	This is done to access the file in the visual studio solution.

	@param scriptPath		: Path where the script is located
	@param subDir			: Sub directory where the file is located (include/src)
	@param groupTarget		: Item group include type for scripting vcxproj (ClInclude/ClCompile)
	*/
	static void WriteFileOnScirptingProject(const std::string& scriptPath, const std::string& itemGroupTarget);

	/**
	Copy the scripting project file and remove the item group include line.
	This is done to remove the access of the file in the visual studio solution.

	@param scriptName		: Name of the script
	*/
	static void UnwriteFileOnScirptingProject(const std::string& scriptName);

	/**
	Clear the scripting project file and rewrite from the fileString

	@param fileString		: New scripting project file content
	*/
	static void RewriteScriptingProjectFile(const std::string& fileString);

public:
	/**
	Construct the header and source files of the new script from template files.
	Created files are also added to the scripting solution.

	@param scriptName		: Name use for creating the files and the class
	*/
	static void ConstructScript(std::string_view scriptName);

	/**
	Destruct the header and source files of the script given its name.
	Created files are also removed from the scripting solution.

	@param scriptName		: Name use for finding the script files, abort if files doesnt exist
	*/
	static void DestructScript(std::string_view scriptName);
};