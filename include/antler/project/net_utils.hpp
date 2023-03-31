#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/system/utils.hpp>

#include <cstring>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>
#include <nlohmann/json.hpp>


namespace antler::project {

   // TODO replace later with string_view.ends_with() and string_view.starts_with()
   static inline bool ends_with(std::string_view src, std::string_view comp) {
      const std::size_t sz = comp.size();
      if (src.size() < sz)
         return false;

      return std::memcmp(&src[0] + src.size()-sz, &comp[0], sz) == 0;
   }

   static inline bool starts_with(std::string_view src, std::string_view comp) {
      const std::size_t sz = comp.size();
      if (src.size() < sz)
         return false;

      return std::memcmp(&src[0], &comp[0], sz) == 0;
   }

   // object to encapsulate curl
   struct curl {
      inline curl() {
         curl_global_init(CURL_GLOBAL_ALL);
         curl_obj = curl_easy_init();
         ANTLER_CHECK(curl_obj, "internal failure, curl_easy_init() failed");
      }

      ~curl() {
         curl_easy_cleanup(curl_obj);
      }

      static std::size_t write_data(void* ptr, size_t sz, size_t nm, void* s) {
         reinterpret_cast<std::string*>(s)->append(reinterpret_cast<char*>(ptr), sz*nm);
         return sz*nm;
      }

      std::string authenticated_request(std::string_view token, std::string_view url) {
         curl_easy_setopt(curl_obj, CURLOPT_XOAUTH2_BEARER, token.data());
         return request(url);
      }

      std::string request(std::string_view url) {
         curl_easy_setopt(curl_obj, CURLOPT_URL, url.data());
         curl_easy_setopt(curl_obj, CURLOPT_WRITEFUNCTION, &curl::write_data);
         curl_easy_setopt(curl_obj, CURLOPT_USERAGENT, "antler-proj");

         std::string buff;

         curl_easy_setopt(curl_obj, CURLOPT_WRITEDATA, &buff);

         auto res = curl_easy_perform(curl_obj);
         ANTLER_CHECK(!res, "internal failure, curl_easy_perform() failed with error {0}", curl_easy_strerror(res));
         return buff;
      }

      inline static bool is_url(std::string_view url) { return starts_with(url, "http:") || starts_with(url, "https:"); }
      inline bool is_reachable(std::string_view url) {
         try {
            (void)request(url);
            return true;
         } catch(...) {
            return false;
         }
      }

   private:
      CURL* curl_obj;
   };

   // object to encapsulate github api
   struct github {
      inline github() {
         has_gh_app = system::execute_quiet("gh", {"--version"}) == 0;
         system::debug_log("Found and using github app!");

         if (!has_gh_app) {
            bearer_token = sender.request("https://ghcr.io/token?service=registry.docker.io&scope=repository:AntelopeIO/experimental-binaries:pull");
            bearer_token = bearer_token.substr(bearer_token.find(":\"")+2);
            bearer_token = bearer_token.substr(0, bearer_token.size()-3);
            system::debug_log("Using default bearer token: {0}", bearer_token);
         } else {
            bearer_token = system::execute("gh", {"auth", "token"});
            system::debug_log("Using github app bearer token: {0}", bearer_token);
         }
      }

      std::string request(std::string_view org, std::string_view repo) {
         std::string url = "https://api.github.com/repos/";
         url += std::string(org) + std::string("/");
         url += repo;
         auto s = sender.authenticated_request(bearer_token, url.c_str());
         return s;
      }

      inline std::string request(std::string_view s) { return request(get_org(s), get_repo(s)); }

      inline std::string get_default_branch(std::string_view org, std::string_view repo) {
         auto js = nlohmann::json::parse(request(org, repo));
         auto def_branch = js["default_branch"];
         ANTLER_CHECK(!def_branch.is_null(), "github api failure, default branch not found");
         return def_branch.get<std::string>();
      }

      inline bool is_reachable(std::string_view s) {
         try {
            auto js = nlohmann::json::parse(request(s));
            auto msg = js["message"];
            if (!msg.is_null())
               if (msg == "Not Found")
                  return false;
            return true;
         } catch(...) {
            return false;
         }
      }

      static std::string_view get_org(std::string_view s) {
         auto sub = s.substr(0, s.find_last_of("/"));
         auto pos = sub.find_last_of("/");
         return pos == std::string_view::npos ?
               sub :
               sub.substr(pos+1);
      }

      static std::string_view get_repo(std::string_view s) {
         auto pos = s.find_last_of("/");
         if (pos == std::string_view::npos)
            return "";
         return s.substr(pos+1);
      }

      static inline bool is_shorthand(std::string_view s) {
         auto sub = s.substr(0, s.find_last_of("/"));
         return sub.size() != s.size() && sub.find_last_of("/") == std::string_view::npos;
      }

   private:
      curl sender;
      std::string bearer_token = "";
      bool has_gh_app          = false;
   };

   struct git {
      static inline constexpr std::string_view executable = "git";

      /// @brief clone a repo from github
      /// @param org
      /// @param repo
      /// @param branch
      static bool clone(const std::string& org, const std::string& repo, const std::string& branch, uint32_t jobs, const system::fs::path& dest) {
         int32_t ret = system::execute(std::string(executable), { "clone", "-j", std::to_string(jobs), "--recurse-submodules", "--remote-submodules", "https://github.com/"+org+"/"+repo, "--depth", "1", "--branch", branch, dest.string() });
         system::debug_log("clone for {0}/{1} returned {2}\n", org, repo, ret);
         return ret == 0;
      }

      /// @brief clone a repo from git
      /// @param url
      /// @param branch
      static bool clone(const std::string& url, const std::string& branch, uint32_t jobs, const system::fs::path& dest) {
         int32_t ret = system::execute(std::string(executable), { "clone", "-j", std::to_string(jobs), "--recurse-submodules", "--remote-submodules", url, "--depth", "1", "--branch", branch, dest.string() });
         system::debug_log("clone for {0} returned {2}\n", url, ret);
         return ret == 0;
      }

      /// @brief fetch from a repo from github
      /// @param org
      /// @param repo
      /// @param branch
      static bool pull(const system::fs::path& src) {
         int32_t ret = system::execute(std::string(executable), { "-C", src.string(), "pull" });
         system::debug_log("pull for {0} returned {2}\n", src.string(), ret);
         return ret == 0;
      }
   };



} // namespace antler::project
