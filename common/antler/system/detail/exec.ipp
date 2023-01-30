#ifndef antler_system_exec_h
#error "bad inclusion"
#endif

/// @copyright See `LICENSE` in the root directory of this project.

#include <stdio.h>
#include <array>
#include <sstream>

namespace antler {
namespace system {

inline result exec(std::string_view cmd_in) noexcept {

   // We get stdout + stderr.
   std::string cmd(cmd_in);
   cmd += " 2>&1";

   result rv;

   // Open the pipe...
   FILE* pipe = popen(cmd.c_str(), "r");
   if (!pipe) {
      // ...let the user know on failure.
      rv.return_code = -1;
      rv.output = "antler::system::exec() error: failed to open pipe.";
      return rv;
   }

   std::array<char, 256> buffer;
   while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
      rv.output += buffer.data();

   rv.return_code = pclose(pipe);

   return rv;
}


} // namespace system
} // namespace antler
