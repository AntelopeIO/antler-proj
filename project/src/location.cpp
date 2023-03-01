/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <boost/algorithm/string.hpp> // boost::split()

#include <antler/system/exec.hpp>

#include <curl/curl.h>


namespace antler::project::location {

// object to encapsulate github api
struct github {
   static size_t write_data(void* ptr, size_t sz, size_t nm, void* s) {
      reinterpret_cast<std::string*>(s)->append(reinterpret_cast<char*>(ptr), sz*nm);
      return sz*nm;
   }
   inline github() {
      CURL* curl;
      curl_global_init(CURL_GLOBAL_ALL);
      curl = curl_easy_init();
      if (curl) {
         curl_easy_setopt(curl, CURLOPT_URL, "https://ghcr.io/token?service=registry.docker.io&scope=repository:AntelopeIO/experimental-binaries:pull");
         //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &github::write_data);
         std::string buff;
         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
         auto res = curl_easy_perform(curl);
         if (res) {
            std::cerr << curl_easy_strerror(res) << std::endl;
            throw std::runtime_error("internal curl failure");
         }
         curl_easy_cleanup(curl);
         bearer_token = buff.substr(buff.find(":\"")+2);
         bearer_token = bearer_token.substr(0, bearer_token.size()-3);
      }
   }

   std::string bearer_token;
};

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

bool is_archive(std::string_view s) {
   return ends_with(s, ".tar.gz")  ||
          ends_with(s, ".tgz")     ||
          ends_with(s, ".tar.bz2") ||
          ends_with(s, ".tar.xz")  ||
          ends_with(s, ".tar.zst");
}

inline bool is_github(std::string_view s) { return starts_with(s, "https://github.com"); }

bool is_github_archive(std::string_view s) { return is_github(s) && is_archive(s); }

bool is_github_repo(std::string_view s) { return is_github(s) && !is_archive(s); }

bool is_local_file(std::string_view s) {

   if (is_github(s))
      return false;

   if (starts_with(s, "file://"))
      s = s.substr(7);

   std::error_code sec;
   return std::filesystem::exists(s, sec);
}


bool is_org_repo_shorthand(std::string_view s) {

   std::vector<std::string> splits;
   boost::split(splits, s, [](const char c) { return c == '/'; });
   if (splits.size() != 2)
      return false;

   // Set ss to the command string.
   std::ostringstream ss;
   ss << "gh repo view " << s;

   // Call the command, test the result.
   return system::exec(ss.str()).return_code != 0;
}


} // namespace antler::project::location
