#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string_view>

namespace antler::project::location {

/// @param l  Location to evaluate.
/// @return true if l looks like an archive (e.g. ends in `.tar.gz`, `.tar.bz2`, `.tar.zstd`).
[[nodiscard]] bool is_archive(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l looks like a github repository.
[[nodiscard]] bool is_github_repo(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l looks like an archive from github.
[[nodiscard]] bool is_github_archive(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l looks like a local file.
[[nodiscard]] bool is_local(std::string_view l);
/// @note requires `gh` is installed and user is authenticated or the repo is public.
/// @param l Location to evaluate.
/// @return true if l is a repo, calls `gh` to test.
[[nodiscard]] bool is_github_org_repo_shorthand(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l is reachable.
bool is_reachable(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l looks like a url.
bool is_url(std::string_view l);

} // namespace antler::project::location
