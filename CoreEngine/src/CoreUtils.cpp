#include "CoreUtils.h"
#include <filesystem>

std::string Anubis::GetFileNameModern(const char* full_path)
{
    // 1. Create a path object from the macro output
    std::filesystem::path p(full_path);

    // 2. Use the filename() method
    // This correctly handles both '/' and '\' separators.
    return p.filename().string();
}
