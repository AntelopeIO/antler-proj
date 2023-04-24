/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/net_utils.hpp>
#include <antler/project/location.hpp>
#include <antler/system/utils.hpp>

#include <cstring>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>
#include <nlohmann/json.hpp>


namespace antler::project::location {

bool is_archive(std::string_view s) {
   return ends_with(s, ".tar.gz")  ||
         ends_with(s, ".tgz")     ||
         ends_with(s, ".tar.bz2") ||
         ends_with(s, ".tar.xz")  ||
         ends_with(s, ".tar.zst");
}

constexpr std::string_view github_com = "https://github.com/";

std::string_view normalize(std::string_view loc) {
   if (is_github_repo(loc)) {
      return strip_github_url_to_shorthand(loc);
   }   

   return loc;
}

static inline bool is_github(std::string_view s) { return starts_with(s, github_com); }

bool is_github_archive(std::string_view s) { return is_github(s) && is_archive(s); }

bool is_url(std::string_view l) { return curl::is_url(l); }

bool is_github_shorthand(std::string_view s) { return github::is_shorthand(s); }

bool is_github_repo(std::string_view s) { return is_github(s) && !is_archive(s); }

std::string_view strip_github_url_to_shorthand(std::string_view github_url) {
   if(github_url.size() <= github_com.size())
      throw std::out_of_range("URL passed: "+  std::string(github_url) + " should have minimum size of " + std::to_string(github_com.size() + 1));
   
   return github_url.substr(github_com.size());
}

bool is_reachable(std::string_view l) {
   if (!is_github_shorthand(l)) {
      system::error_log("In this version of antler-proj only github shorthands (i.e. org/project) and github URLs (i.e. https://github.com/org/project) are supported. Archives will be supported in a future version.");
      return false;
   }

   return github{}.is_reachable(l);
   // TODO add support for archives
   if (is_github_repo(l) || is_github_shorthand(l)) {
      return github{}.is_reachable(l);
   } else if (is_archive(l) || is_url(l) || is_github_archive(l)) {
      return curl{}.is_reachable(l);
   } else {
      return false;
   }
}

bool clone_github_repo(const std::string& org, const std::string& repo, const std::string& branch, uint32_t jobs, system::fs::path dest) {
   return git::clone(org, repo, branch, jobs, dest);
}

bool clone_git_repo(const std::string& url, const std::string& branch, uint32_t jobs, system::fs::path dest) {
   return git::clone(url, branch, jobs, dest);
}

bool pull_git_repo(system::fs::path src) { return git::pull(src); }

std::string github_request(const std::string& org, const std::string& repo) {
   return github{}.request(org, repo);
}

std::string get_github_default_branch(const std::string& org, const std::string& repo) {
   return github{}.get_default_branch(org, repo);
}

} // namespace antler::project::location
