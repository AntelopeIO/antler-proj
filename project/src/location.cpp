/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/location.hpp>

#include <filesystem>
#include <iostream>
#include <boost/algorithm/string.hpp> // boost::split()

#include <antler/system/exec.hpp>


namespace antler::project::location {

bool is_archive(std::string_view s) {

   return
      s.ends_with(".tar.gz")
      || s.ends_with(".tgz")
      || s.ends_with(".tar.bz2")
      || s.ends_with(".tar.xz")
      || s.ends_with(".tar.zst")
      ;
}


bool is_github_archive(std::string_view s) {

   return s.starts_with("https://github.com") && is_archive(s);
}


bool is_github_repo(std::string_view s) {

   return s.starts_with("https://github.com") && !is_archive(s);
}


bool is_local_file(std::string_view s) {

   if (s.starts_with("https://github.com"))
      return false;

   if (s.starts_with("file://"))
      s = s.substr(7);

   std::error_code sec;
   return std::filesystem::exists(s, sec);
}


bool is_org_repo_shorthand(std::string_view s) {

   std::vector<std::string_view> splits;
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