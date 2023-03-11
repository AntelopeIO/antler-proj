/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/system/version.hpp>

#include "token.hpp"

namespace antler::project {

void project::print(std::ostream& os) const noexcept {
   // Add a header.
   os << magic_comment << "\n";
   os << comment_preamble << "\n";
   os << "#   generated with v" << system::version::full() << std::endl;

   os << "#\n"
      << "# This file was auto-generated. Be aware antler-proj may discard added comments.\n"
      << "\n\n";
   os << to_yaml();
}

} // namespace antler::project