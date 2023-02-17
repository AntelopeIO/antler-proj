/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version.hpp>
#include <antler/string/from.hpp>

#include <boost/algorithm/string.hpp> // boost::split()

#include <sstream>



namespace antler::project {


//--- constructors/destruct ------------------------------------------------------------------------------------------

version::version() = default;


version::version(std::string_view ver) {
   load(ver);
}


version::version(const semver& sv) {
   load(sv);
}


version::version(const self& rhs)
   : m_raw{ rhs.m_raw }
{
   if (rhs.m_semver)
      m_semver = std::make_unique<semver>(*rhs.m_semver);
}


//--- operators ---------------------------------------------------------------------------------------------------------

version& version::operator=(std::string_view ver) {
   load(ver);
   return *this;
}


version& version::operator=(const semver& sv) {
   load(sv);
   return *this;
}


version& version::operator=(const self& rhs) {
   m_raw = rhs.m_raw;
   if (rhs.m_semver)
      m_semver = std::make_unique<semver>(*rhs.m_semver);
   else
      m_semver.reset();
   return *this;
}


std::strong_ordering version::operator<=>(const self& rhs) const noexcept {
   if (is_semver() && rhs.is_semver())
      return *m_semver <=> *rhs.m_semver;
   return raw_compare(m_raw, rhs.m_raw);
}


bool version::operator==(const self& rhs) const noexcept {
   return operator<=>(rhs) == 0;
}


bool version::operator!=(const self& rhs) const noexcept {
   return operator<=>(rhs) != 0;
}


version::operator semver() const noexcept {
   if (m_semver)
      return *m_semver;
   return semver{};
}


//--- alphabetic --------------------------------------------------------------------------------------------------------


void version::clear() noexcept {
   m_raw.clear();
   m_semver.reset();
}


bool version::empty() const noexcept {
   return m_raw.empty();
}


bool version::is_semver() const noexcept {
   if (m_semver)
      return true;
   return false;
}


void version::load(std::string_view s) {
   m_raw = s;
   auto temp = semver::parse(m_raw);
   if (!temp)
      m_semver.reset();
   else
      m_semver = std::make_unique<semver>(temp.value());
}


void version::load(const semver& sv) {
   std::stringstream ss;
   ss << sv;
   m_raw = ss.str();
   m_semver = std::make_unique<semver>(sv);
}


std::string_view version::raw() const noexcept {
   return m_raw;
}


std::strong_ordering version::raw_compare(std::string_view l_in, std::string_view r_in) noexcept {
   if (l_in == r_in)
      return std::strong_ordering::equal;

   std::vector<std::string_view> l;
   boost::split(l, l_in, boost::is_any_of(".,-+;"));

   std::vector<std::string_view> r;
   boost::split(r, r_in, boost::is_any_of(".,-+;"));

   for (size_t i = 0; i < std::min(l.size(), r.size()); ++i) {
      if (l[i] == r[i])
         continue;

      // Can we convert the whole thing to a number?
      auto ln = l[i].find_first_not_of("0123456789");
      auto rn = r[i].find_first_not_of("0123456789");
      if (ln != std::string_view::npos || rn != std::string_view::npos) {
         // Nope, one or both of the strings contain non numeric chars.

         // Get the number portion into either lnum or rnum int and the trailing non numeric chars into the string lremain or
         // rremain.
         int lnum = 0;
         std::string_view lremain;
         if (ln == std::string_view::npos) {
            [[maybe_unused]] auto discard = string::from(l[i], lnum);
            lremain = l[i];
         }
         else {
            [[maybe_unused]] auto discard = string::from(l[i].substr(0, ln), lnum);
            lremain = l[i].substr(ln);
         }

         int rnum = 0;
         std::string_view rremain;
         if (rn == std::string_view::npos) {
            [[maybe_unused]] auto discard = string::from(r[i], rnum);
            rremain = r[i];
         }
         else {
            [[maybe_unused]] auto discard = string::from(r[i].substr(0, rn), rnum);
            rremain = r[i].substr(rn);
         }

         // If the numbers differ, return the difference between them.
         if (auto cmp = lnum <=> rnum; cmp != 0)
            return cmp;

         // Otherwise, return the difference in the remaining values.
         return lremain <=> rremain;
      }

      // Convert into ints and compare (or do a simple string compare).
      if (int lnum = 0, rnum = 0; string::from(l[i], lnum) && string::from(r[i], rnum)) {
         return lnum <=> rnum;
      }

      // Nope, STILL can't convert to JUST a number. Just do a raw string compare.
      return l[i] <=> r[i];
   }

   // Thus far, all the splits are equal, so just compare the number of splits.
   // Example: `a.b.c <=> a.b.c.d` finds the first 3 splits equal, so we just compare the split count: `3 <=> 4`.
   return l.size() <=> r.size();
}



} // namespace antler::project
