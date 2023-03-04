/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version.hpp>
#include <antler/string/from.hpp>

#include <boost/algorithm/string.hpp> // boost::split()

#include <sstream>



namespace antler::project {

//--- alphabetic --------------------------------------------------------------------------------------------------------


void version::clear() noexcept {
   major_comp = 0;
   minor_comp = 0;
   patch_comp = 0;
   tweak_comp = 0;
}


bool version::empty() const noexcept {
   return major() == 0 &&
          minor() == 0 &&
          patch() == 0 &&
          tweak().empty();
}

} // namespace antler::project