/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <filesystem>
#include <iostream>
#include <boost/algorithm/string.hpp> // boost::split()

#include <antler/system/exec.hpp>


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

bool is_archive(std::string_view s) {

   return ends_with(s, ".tar.gz")  ||
          ends_with(s, ".tgz")     ||
          ends_with(s, ".tar.bz2") ||
          ends_with(s, ".tar.xz")  ||
          ends_with(s, ".tar.zst");
}


bool is_github_archive(std::string_view s) {

   return starts_with(s, "https://github.com") && is_archive(s);
}


bool is_github_repo(std::string_view s) {

   return starts_with(s, "https://github.com") && !is_archive(s);
}


bool is_local_file(std::string_view s) {

   if (starts_with(s, "https://github.com"))
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
   ss << "gh repo list " << splits[0] << " --json name";

   // Call the command, test the result.
   const auto result = system::exec(ss.str());
   if (!result) {
      std::cerr << result.output << "\n";
      return false;
   }

   // Reset/clear the stream and write the search string into it.
   ss.str(""); // Reset the stream.
   ss << "\"name\":\"" << splits[1] << "\"";

   // Search for the repo.
   return result.output.find(ss.str()) != std::string::npos;
}


} // namespace antler::project::location
