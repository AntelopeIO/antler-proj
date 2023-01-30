#ifndef antler_project_location_h
#define antler_project_location_h

/// @copyright See `LICENSE` in the root directory of this project.


#include <string_view>

namespace antler {
namespace project {
namespace location {

/// @param s  String to eveluate.
/// @return true if s looks like an archive (e.g. ends in `.tar.gz`, `.tar.bz2`, `.tar.zstd`).
bool is_archive(std::string_view s);
/// @param s  String to eveluate.
/// @return true if s looks like a github repository.
bool is_github_repo(std::string_view s);
/// @param s  String to eveluate.
/// @return true if s looks like an archive from github.
bool is_github_archive(std::string_view s);
/// @param s  String to eveluate.
/// @return true if s looks like a local file.
bool is_local_file(std::string_view s);
/// @note requires `gh` is installed and user is authenticated or the repo is public.
/// @param s  String to eveluate.
/// @return true if s is a repo, calls `gh` to test.
bool is_org_repo_shorthand(std::string_view s);

} // namespace location
} // namespace project
} // namespace antler

#endif
