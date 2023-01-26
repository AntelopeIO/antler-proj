#ifndef antler_project_version_h
#define antler_project_version_h

#include <antler/project/semver.h>

#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <memory>



/// For semver requirements see: https://semver.org/spec/v2.0.0-rc.1.html
///
/// @TODO - We need to improve pre-release as it's not currently calculated correctly.
///         See https://semver.org/spec/v2.0.0-rc.1.html#spec-item-10
///
/// @TODO - We need to add build number.
///         See https://semver.org/spec/v2.0.0-rc.1.html#spec-item-11
///
/// @TODO convert semver to struct { array<int,3>; string pre_release; string build; }
///         See https://semver.org/spec/v2.0.0-rc.1.html#spec-item-12  for precedence.
///


namespace antler {
namespace project {

class version {
public:
   using self = version;

   version();
   version(std::string_view ver);
   version(const semver& sv);
   version(const self& rhs);

   version& operator=(std::string_view ver);
   version& operator=(const semver& sv);
   version& operator=(const self& rhs);

   /// Clear any values.
   void clear() noexcept;

   bool empty() const noexcept;
   std::string_view raw() const noexcept;

   bool is_semver() const noexcept;
   /// @return The version in semver_t format. If is_semver() would return false, this value is invalid.
   operator semver() const noexcept;

   bool operator==(const self& rhs) const noexcept;
   bool operator!=(const self& rhs) const noexcept;
   bool operator<(const self& rhs) const noexcept;
   bool operator<=(const self& rhs) const noexcept;
   bool operator>(const self& rhs) const noexcept;
   bool operator>=(const self& rhs) const noexcept;

private:
   enum class cmp {
      eq,
      lt,
      gt,
   };

   static cmp raw_compare(std::string_view l_in, std::string_view r_in) noexcept;
   cmp compare(const self& rhs) const noexcept;


   void load(std::string_view s);
   void load(const semver& sv);

private:
   std::string m_raw;
   std::unique_ptr<semver> m_semver;
};


} // namespace project
} // namespace antler

inline std::ostream& operator<<(std::ostream& os, const antler::project::version& o) { os << o.raw(); return os; }
//std::istream& operator>>(std::istream& is, antler::project::object::version& e);



#endif
