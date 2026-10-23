#include "util.h"

#include <Windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Ole32.lib")

std::filesystem::path Util::getFolderPath(const KNOWNFOLDERID& _id)
{
    wchar_t* wpath = nullptr;
    if (SHGetKnownFolderPath(_id, 0, NULL, &wpath) != S_OK)
    {
        if (wpath) CoTaskMemFree(static_cast<void*>(wpath));
        return "";
    }

    std::filesystem::path path = wpath;
    CoTaskMemFree(static_cast<void*>(wpath));
    return path;
}


