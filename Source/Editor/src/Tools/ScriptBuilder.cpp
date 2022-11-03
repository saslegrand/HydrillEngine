#include "Tools/ScriptBuilder.hpp"

#include <fstream>
#include <filesystem>

#include <EngineContext.hpp>
#include <Tools/StringHelper.hpp>
#include <Core/Logger.hpp>

#include "PathConfig.hpp"


#define ITEM_GROUP_INCLUDE(group, path) std::string("    <" + group + R"(=")" + path + R"(" />)" + '\n');

#ifdef PUBLISHED
constexpr char SCRIPTING_ROOT[] = R"(..\Source\Scripting\)";
constexpr char SCRIPTING_VCXPROJ[] = R"(..\Visual\Source\Scripting\Scripting.vcxproj)";
#else
constexpr char SCRIPTING_ROOT[] = R"(..\..\..\Source\Scripting\)";
constexpr char SCRIPTING_VCXPROJ[] = R"(..\Scripting\Scripting.vcxproj)";
#endif

const std::string ScriptBuilder::templateFileAccess = INTERNAL_EDITOR_RESOURCES_ROOT + std::string("ScriptTemplate");
const std::string ScriptBuilder::scriptNameToReplace = "$$SCRIPT_NAME$$";


template <typename Type>
bool IsFileOpen(const Type& stream, const std::string& filePath) 
    requires std::is_base_of_v<std::ios, Type>
{
    if (!stream.is_open())
    {
        Logger::Error("ScriptBuilder - Failed to open : '" + filePath + "' is inaccessible");
        return false;
    }

    return true;
}

#pragma region SCRIPT_CONSTRUCTION

void ScriptBuilder::RewriteScriptingProjectFile(const std::string& fileString)
{
    // Open write stream
    std::ofstream writeStream(SCRIPTING_VCXPROJ);

    if (!IsFileOpen(writeStream, SCRIPTING_VCXPROJ))
        return;

    // Check if the file stream is correctly opened
    if (!writeStream.is_open())
        return;

    // Clear file, write new info, close file
    writeStream.clear();
    writeStream << fileString;
    writeStream.close();
}

void ScriptBuilder::WriteFileOnScirptingProject(const std::string& scriptPath, const std::string& itemGroupTarget)
{
    // Get vcxproj group target
    std::string groupTarget = itemGroupTarget + " Include";

    // Open read stream
    std::ifstream readStream (SCRIPTING_VCXPROJ);

    // Check if the file stream is correctly opened
    if (!IsFileOpen(readStream, SCRIPTING_VCXPROJ))
        return;

    // Get vcxproj file into a string
    std::string line, fileString;
    bool lineAdded = false;
    while (std::getline(readStream, line))
    {
        // Check for adding the ItemGroup line in the file
        if (!lineAdded)
        {
            size_t start_pos = 0;
            if ((start_pos = line.find(groupTarget, start_pos)) != std::string::npos)
            {
                // Add ItemGroup line
                fileString += ITEM_GROUP_INCLUDE(groupTarget, scriptPath);
                lineAdded = true;
            }
        }

        // Write file line on string
        fileString += line + '\n';
    }

    readStream.close();

    // Rewrite scripting vcxproj
    RewriteScriptingProjectFile(fileString);
}

void ScriptBuilder::ConstructFile(const std::string& scriptName, const std::string& subDir, const std::string& fileExtension, const std::string& groupTarget)
{
    // Get script full path
    std::string scriptPath = SCRIPTING_ROOT + subDir + scriptName + fileExtension;
    std::string templatePath = templateFileAccess + fileExtension;

    // Create read/write streams
    std::ofstream outStream(scriptPath);
    std::ifstream readStream(templatePath);

    // Check if the file streams are correctly opened
    if (!IsFileOpen(outStream, scriptPath) ||
        !IsFileOpen(readStream, templatePath))
        return;

    // Get template file into a string
    std::string fileString, line;
    while (std::getline(readStream, line))
        fileString += line + '\n';

    // Replace all macro $$SCRIPT_NAME$$ with the new script name
    StringHelper::ReplaceAll(fileString, scriptNameToReplace, scriptName);

    // Write on out file
    outStream << fileString;

    // Close streams
    outStream.close();
    readStream.close();

#ifdef PUBLISHED
    scriptPath = R"(..\..\)" + scriptPath;
#endif

    // Write file include in scripting vcxproj
    WriteFileOnScirptingProject(scriptPath, groupTarget);
}

void ScriptBuilder::ConstructScript(std::string_view scriptName)
{
    // Avoid create unnamed scripts
    if (scriptName.empty())
        return;

    // Construct hpp script file
    ConstructFile(scriptName.data(), R"(include\)", ".hpp", "ClInclude");

    // Construct cpp script file
    ConstructFile(scriptName.data(), R"(src\)", ".cpp", "ClCompile");
}

#pragma endregion


#pragma region SCRIPT_DESTRUCTION

void ScriptBuilder::UnwriteFileOnScirptingProject(const std::string& scriptName)
{
    // Open read stream
    std::ifstream readStream(SCRIPTING_VCXPROJ);

    if (!IsFileOpen(readStream, SCRIPTING_VCXPROJ))
        return;

    // Get vcxproj file into a string
    std::string line, fileString;
    bool lineDeleted = false;
    while (std::getline(readStream, line))
    {
        // Check for the line where the script file is added
        if (!lineDeleted)
        {
            size_t start_pos = 0;
            if ((start_pos = line.find(scriptName, start_pos)) != std::string::npos)
            {
                // Remove line (dont wrote it again)
                lineDeleted = true;
                continue;
            }
        }

        // Write file line on string
        fileString += line + '\n';
    }

    readStream.close();

    // Rewrite scripting vcxproj
    RewriteScriptingProjectFile(fileString);
}

void ScriptBuilder::DestructFile(const std::string& scriptName, const std::string& subDir, const std::string& fileExtension)
{
    // Get script full path
    std::filesystem::path fullPath = SCRIPTING_ROOT + subDir + scriptName + fileExtension;

    if (std::filesystem::exists(fullPath))
    {
        if (!std::filesystem::remove(fullPath))
        {
            Logger::Error("ScriptBuilder - Failed to remove : " + fullPath.generic_string());
            return;
        }
    }
    
    UnwriteFileOnScirptingProject(scriptName);
}

void ScriptBuilder::DestructScript(std::string_view scriptName)
{
    // Avoid create not named scripts
    if (scriptName.empty())
        return;

    // Construct hpp script file
    DestructFile(std::string(scriptName.data()), R"(include\)", ".hpp");

    // Construct cpp script file
    DestructFile(std::string(scriptName.data()), R"(src\)", ".cpp");
}

#pragma endregion