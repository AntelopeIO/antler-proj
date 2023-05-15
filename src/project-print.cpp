/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/system/version.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <ryml.hpp>
#ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
#include <c4/std/string.hpp> // to_csubstr(std::string)
#endif

#pragma GCC diagnostic pop

#include <sstream>

#include "token.hpp"

namespace antler::project {

inline static c4::csubstr to_csubstr(std::string_view sv) noexcept {
   return {sv.data(), sv.size()};
}
inline static c4::csubstr to_csubstr(token tok) noexcept {
   return to_csubstr(system::to_string(tok));
}
inline static c4::csubstr to_csubstr(const version& v) noexcept {
   return to_csubstr(v.to_string());
}

void project::print(std::ostream& os) const noexcept {

   // Warning: nodes do not like non-literal values!!!
   //  Be very, very careful here or you'll end up with garbage and segfaults.


   ryml::Tree tree;

   // We want a root node that's a map.
   auto root = tree.rootref();
   root.change_type(c4::yml::MAP);

   // Store the project name and version
   if (!m_name.empty())
      root[to_csubstr(token::project)] << to_csubstr(m_name);

   if (!m_ver.empty())
      root[to_csubstr(token::version)] << to_csubstr(m_ver);

   // Maintain once.
   //
   // We are going to use the same code to write out the library, apps, and test sequences. To do this, we will create a list
   // containing a reference/pointer to the project's lists and a list of the corresponding type. We iterate through each one.

   const std::vector<const object::list_t*> obj_lists{&m_libs, &m_apps, &m_tests};
   const std::vector<token>                 list_type{token::libraries, token::apps, token::tests};

   for (size_t i = 0; i < obj_lists.size(); ++i) {
      const auto& obj_list = *obj_lists[i]; // convenience.
      if (obj_list.empty())                 // Don't add the list if it's empty.
         continue;

      // Each of these lists is a yaml sequence, so create a sequence node with the correct key for the list type.
      auto obj_node = (root[to_csubstr(list_type[i])] << "");
      obj_node.change_type(c4::yml::SEQ);

      // Iterate through the list of objects...
      for (size_t j = 0; j < obj_list.size(); ++j) {
         const auto& obj = obj_list[j]; // convenience

         // Create a map node to contain the key/value pairs.
         auto map_node = obj_node[j] << "";
         map_node.change_type(c4::yml::MAP);

         // Add the elements.

         // name
         if (!obj.name().empty())
            map_node[to_csubstr(token::name)] << to_csubstr(obj.name());
         // lang
         if (!obj.language().empty())
            map_node[to_csubstr(token::lang)] << to_csubstr(obj.language());
         // options

         const auto& conjoin_opts = [&](const auto& opts) {
            std::string s;
            for (const auto& o : opts)
               s += std::string(o) + ";";
            return s;
         };

         if (!obj.compile_options().empty())
            map_node[to_csubstr(token::compile_options)] << to_csubstr(conjoin_opts(obj.compile_options()));
         if (!obj.link_options().empty())
            map_node[to_csubstr(token::link_options)] << to_csubstr(conjoin_opts(obj.link_options()));

         // depends - dependencies are also a list.
         if (!obj.dependencies().empty()) {
            // Make the sequence node with the correct key.
            auto dep_node = (map_node[to_csubstr(token::depends)] << "");
            dep_node.change_type(c4::yml::SEQ);

            // Iterate over every element in the dependency list...
            for (size_t k = 0; k < obj.dependencies().size(); ++k) {
               const auto& dep = obj.dependencies()[k]; // convenience

               // Create a map node to contain the key/value pairs.
               auto dep_map_node = dep_node[k] << "";
               dep_map_node.change_type(c4::yml::MAP);

               // Add the elements.
               // name
               if (!dep.name().empty())
                  dep_map_node[to_csubstr(token::name)] << to_csubstr(dep.name());
               // location
               if (!dep.location().empty())
                  dep_map_node[to_csubstr(token::from)] << to_csubstr(dep.location());
               // tag or commit hash
               if (!dep.tag().empty())
                  dep_map_node[to_csubstr(token::tag)] << to_csubstr(dep.tag());
               // release
               if (!dep.release().empty())
                  dep_map_node[to_csubstr(token::release)] << to_csubstr(dep.release());
               // hash
               if (!dep.hash().empty())
                  dep_map_node[to_csubstr(token::hash)] << to_csubstr(dep.hash());

               // Patch files.
               const auto& patch_files = dep.patch_files();
               if (!patch_files.empty()) {
                  auto patch_node = dep_map_node[to_csubstr(token::patch)] << "";
                  patch_node.change_type(c4::yml::SEQ);
                  for (size_t n = 0; n < patch_files.size(); ++n)
                     patch_node[n] << patch_files[n];
               }
            }
         }
         // command
         if (!obj.command().empty()) {
            map_node[to_csubstr(token::command)] << to_csubstr(obj.command());
         }
      }
   }



   // Add a header.
   os << magic_comment << "\n";
   os << comment_preamble << "\n";
   os << "#   generated with v" << system::version::full() << std::endl;

   os << "#\n"
      << "# This file was auto-generated. Be aware antler-proj may discard added comments.\n"
      << "\n\n";
   os << tree;
}

} // namespace antler::project