/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>
#include <antler/system/version.hpp>
#include <key.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <ryml.hpp>
#ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
#  include <c4/std/string.hpp> // to_csubstr(std::string)
#endif

#pragma GCC diagnostic pop

#include <sstream>
#include <magic_enum.hpp>

namespace { // anonymous

/// Templates to work around ryml::csubstr data type. Note that csubstr is very similar to std::string_view.

template<typename T>
[[nodiscard]] inline const c4::csubstr to_csubstr(T t) {
   // The default function should probably not exist. It will ONLY work in liited situations. In other cases, it silently allows
   // ryml to write garbage.

   t.force_this_template_function_to_generate_an_error(); // "disable" calls to this function.
   std::stringstream ss;
   ss << t;
   return c4::to_csubstr(ss.str());
}

template<>
[[nodiscard]] inline const c4::csubstr to_csubstr(std::string s) {
   return c4::to_csubstr(s);
}

template<>
[[nodiscard]] inline const c4::csubstr to_csubstr(std::string_view s) {
   return c4::to_csubstr(s);
}

template<>
[[nodiscard]] inline const c4::csubstr to_csubstr(key::word e) {
   return to_csubstr(key::literals[static_cast<size_t>(e)]);
}

template<>
[[nodiscard]] inline const c4::csubstr to_csubstr(antler::project::language e) {
   return to_csubstr(antler::project::language_literals[static_cast<size_t>(e)]);
}

template<typename T>
[[nodiscard]] inline const c4::csubstr to_csubstr_insert(T t) {
   std::stringstream ss;
   ss << t;
   return c4::to_csubstr(ss.str());
}

template<>
[[nodiscard]] inline const c4::csubstr to_csubstr_insert(antler::project::version v) {
   // This is a hack for now.
   return c4::to_csubstr(v.raw());
}


} // anonymous namespace



namespace antler::project {

void project::print(std::ostream& os) const noexcept {

   // Warning: nodes do not like non-literal values!!!
   //  Be very, very careful here or you'll end up with garbage and segfaults.


   ryml::Tree tree;

   // We want a root node that's a map.
   auto root = tree.rootref();
   root.change_type(c4::yml::MAP);

   // Store the project name and version
   if (!m_name.empty())
      root[to_csubstr(key::word::project)] << to_csubstr(m_name);

   if (!m_ver.empty())
      root[to_csubstr(key::word::version)] << to_csubstr_insert(m_ver);

   // Maintain once.
   //
   // We are going to use the same code to write out the library, apps, and test sequences. To do this, we will create a list
   // containing a reference/pointer to the project's lists and a list of the corresponding type. We iterate through each one.

   const std::vector<const object::list_t*> obj_lists{ &m_libs, &m_apps, &m_tests };
   const std::vector<key::word>             list_type{ key::word::libraries, key::word::apps, key::word::tests };

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
            map_node[to_csubstr(key::word::name)] << to_csubstr(obj.name());
         // lang
         if (obj.language() != language::none)
            map_node[to_csubstr(key::word::lang)] << to_csubstr(obj.language());
         // options
         if (!obj.options().empty())
            map_node[to_csubstr(key::word::options)] << to_csubstr(obj.options());

         // depends - dependencies are also a list.
         if (!obj.dependencies().empty()) {
            // Make the sequence node with the correct key.
            auto dep_node = (map_node[to_csubstr(key::word::depends)] << "");
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
                  dep_map_node[to_csubstr(key::word::name)] << to_csubstr(dep.name());
               // location
               if (!dep.location().empty())
                  dep_map_node[to_csubstr(key::word::from)] << to_csubstr(dep.location());
               // tag or commit hash
               if (!dep.tag().empty())
                  dep_map_node[to_csubstr(key::word::tag)] << to_csubstr(dep.tag());
               // release
               if (!dep.release().empty())
                  dep_map_node[to_csubstr(key::word::release)] << to_csubstr(dep.release());
               // hash
               if (!dep.hash().empty())
                  dep_map_node[to_csubstr(key::word::hash)] << to_csubstr(dep.hash());

               // Patch files.
               const auto& patch_files = dep.patch_files();
               if (!patch_files.empty()) {
                  auto patch_node = dep_map_node[to_csubstr(key::word::patch)] << "";
                  patch_node.change_type(c4::yml::SEQ);
                  for (size_t n = 0; n < patch_files.size(); ++n)
                     patch_node[n] << patch_files[n];
               }
            }
         }
         // command
         if (!obj.command().empty()) {
            map_node[to_csubstr(key::word::command)] << to_csubstr(obj.command());
         }
      }
   }



   // Add a header.
   os << "# `" << (!m_name.empty() ? m_name : "UNNAMED") << "` project file.\n"
      << "#   generated by antler-proj v"
      << system::version::full() << std::endl;

   // If we know the source file name, then add it to the header.
   if (!m_path.empty())
      os << "#   source: " << m_path.string() << std::endl;


   os << "#\n"
      << "# This file was auto-generated. Be aware antler-proj may discard added comments.\n"
      << "\n\n";
   os << tree;
}


void project::print(std::ostream& os, pop e) noexcept {
   os << magic_enum::enum_name(e);
}


} // namespace antler::project
