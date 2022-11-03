#pragma once

#include <string>
#include <filesystem>

#include "EngineDll.hpp"
#include "Core/Logger.hpp"

namespace fs = std::filesystem;

namespace StringHelper
{
    /*
    @brief Rename filename

    @return bool : Has succeeded to rename the file
    */
    bool RenameFile(const std::string& path, std::string_view newFilename);

    /*
    @brief Rename filename

    @return bool : Has succeeded to rename the file
    */
    bool RenameFile(fs::path path, std::string_view newFilename);


    /*
    @brief Move a file to another directory

    @return bool : Has succeeded to move the file
    */
    bool MoveFile(const std::string& path, std::string_view newDirLocation);

    /*
    @brief Move a file to another directory

    @return bool : Has succeeded to move the file
    */
    bool MoveFile(fs::path path, std::string_view newDirLocation);

    /*
    @brief Replace all specific string with another given string in a filestring

    @param fileString : string to search on
    @param strToReplace : string 'from' to replace
    @param strToReplaceWith : string 'to' to replace with
    */
    void ReplaceAll(std::string& fileString, const std::string& strToReplace, const std::string& strToReplaceWith);

    /*
    @brief Retreive filename from filepath string

    @return std::string : filename
    */
    std::string GetFileNameFromPath(const std::string& filepath);

    /*
    @brief Retreive filename without extension from filepath string

    @return std::string : filename without extension
    */
    std::string GetFileNameFromPathWithoutExtension(const std::string& filepath);


    /*
    @brief Retreive filename without extension from filename string
    
    @return std::string : filename without extension
    */
    std::string GetFileNameWithoutExtension(const std::string& filename);

    /*
   @brief Retreive filepath without extension from filepath string

   @return std::string : filepath without extension
   */
    std::string GetFilePathWithoutExtension(const std::string& filepath);

    /*
    @brief Retreive file extension from filepath string

    @return std::string : file extension
    */
    std::string GetFileExtensionFromPath(const std::string& filepath);

    /*
    @brief Retreive file directory from filepath string

    @return std::string : file directory
    */
    std::string GetDirectory(const std::string& filepath);

    /*
    @brief Change string to only upper case character

    @param str : string to modify

    @return std::string : string modified
    */
    ENGINE_API std::string ToUpper(std::string str);

    /*
    @brief Check if a string contains another one (ignore upper/lower case)

    @param str1 : string to check
    @param str2 : string to find in str1

    @return bool : true if contained, false otherwise
    */
    ENGINE_API bool StrStrCaseUnsensitive(const std::string& str1, const std::string& str2);

}

#include "Tools/StringHelper.inl"
