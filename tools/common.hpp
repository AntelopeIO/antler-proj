#pragma once

#include "CLI11.hpp"

#include <antler/project/project.hpp>
#include <antler/project/cmake.hpp>

namespace antler {

   /// Remove escape sequence character from input.
   /// @note This is a SIMPLE algorithm that ONLY removes the first `\` in a 2 charachter sequence.
   /// @todo Consider implementing more complicated transforms (e.g. convert "\t" to 0x09 or "\xFF" to 0xFF, etc).
   /// @param input  Input string that may or may not contain escape characters.
   /// @return The transformed string.
   std::string escape_transform(std::string input) {
      for(auto i=input.begin(); i != input.end(); ++i) {
         // Only remove this char *if* there is a char following.
         // Note: erase plus increment effectively steps over the char follwing the erased `\` (e.g. "\\-O2" becomes "\-O2").
         if(*i == '\\' && i+1 != input.end())
            i = input.erase(i);
      }
      return input;
   }

   inline project::project load_project(const system::fs::path& path) {
      auto p = system::fs::canonical(system::fs::path(path));
      ANTLER_CHECK(project::project::update_path(p),
         "path either did not exist or neither `project.yaml` nor `project.yml` file cound be found.");
      project::project proj;
      ANTLER_CHECK(proj.from_yaml(project::yaml::load(p)),
         "error while loading project file");
      proj.path(p.parent_path());
      return proj;
   }

   bool should_repopulate(project::project& proj) {
      auto p = proj.path() / project::project::manifest_name;
      auto build = proj.path() / "build" / project::cmake_lists::filename;

      auto last_manifest_time = system::fs::last_write_time(p);

      if (!system::fs::exists(build)) {
         return true;
      }

      auto last_pop_time      = system::fs::last_write_time(build);

      return last_pop_time < last_manifest_time;
   }
}
