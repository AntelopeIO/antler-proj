#ifndef antler_system_exec_h
#define antler_system_exec_h

#include <string_view>
#include <string>


namespace antler {
namespace system {

struct result {

   int return_code = 0;
   std::string output;


   // bool operator returns true when error state is set, false otherwise
   // implementation for bool operator
   typedef void (*unspecified_bool_type)();
   static void unspecified_bool_true() { ; }
   operator unspecified_bool_type() const { return (return_code != 0 ? 0 : unspecified_bool_true); }

   /// @return true when there is NO error; false for error (oppositie of the bool operator
   bool operator!() const { return return_code != 0; }
};


result exec(std::string_view cmd) noexcept;

} // namespace system
} // namespace antler

#include <antler/system/detail/exec.ipp>

#endif
