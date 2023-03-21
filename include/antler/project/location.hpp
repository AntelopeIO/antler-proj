#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string_view>
#include "../system/utils.hpp"
#include "net_utils.hpp"

namespace antler::project::location {

/// @param l  Location to evaluate.
/// @return true if l looks an archive.
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
[[nodiscard]] bool is_github_shorthand(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l is reachable.
bool is_reachable(std::string_view l);
/// @param l  Location to evaluate.
/// @return true if l looks like a url.
bool is_url(std::string_view l);

bool clone_github_repo(const std::string& org, const std::string& repo, const std::string& branch, uint32_t jobs, system::fs::path dest = ".");
bool clone_git_repo(const std::string& url, const std::string& branch, uint32_t jobs, system::fs::path dest = ".");
bool pull_git_repo(system::fs::path src);
std::string github_request(const std::string& org, const std::string& repo);
std::string get_github_default_branch(const std::string& org, const std::string& repo);

} // namespace antler::project::location
