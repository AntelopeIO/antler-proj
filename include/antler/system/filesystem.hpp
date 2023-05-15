#pragma once

#if __has_include(<filesystem>)
#include <filesystem>
namespace antler::system
{
namespace fs = std::filesystem;
}
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace antler::system
{
namespace fs = std::experimental::filesystem;
}
#else
error "Missing the <filesystem> header."
#endif