/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/semver.hpp>
#include <test_common.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>



struct semver_t {
   antler::project::semver::value_type x;
   antler::project::semver::value_type y;
   antler::project::semver::value_type z;
   std::string pre;
   std::string build;
};
std::ostream& operator<<(std::ostream& os, const semver_t& sv) {
   os << sv.x << "." << sv.y << "." << sv.z;
   if (!sv.pre.empty())
      os << "-" << sv.pre;
   if (!sv.build.empty())
      os << "+" << sv.build;
   return os;
}


struct semver_entry {
   std::string l; // left comparison
   semver_t r;    // right comparison
};

const std::vector<semver_entry> semver_list = {
   {"1.0.0",       { 1, 0, 0, "", "" }    },
   { "1.0",        { 1, 0, 0, "", "" }    },
   { "1",          { 1, 0, 0, "", "" }    },
   { "1.0-rc1",    { 1, 0, 0, "rc1", "" } },
   { "1.0rc1",     { 1, 0, 0, "rc1", "" } },
   { "1rc1",       { 1, 0, 0, "rc1", "" } },
   { "1.99.0-rc1", { 1, 99, 0, "rc1", "" }},
   { "1.1.1-rc",   { 1, 1, 1, "rc", "" }  },
};


void test_semver() {

   for (auto& a : semver_list) {


      auto temp = antler::project::semver::parse(a.l);

      std::stringstream ss;
      ss << a.l << " , " << a.r << "  semver::parse(l): ";
      if (temp)
         ss << temp.value();
      else
         ss << "empty";
      auto msg = ss.str();

      TEST(msg, temp);
      if (!temp)
         continue;

      auto l = temp.value();
      const auto& r = antler::project::semver(a.r.x, a.r.y, a.r.z, a.r.pre, a.r.build);
      ;

      TEST(msg, l == r);
   }
}


int main(int, char**) {

   test_semver();

   return result();
}
