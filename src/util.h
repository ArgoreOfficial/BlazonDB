#pragma once
#include <filesystem>
#include <shlobj.h>

namespace Util {
    std::filesystem::path getFolderPath(const KNOWNFOLDERID& _id);
}

