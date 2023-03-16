/// @copyright See `LICENSE` in the root directory of this project.

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <antler/project/version.hpp>

extern "C" {
   int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
      using namespace antler::project;
      std::string ver = std::string(reinterpret_cast<const char *>(data), size);

      try {
         volatile version v{ver};
         return 0;
      } catch (...) {
         return -1;
      }
   }
}