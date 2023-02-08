/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_compare.hpp>
#include <antler/string/from.hpp>

#include <boost/algorithm/string.hpp> // boost::split()


namespace antler::project {

void print(std::ostream& os, cmp_result e) noexcept {
   switch (e) {
#define CASE_OF(X) case cmp_result::X: os << #X; return;
      CASE_OF(eq);
      CASE_OF(lt);
      CASE_OF(gt);
#undef CASE_OF
   }
   os << "unknown cmp_result (" << unsigned(e) << ")";
}


cmp_result raw_compare(std::string_view lhs, std::string_view rhs) noexcept {

   if (lhs == rhs)
      return cmp_result::eq;

   std::vector<std::string_view> l;
   boost::split(l, lhs, boost::is_any_of(".,-;+"));

   std::vector<std::string_view> r;
   boost::split(r, rhs, boost::is_any_of(".,-;+"));

   for (size_t i = 0; i < std::min(l.size(), r.size()); ++i) {
      if (l[i] == r[i])
         continue;

      int lnum = 0;
      int rnum = 0;

      // Can we convert the whole thing to a number?
      auto ln = l[i].find_first_not_of("0123456789");
      auto rn = r[i].find_first_not_of("0123456789");
      if (ln != std::string_view::npos || rn != std::string_view::npos) {

         // Nope, so try to compare numbers and letters.

         std::string_view lremain;
         if (ln == std::string_view::npos) {
            [[maybe_unused]] auto discard = string::from(l[i], lnum);
            lremain = l[i];
         }
         else {
            [[maybe_unused]] auto discard = string::from(l[i].substr(0, ln), lnum);
            lremain = l[i].substr(ln);
         }

         std::string_view rremain;
         if (rn == std::string_view::npos) {
            [[maybe_unused]] auto discard = string::from(r[i], rnum);
            rremain = r[i];
         }
         else {
            [[maybe_unused]] auto discard = string::from(r[i].substr(0, rn), rnum);
            rremain = r[i].substr(rn);
         }

         if (lnum != rnum) {
            if (lnum < rnum)
               return cmp_result::lt;
            return cmp_result::gt;
         }

         auto temp = lremain.compare(rremain);
         if (temp < 0)
            return cmp_result::lt;
         return cmp_result::gt;
      }

      if (!string::from(l[i], lnum) || !string::from(r[i], rnum)) {
         // Nope, STILL can't convert to JUST a number. Just do a raw string compare.
         auto temp = l[i].compare(r[i]);
         if (temp < 0)
            return cmp_result::lt;
         return cmp_result::gt;
      }
      if (lnum < rnum)
         return cmp_result::lt;
      return cmp_result::gt;
   }

   if (l.size() < r.size())
      return cmp_result::lt;
   if (l.size() > r.size())
      return cmp_result::gt;
   return cmp_result::eq;
}


} // namespace antler::project
