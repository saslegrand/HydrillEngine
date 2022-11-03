
inline bool StringHelper::RenameFile(fs::path path, std::string_view newFilename)
{
    try {
        std::string newPath = path.parent_path().string() + "\\" + std::string(newFilename);

        fs::rename(path, std::filesystem::path(newPath));
        return true;
    }
    catch (std::filesystem::filesystem_error const& ex) {
        Logger::Error("StringHelper - " + std::string(ex.what()));
        return false;
    }
}

inline bool StringHelper::RenameFile(const std::string& path, std::string_view newFilename)
{
    fs::path fsPath = path;
    return RenameFile(fsPath, newFilename);
}

inline bool StringHelper::MoveFile(fs::path path, std::string_view newDirLocation)
{
    try {
        std::string fileName = path.filename().string();

        std::string newPath = fs::path(newDirLocation).string() + "\\" + std::string(fileName);

        fs::rename(path, std::filesystem::path(newPath));
        return true;
    }
    catch (std::filesystem::filesystem_error const& ex) {
        Logger::Error("StringHelper - " + std::string(ex.what()));
        return false;
    }
}

inline bool StringHelper::MoveFile(const std::string& path, std::string_view newDirLocation)
{
    fs::path fsPath = path;
    return MoveFile(fsPath, newDirLocation);
}

inline void StringHelper::ReplaceAll(std::string& fileString, const std::string& strToReplace, const std::string& strToReplaceWith)
{
    // Cant replace void string
    if (strToReplace.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = fileString.find(strToReplace, start_pos)) != std::string::npos)
    {
        fileString.replace(start_pos, strToReplace.length(), strToReplaceWith);
        start_pos += strToReplaceWith.length(); // In case 'strToReplaceWith' contains 'strToReplace', like replacing 'x' with 'yx'
    }
}

inline std::string StringHelper::GetFileNameFromPath(const std::string& filepath)
{
    return fs::path(filepath).filename().string();
}

inline std::string StringHelper::GetFileNameFromPathWithoutExtension(const std::string& filepath)
{
    return fs::path(filepath).filename().replace_extension("").string();
}

inline std::string StringHelper::GetFileNameWithoutExtension(const std::string& filename)
{
    return fs::path(filename).replace_extension("").string();
}

inline std::string StringHelper::GetFilePathWithoutExtension(const std::string& filepath)
{
    return GetDirectory(filepath) + GetFileNameFromPathWithoutExtension(filepath);
}

inline std::string StringHelper::GetFileExtensionFromPath(const std::string& filepath)
{
    return fs::path(filepath).extension().string();
}

inline std::string StringHelper::GetDirectory(const std::string& filepath)
{
    return fs::path(filepath).remove_filename().string();
}