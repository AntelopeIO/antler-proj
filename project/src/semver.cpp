/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>
#include <antler/string/from.hpp>
#include <antler/string/split.hpp>

#include <limits>
#include <vector>
#include <iostream>
#include <sstream>



namespace antler::project {


//--- constructors/destrructor ------------------------------------------------------------------------------------------

semver::semver(value_type x, value_type y, value_type z, std::string_view pre_release, std::string_view build) noexcept
   : m_xyz{ x, y, z }
   , m_pre{ pre_release }
   , m_build{ build }
{
}


//--- operators ---------------------------------------------------------------------------------------------------------

bool semver::operator==(const self& rhs) const noexcept {
   return compare(rhs) == cmp_result::eq;
}


bool semver::operator!=(const self& rhs) const noexcept {
   return compare(rhs) != cmp_result::eq;
}


bool semver::operator<(const self& rhs) const noexcept {
   return compare(rhs) == cmp_result::lt;
}


bool semver::operator<=(const self& rhs) const noexcept {
   return compare(rhs) != cmp_result::gt;
}


bool semver::operator>(const self& rhs) const noexcept {
   return compare(rhs) == cmp_result::gt;
}


bool semver::operator>=(const self& rhs) const noexcept {
   return compare(rhs) != cmp_result::lt;
}


//--- alphabetic --------------------------------------------------------------------------------------------------------

void semver::clear() noexcept {
   m_xyz.fill(0);
   m_pre.clear();
   m_build.clear();
}


cmp_result semver::compare_b_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // If either is empty, this is a quick comparison.
   if (lhs.empty())
      return (rhs.empty() ? cmp_result::eq : cmp_result::lt);
   if (rhs.empty())
      return cmp_result::gt;

   return compare_pb_rule12(lhs, rhs);
}


cmp_result semver::compare_p_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // If either is empty, this is a quick comparison.
   if (lhs.empty())
      return (rhs.empty() ? cmp_result::eq : cmp_result::gt);
   if (rhs.empty())
      return cmp_result::lt;

   return compare_pb_rule12(lhs, rhs);
}


cmp_result semver::compare_pb_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // Requirements here:
   //    https://semver.org/spec/v2.0.0-rc.1.html#spec-item-12
   // Correct requirements are actually here:
   //    https://github.com/semver/semver/blob/v2.0.0/semver.md

   // Split on '.'
   auto l = string::split(lhs, ".");
   auto r = string::split(rhs, ".");

   // Compare the splits.
   const size_t comp_count = std::min(l.size(), r.size());
   for (size_t i = 0; i < comp_count; ++i) {
      // Same? On to the next one...
      if (l[i] == r[i])
         continue;
      // Numbers have higher magnitude than letters, look to see if either value is ALL numbers.
      auto left = l[i].find_first_not_of("0123456789");
      auto right = r[i].find_first_not_of("0123456789");
      if (left != std::string_view::npos) {     // Left is NOT numbers only.
         if (right != std::string_view::npos) { // Also Right is NOT numbers only.
            // Simple string compare works here.
            if (l[i] < r[i])
               return cmp_result::lt;
            return cmp_result::gt;
         }
         // Left has letters, right is numbers. So right is of higer magnitude.
         return cmp_result::lt;
      }
      // Left is a number, if right is NOT a number, then left has greater magnitude.
      if (right != std::string_view::npos)
         return cmp_result::gt;
      // Both are numbers, convert and compare.
      int lnum = 0;
      string::from(l[i], lnum); // ignore return code. If it's a failure, we will just use the zero value.
      int rnum = 0;
      string::from(r[i], rnum);
      if (lnum == rnum)
         continue;
      if (lnum < rnum)
         return cmp_result::lt;
      return cmp_result::gt;
   }
   // So far, all the splits are equal. Is one of them longer then the other?
   if (l.size() == r.size())
      return cmp_result::eq;
   if (l.size() < r.size())
      return cmp_result::lt;
   return cmp_result::gt;
}


cmp_result semver::compare(const self& rhs) const noexcept {
   // Compare x.y.z
   for (size_t i = 0; i < m_xyz.size(); ++i) {
      if (m_xyz[i] == rhs.m_xyz[i])
         continue;
      if (m_xyz[i] < rhs.m_xyz[i])
         return cmp_result::lt;
      return cmp_result::gt;
   }
   // x.y.z are the same, so compare pre-release and build.
   auto result = compare_p_rule12(m_pre, rhs.m_pre);
   if (result != cmp_result::eq)
      return result;

   return compare_b_rule12(m_build, rhs.m_build);
}


std::optional<semver> semver::parse(std::string_view s) noexcept {

   semver rv;

   // Get build first, if any.
   auto pos = s.find_first_of('+');
   if (pos != std::string_view::npos) {
      // Copy the build substring, test it's valid, and
      rv.m_build = s.substr(pos + 1);
      if (rv.m_build.empty() || !validate_pb_rule10or11(rv.m_build))
         return std::optional<semver>();
      s = s.substr(0, pos);
   }
   // Now get the pre-release, if any.
   // Note we allow the deviation of having rc values NOT require the leading dash ('-').
   pos = s.find_first_of('-');
   if (pos != std::string_view::npos) { // found '-'
      // Copy the build substring, test it's valid, and
      rv.m_pre = s.substr(pos + 1);
      if (rv.m_pre.empty() || !validate_pb_rule10or11(rv.m_pre))
         return std::optional<semver>();
      s = s.substr(0, pos);
   }
   else {
      pos = s.find("rc");
      if (pos != std::string_view::npos) { // found "rc"
         rv.m_pre = s.substr(pos);
         if (rv.m_pre.empty() || !validate_pb_rule10or11(rv.m_pre))
            return std::optional<semver>();
         s = s.substr(0, pos);
      }
   }


   // Split x.y.z apart, validate it as well.
   auto splits = string::split(s, ".");
   if (splits.empty() || (splits.size() > 3))
      return std::optional<semver>();
   for (size_t i = 0; i < splits.size(); ++i) {
      // From returns false if any value isn't in [0-9].
      if (!string::from(splits[i], rv.m_xyz[i]))
         return std::optional<semver>();
   }

   return rv;
}


void semver::print(std::ostream& os) const noexcept {

   // x.y.z
   os << m_xyz[0] << '.' << m_xyz[1] << '.' << m_xyz[2];
   // pre-release?
   if (!m_pre.empty())
      os << '-' << m_pre;
   // build?
   if (!m_build.empty())
      os << '+' << m_build;
}


std::string semver::string() const noexcept {
   std::ostringstream ss;
   print(ss);
   return ss.str();
}


bool semver::validate_pb_rule10or11(std::string_view s) noexcept {
   for(auto c : s) {
      if( (c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c == '-') ) {
         // This char was valid, on to the next one.
         continue;
      }
      return false;
   }
   // All chars were valid!!!
   return true;
}


} // namespace antler::project
