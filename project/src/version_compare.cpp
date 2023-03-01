/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_compare.hpp>
#include <antler/string/from.hpp>

#include <boost/algorithm/string.hpp> // boost::split()


namespace antler::project {

int64_t raw_compare(std::string_view lhs, std::string_view rhs) noexcept {

   if (lhs == rhs)
      return 0; 

   std::vector<std::string> l;
   boost::split(l, lhs, boost::is_any_of(".,-;+"));

   std::vector<std::string> r;
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
               return -1;
            return 1;
         }

         auto temp = lremain.compare(rremain);
         if (temp < 0)
            return -1;
         return 1;
      }

      if (!string::from(l[i], lnum) || !string::from(r[i], rnum)) {
         // Nope, STILL can't convert to JUST a number. Just do a raw string compare.
         auto temp = l[i].compare(r[i]);
         if (temp < 0)
            return -1;
         return 1;
      }
      if (lnum < rnum)
         return -1;
      return 1;
   }

   if (l.size() < r.size())
      return -1; 
   if (l.size() > r.size())
      return 1;
   return 0;
}


} // namespace antler::project
