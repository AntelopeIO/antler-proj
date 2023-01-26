#include <antler/project/version.h>
#include <antler/string/from.h>
#include <antler/string/split.h>

#include <limits>
#include <vector>
#include <iostream>
#include <sstream>



namespace antler {
namespace project {


//--- constructors/destruct ------------------------------------------------------------------------------------------

version::version() = default;


version::version(std::string_view ver)
{
   load(ver);
}


version::version(const semver& sv)
{
   load(sv);
}


version::version(const self& rhs)
   : m_raw{rhs.m_raw}
{
   if(rhs.m_semver)
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
   if(rhs.m_semver)
      m_semver = std::make_unique<semver>(*rhs.m_semver);
   else
      m_semver.reset();
   return *this;
}


bool version::operator==(const self& rhs) const noexcept {
   return compare(rhs) == cmp::eq;
}


bool version::operator!=(const self& rhs) const noexcept {
   return compare(rhs) != cmp::eq;
}


bool version::operator<(const self& rhs)  const noexcept {
   return compare(rhs) == cmp::lt;
}


bool version::operator<=(const self& rhs) const noexcept {
   return compare(rhs) != cmp::gt;
}


bool version::operator>(const self& rhs) const noexcept {
   return compare(rhs) == cmp::gt;
}


bool version::operator>=(const self& rhs) const noexcept {
   return compare(rhs) != cmp::lt;
}


version::operator semver() const noexcept {
   if(m_semver)
      return *m_semver;
   return semver{};
}




//--- alphabetic --------------------------------------------------------------------------------------------------------


void version::clear() noexcept {
   m_raw.clear();
   m_semver.reset();
}


version::cmp version::compare(const self& rhs) const noexcept {
   if(is_semver() && rhs.is_semver()) {
      if( *m_semver == *rhs.m_semver )
         return cmp::eq;
      if( *m_semver < *rhs.m_semver )
         return cmp::lt;
      return cmp::gt;
   }
   return raw_compare(m_raw, rhs.m_raw);
}


bool version::empty() const noexcept {
   return m_raw.empty();
}


bool version::is_semver() const noexcept {
   if( m_semver )
      return true;
   return false;
}


void version::load(std::string_view s) {
   m_raw = s;
   auto temp = semver::parse(m_raw);
   if(!temp)
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


version::cmp version::raw_compare(std::string_view l_in, std::string_view r_in) noexcept {
   if(l_in == r_in)
      return cmp::eq;

   auto l = string::split(l_in,".,-;");
   auto r = string::split(r_in,".,-;");

   for(size_t i = 0; i < std::min(l.size(), r.size()); ++i) {
      if(l[i] == r[i])
         continue;

      int lnum=0;
      int rnum=0;

      // Can we convert the whole thing to a number?
      auto ln = l[i].find_first_not_of("0123456789");
      auto rn = r[i].find_first_not_of("0123456789");
      if(ln != std::string_view::npos || rn != std::string_view::npos) {

         // Nope, so try to compare numbers and letters.

         std::string_view lremain;
         if(ln == std::string_view::npos) {
            string::from(l[i],lnum);
            lremain = l[i];
         }
         else {
            string::from(l[i].substr(0,ln),lnum);
            lremain = l[i].substr(ln);
         }

         std::string_view rremain;
         if(rn == std::string_view::npos) {
            string::from(r[i],rnum);
            rremain = r[i];
         }
         else {
            string::from(r[i].substr(0,rn),rnum);
            rremain = r[i].substr(rn);
         }

         if(lnum != rnum) {
            if(lnum < rnum)
               return cmp::lt;
            return cmp::gt;
         }

         auto temp = lremain.compare(rremain);
         if(temp < 0)
            return cmp::lt;
         return cmp::gt;
      }

      if( !string::from(l[i],lnum) || !string::from(r[i],rnum) ) {
         // Nope, STILL can't convert to JUST a number. Just do a raw string compare.
         auto temp = l[i].compare(r[i]);
         if(temp < 0)
            return cmp::lt;
         return cmp::gt;
      }
      if(lnum < rnum)
         return cmp::lt;
      return cmp::gt;
   }

   if(l.size() < r.size())
      return cmp::lt;
   if(l.size() > r.size())
      return cmp::gt;
   return cmp::eq;
}




} // namespace project
} // namespace antler
