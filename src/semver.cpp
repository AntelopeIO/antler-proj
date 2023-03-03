/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>
#include <antler/string/from.hpp>

#include <limits>
#include <vector>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp> // boost::split()


namespace antler::project {

//--- constructors/destrructor ------------------------------------------------------------------------------------------

semver::semver(value_type x, value_type y, value_type z, std::string_view pre_release, std::string_view build) noexcept
   : m_xyz{ x, y, z }
   , m_pre{ pre_release }
   , m_build{ build }
{
}


//--- operators ---------------------------------------------------------------------------------------------------------

int64_t semver::compare(const semver& rhs) const noexcept {
   for (size_t i = 0; i < m_xyz.size(); ++i ) {
      if (m_xyz[i] < rhs.m_xyz[i])
         return -1;
      else if (m_xyz[i] > rhs.m_xyz[i])
         return 1;
   }

   if (auto cmp = compare_p_rule12(m_pre, rhs.m_pre); cmp !=0 )
      return cmp;
   
   return compare_b_rule12(m_build, rhs.m_build);
}

//--- alphabetic --------------------------------------------------------------------------------------------------------

void semver::clear() noexcept {
   m_xyz.fill(0);
   m_pre.clear();
   m_build.clear();
}


int64_t semver::compare_b_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // If either is empty, this is a quick comparison.
   if (lhs.empty())
      return (rhs.empty() ? 0 : -1);
   if (rhs.empty())
      return 1;

   return compare_pb_rule12(lhs, rhs);
}


int64_t semver::compare_p_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // If either is empty, this is a quick comparison.
   if (lhs.empty())
      return (rhs.empty() ? 0 : 1);
   if (rhs.empty())
      return -1;

   return compare_pb_rule12(lhs, rhs);
}


int64_t semver::compare_pb_rule12(std::string_view lhs, std::string_view rhs) noexcept {

   // Requirements here:
   //    https://semver.org/spec/v2.0.0-rc.1.html#spec-item-12
   // Correct requirements are actually here:
   //    https://github.com/semver/semver/blob/v2.0.0/semver.md

   // Split on '.'
   // std::vector<std::string> is a limitation of boost,
   // TODO in next release of CDT/ANTLER change this back to std::vector<std::string_view> with a sufficient version of boost
   std::vector<std::string> l;
   boost::split(l, lhs, boost::is_any_of("."));

   // std::vector<std::string> is a limitation of boost,
   // TODO in next release of CDT/ANTLER change this back to std::vector<std::string_view> with a sufficient version of boost
   std::vector<std::string> r;
   boost::split(r, rhs, boost::is_any_of("."));

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
            return l[i] < r[i] ? -1 : l[i] > r[i] ? 1 : 0;
         }
         // Left has letters, right is numbers. So right is of higer magnitude.
         return -1;
      }
      // Left is a number, if right is NOT a number, then left has greater magnitude.
      if (right != std::string_view::npos)
         return 1;
      // Both are numbers, convert and compare.
      int lnum = 0;
      [[maybe_unused]] auto discard1 = string::from(l[i], lnum); // ignore return code. If it's a failure, we will just use the zero value.
      int rnum = 0;
      [[maybe_unused]] auto discard2 = string::from(r[i], rnum);
      if ( lnum < rnum )
         return -1;
      else if ( lnum > rnum )
         return 1;
   }
   // So far, all the splits are equal. Compare the split size.
   return l.size() < r.size() ? -1 : l.size() > r.size() ? 1 : 0;
}


std::optional<semver> semver::parse(std::string_view s) noexcept {

   semver rv;

   // Get build first, if any.
   auto pos = s.find_first_of('+');
   if (pos != std::string_view::npos) {
      // Copy the build substring, test it's valid, and
      rv.m_build = s.substr(pos + 1);
      if (rv.m_build.empty() || !validate_pb_rule10or11(rv.m_build))
         return {};
      s = s.substr(0, pos);
   }
   // Now get the pre-release, if any.
   // Note we allow the deviation of having rc values NOT require the leading dash ('-').
   pos = s.find_first_of('-');
   if (pos != std::string_view::npos) { // found '-'
      // Copy the build substring, test it's valid, and
      rv.m_pre = s.substr(pos + 1);
      if (rv.m_pre.empty() || !validate_pb_rule10or11(rv.m_pre))
         return {};
      s = s.substr(0, pos);
   }
   else {
      pos = s.find("rc");
      if (pos != std::string_view::npos) { // found "rc"
         rv.m_pre = s.substr(pos);
         if (rv.m_pre.empty() || !validate_pb_rule10or11(rv.m_pre))
            return {};
         s = s.substr(0, pos);
      }
   }


   // Split x.y.z apart, validate it as well.
   std::vector<std::string> splits;
   boost::split(splits, s, boost::is_any_of("."));
   if (splits.empty() || (splits.size() > 3))
      return {};
   for (size_t i = 0; i < splits.size(); ++i) {
      // From returns false if any value isn't in [0-9].
      if (!string::from(splits[i], rv.m_xyz[i]))
         return {};
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
