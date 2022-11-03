#include "Tools/FileDialogs.hpp"

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <IO/Window.hpp>


void FileDialogs::Initialize(Window* window)
{
	m_window = window;
}

std::filesystem::path FileDialogs::OpenFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(m_window->GetInstance());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return std::filesystem::path(ofn.lpstrFile);
	}

	return std::filesystem::path();
}

std::filesystem::path FileDialogs::SaveFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(m_window->GetInstance());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return std::filesystem::path(ofn.lpstrFile);
	}

	return std::filesystem::path();
}


static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		std::string tmp = (const char*)lpData;
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}


std::filesystem::path FileDialogs::OpenDirectory(const char* currentDir)
{

	char path[MAX_PATH];

	BROWSEINFOA bi = { 0 };
	bi.hwndOwner = glfwGetWin32Window(m_window->GetInstance());
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;
	bi.lpszTitle = ("Please choose a folder to build your Hydrill project into");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)currentDir;
	

	LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDListA(pidl, path);

		//free memory used
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		return std::filesystem::path(path);
	}

	return std::filesystem::path();
}