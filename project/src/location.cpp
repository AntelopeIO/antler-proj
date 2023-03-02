/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <boost/algorithm/string.hpp> // boost::split()

#include <antler/system/exec.hpp>

#include <curl/curl.h>


namespace antler::project::location {

// TODO replace later with string_view.ends_with() and string_view.starts_with()
static inline bool ends_with(std::string_view src, std::string_view comp) {
   const std::size_t sz = comp.size();
   if (src.size() < sz)
      return false;
   
   return memcmp(&src[0] + src.size()-sz, &comp[0], sz) == 0;
}

static inline bool starts_with(std::string_view src, std::string_view comp) {
   const std::size_t sz = comp.size();
   if (src.size() < sz)
      return false;
   
   return memcmp(&src[0], &comp[0], sz) == 0;
}

// object to encapsulate curl
struct curl {
   inline curl() {
      curl_global_init(CURL_GLOBAL_ALL);
      curl_obj = curl_easy_init();
      if (!curl_obj)
         throw std::runtime_error("internal error, curl not initialized");
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
      if (res) {
         std::cerr << "Failure: " << curl_easy_strerror(res) << std::endl;
         throw std::runtime_error("internal curl failure");
      }
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

   CURL* curl_obj;
};

// object to encapsulate github api
struct github {
   inline github() = default;

   std::string request(std::string_view org, std::string_view repo) {
      bearer_token = sender.request("https://ghcr.io/token?service=registry.docker.io&scope=repository:AntelopeIO/experimental-binaries:pull");
      bearer_token = bearer_token.substr(bearer_token.find(":\"")+2);
      bearer_token = bearer_token.substr(0, bearer_token.size()-3);

      std::string url = "https://api.github.com/repos/";
      url += std::string(org) + std::string("/");
      url += repo;
      auto s = sender.authenticated_request(bearer_token, url.c_str());
      std::cout << s << std::endl;
      return s;
   }

   inline std::string request(std::string_view s) { return request(get_org(s), get_repo(s)); }

   inline bool is_reachable(std::string_view s) {
      try {
         (void)request(s);
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
      return s.substr(pos+1);
   }

   static inline bool is_shorthand(std::string_view s) {
      auto sub = s.substr(0, s.find_last_of("/"));
      return sub.find_last_of("/") == std::string_view::npos;
   }

   curl sender;
   std::string bearer_token;
};



bool is_archive(std::string_view s) {
   return ends_with(s, ".tar.gz")  ||
          ends_with(s, ".tgz")     ||
          ends_with(s, ".tar.bz2") ||
          ends_with(s, ".tar.xz")  ||
          ends_with(s, ".tar.zst");
}

static inline bool is_github(std::string_view s) { return starts_with(s, "https://github.com"); }

bool is_github_archive(std::string_view s) { return is_github(s) && is_archive(s); }

bool is_url(std::string_view l) { return curl::is_url(l); }

bool is_local_file(std::string_view s) { return std::filesystem::exists(s); }

bool is_github_org_repo_shorthand(std::string_view s) { return github::is_shorthand(s); }

bool is_github_repo(std::string_view s) { return is_github(s) && !is_archive(s); }

bool is_reachable(std::string_view l) {
   if (is_archive(l) || is_url(l) || is_github_archive(l)) {
      return curl{}.is_reachable(l);
   } else if (is_github_repo(l) || is_github_org_repo_shorthand(l)) {
      return github{}.is_reachable(l);
   } else if (is_local_file(l)) {
      return true;
   } else {
      return false;
   }
}

} // namespace antler::project::location
