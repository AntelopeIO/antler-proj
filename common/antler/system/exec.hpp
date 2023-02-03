#pragma once
#define antler_system_exec_hpp

/// @copyright See `LICENSE` in the root directory of this project.

#include <string_view>
#include <string>


namespace antler {
namespace system {

/// Structure to contain the results of exec().
struct result {

   int return_code = 0;         ///< This value contains the return value from the cmd.
   std::string output;          ///< This contains both stdout and stderr captured when cmd is called.


   // bool operator returns true when error state is set, false otherwise.
   // implementation for bool operator
   typedef void (*unspecified_bool_type)();
   static void unspecified_bool_true() { ; }
   operator unspecified_bool_type() const { return (return_code != 0 ? 0 : unspecified_bool_true); }

   /// @return true when there is NO error; false for error (oppositie of the bool operator
   bool operator!() const { return return_code != 0; }
};

/// Call `system()` with the string cmd. Results are captured and returned.
/// @param cmd  The command to call.
/// @return The result struct containing the integer result and captured stdout and stderr streams.
result exec(std::string_view cmd) noexcept;

} // namespace system
} // namespace antler

#include <antler/system/detail/exec.ipp>
