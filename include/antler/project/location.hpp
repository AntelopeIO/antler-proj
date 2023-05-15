#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string_view>
#include "../system/utils.hpp"
#include "net_utils.hpp"

namespace antler::project::location {

/// If location is github URL then strips out `https://github.com/` from URL. Otherwise returns input argument.
/// @param loc Github location - either full URL or shorthand
/// @return string_view location striped from `https://github.com/`
[[nodiscard]] std::string_view strip_github_com(std::string_view location);

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
[[nodiscard]] bool is_reachable(std::string_view l);

/// @param l  Location to evaluate.
/// @return true if l looks like a url.
[[nodiscard]] bool is_url(std::string_view l);

/// @param org github organization name
/// @param repo git repository name
/// @param branch git branch name
/// @param dest directory to clone to
/// @return true if successful
[[nodiscard]] bool clone_github_repo(const std::string& org, const std::string& repo, const std::string& branch, system::fs::path dest = ".");

/// @param url git repository url
/// @param branch git branch name
/// @param dest directory to clone to
/// @return true if successful
[[nodiscard]] bool clone_git_repo(const std::string& url, const std::string& branch, system::fs::path dest = ".");

/// @note perform git pull for specific directory
/// @param src working directory
/// @return true if successful
[[nodiscard]] bool pull_git_repo(system::fs::path src);

/// @note fetches repos json using github API
/// @param org github organization
/// @param repo git repository name
/// @return json string
[[nodiscard]] std::string github_request(const std::string& org, const std::string& repo);

/// @param org github organization name
/// @param repo github repository name
/// @return branch name
[[nodiscard]] std::string get_github_default_branch(const std::string& org, const std::string& repo);

}  // namespace antler::project::location
