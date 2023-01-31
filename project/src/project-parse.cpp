/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.h>
#include <key.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <ryml.hpp>
#include <c4/std/string.hpp>    // to_substr(std::string)

#pragma GCC diagnostic pop


#include <iostream>
#include <fstream>
#include <streambuf>



namespace antler {
namespace project {

namespace {

/// Load a text file into a string.
///
/// @noteThis should be optimized and stored somewhere. Something like this is a useful library function - when optimized...
///
/// @param path  Path to the file.
/// @param os  ostream to write errors to.
/// @return An optional string that is populated with the file contents *if* the load was successful; otherwise, it's invalid for any error.
std::optional<std::string> load(const std::filesystem::path& path, std::ostream& os) {

   using return_type = std::optional<std::string>;

   std::error_code sec;

   // Sanity check and determine the file size.
   if (!std::filesystem::exists(path, sec)) {
      os << "Path doesn't exist: " << path << "\n";
      return return_type();
   }
   if (!std::filesystem::is_regular_file(path, sec)) {
      os << "Path must be regular file: " << path << "\n";
      return return_type();
   }
   std::uintmax_t sz = std::filesystem::file_size(path, sec);
   if (sec || sz == static_cast<std::uintmax_t>(-1)) {
      os << "Can't determine file size for: " << path << " with error " << sec << "\n";
      return return_type();
   }

   // Create a string of the appropriate size for loading.
   std::string rv;
   rv.reserve(sz);

   // And load it...
   std::ifstream infile(path);
   rv.append(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());

   return rv;
}


} // anonymous namespace


/// Parse the dependency portion of an antler-pack project file. Error are written to os.
/// @param node  Reference to the node to be parsed.
/// @param os  Stream for prinitng errors.
/// @return optional of dependency type. Dependency is populated on successful parse only.
std::optional<dependency> parse_depends(const ryml::NodeRef& node, std::ostream& os) {

   using return_type = std::optional<dependency>;

   dependency rv;

   // We assume node is a map.

   // For each element in this node.
   for (auto i : node) {
      // Sanity check.
      if (!i.has_key()) {
         os << "no key\n";
         continue;
      }

      // Get the key as one of our enums for a switch.
      key::word word = key::to_word(i.key());
      switch (word) {

         case key::word::name: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in dependency list with no value.\n";
               return return_type();
            }
            if (!rv.name().empty()) {
               os << "Duplicate " << word << " values in dependency list: " << i.val() << ", " << rv.name() << "\n";
               return return_type();
            }
            rv.name(i.val());
         } break;

         case key::word::tag: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in dependency list with no value.\n";
               return return_type();
            }
            if (!rv.tag().empty()) {
               os << "Duplicate " << word << " values in dependency list: " << i.val() << ", " << rv.tag() << "\n";
               return return_type();
            }
            rv.tag(i.val());
         } break;

         case key::word::release:
         case key::word::version: { // Allow version to mean release.
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in dependency list with no value.\n";
               return return_type();
            }
            if (!rv.release().empty()) {
               os << "Duplicate " << word << " values in dependency list: " << i.val() << ", " << rv.release() << "\n";
               return return_type();
            }
            rv.release(i.val());
         } break;

         case key::word::hash: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in dependency list with no value.\n";
               return return_type();
            }
            if (!rv.hash().empty()) {
               os << "Duplicate " << word << " values in dependency list: " << i.val() << ", " << rv.hash() << "\n";
               return return_type();
            }
            rv.hash(i.val());
         } break;

         case key::word::from: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in dependency list with no value.\n";
               return return_type();
            }
            if (!rv.location().empty()) {
               os << "Duplicate " << word << " values in dependency list: " << i.val() << ", " << rv.location() << "\n";
               return return_type();
            }
            if (!dependency::validate_location(i.val())) {
               os << "Invalid location: " << i.val() << "\n";
               return return_type();
            }
            rv.location(i.val());
         } break;

         case key::word::patch: {
            // Get the patch file paths.
            for (auto fn : i) {
               // Sanity check.
               if (!fn.has_val()) {
                  os << "no val\n";
                  continue;
               }
               std::string_view temp = fn.val();
               rv.patch_add(temp);
            }
         } break;


         case key::word::project:
         case key::word::libs:
         case key::word::apps:
         case key::word::tests:
         case key::word::lang:
         case key::word::options:
         case key::word::depends:
         case key::word::command: {
            os << "Unexpected tag in dependency list: " << word << "\n";
            return return_type();
         }

         case key::word::none: {
            os << "Unknown tag in dependency list: " << i.key() << "\n";
            return return_type();
         }
      }
   }

   return rv;
}


/// Parse the object portion of an antler-pack project file. Error are written to os.
/// @param node  Reference to the node to be parsed.
/// @param os  Stream for prinitng errors.
/// @return optional of object type. Dependency is populated on successful parse only.
std::optional<object> parse_object(const ryml::NodeRef& node, object::type_t type, std::ostream& os) {

   using return_type = std::optional<object>;

   object rv(type);

   for (auto i : node) {
      if (!i.has_key()) {
         os << "no key\n";
         continue;
      }

      // Get the key as one of our enums for a switch.
      key::word word = key::to_word(i.key());
      switch (word) {

         case key::word::name: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << "Name tag in " << type << " list with no value.\n";
               return return_type();
            }
            if (!rv.name().empty()) {
               os << "Duplicate name values in " << type << " list: " << i.val() << "\n";
               return return_type();
            }
            rv.name(i.val());
         } break;

         case key::word::lang: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in " << type << " list with no value.\n";
               return return_type();
            }
            auto lang = to_language(i.val());
            if (lang == language::none) {
               os << "Invalid language tag in " << type << " list: " << i.val() << "\n";
               return return_type();
            }
            if (rv.language() != language::none) {
               os << "Duplicate language values in " << type << " list: " << rv.language() << ", " << lang << "\n";
               return return_type();
            }
            rv.language(lang);
         } break;

         case key::word::options: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in " << type << " list with no value.\n";
               return return_type();
            }
            if (!rv.options().empty()) {
               os << "Duplicate " << word << " values in " << type << " list: " << rv.options() << ", " << i.val() << "\n";
               return return_type();
            }
            rv.options(i.val());
         } break;


         case key::word::command: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << word << " tag in " << type << " list with no value.\n";
               return return_type();
            }
            if (!rv.command().empty()) {
               os << "Duplicate " << word << " values in " << type << " list: " << rv.command() << ", " << i.val() << "\n";
               return return_type();
            }
            rv.command(i.val());
         } break;


         case key::word::depends: {
            // sanity check
            if (i.has_val() && !i.val().empty()) {
               os << "Unexpected value in " << word << " list: " << i.val() << "\n";
               return return_type();
            }
            // Depends should be a map. For each element, parse out the dependency and store it.
            for (auto j : i) {
               auto optional_dep = parse_depends(j, os);
               if (!optional_dep)
                  return return_type();
               if (rv.dependency_exists(optional_dep.value().name())) {
                  os << "Multiple dependencies with the same name in " << word << " list: " << optional_dep.value().name() << "\n";
                  return return_type();
               }
               rv.upsert_dependency(std::move(optional_dep.value()));
            }
         } break;


         case key::word::apps:
         case key::word::from:
         case key::word::hash:
         case key::word::libs:
         case key::word::patch:
         case key::word::project:
         case key::word::release:
         case key::word::tag:
         case key::word::tests:
         case key::word::version: {
            os << "Unexpected tag in " << type << " list: " << word << "\n";
            return return_type();
         }

         case key::word::none: {
            os << "Unknown tag in " << type << " list: " << i.key() << "\n";
            return return_type();
         }
      }
   }

   return rv;
}


std::optional<project> project::parse(const std::filesystem::path& path, std::ostream& os) {

   using return_type = std::optional<project>;

   // Get file contents and store it in source.
   std::string source;
   {
      auto temp = load(path, os);
      if (!temp)
         return return_type();
      source = temp.value();
   }

   // Parse source IN PLACE. So do NOT modify source after parsing!
   ryml::Tree tree = ryml::parse_in_place(c4::to_substr(source));

   // Create the project object and set it's source path.
   project rv;
   rv.path(path);

   // For each member of the tree...
   for (const auto& i : tree.rootref()) {
      // Sanity check.
      if (!i.has_key()) {
         os << "Missing key in root.\n";
         return return_type{};
      }

      // Get the key as one of our enums for a switch.
      key::word word = key::to_word(i.key());
      switch (word) {

         case key::word::project: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << "Project tag at root level with no value.\n";
               return return_type();
            }
            if (!rv.name().empty()) {
               os << "Multiple project tags at root level: " << rv.name() << ", " << i.val() << "\n";
               return return_type();
            }
            rv.name(i.val());
         } break;

         case key::word::version: {
            // Sanity check before setting value.
            if (!i.has_val()) {
               os << "Version tag at root level with no value.\n";
               return return_type();
            }
            if (!rv.version().empty()) {
               os << "Multiple version tags at root level: " << rv.version() << ", " << i.val() << "\n";
               return return_type();
            }
            rv.version(antler::project::version(i.val()));
         } break;

         case key::word::apps:
         case key::word::libs:
         case key::word::tests: {
            // sanity check
            if (i.has_val() && !i.val().empty()) {
               os << "Unexpected value in " << word << " list: " << i.val() << "\n";
               return return_type();
            }

            // Apps, libs, and tests are nearly the same. But they have a few small differences in parsing and also end up in
            // different lists. So here we set the object type and a reference to the storage list.
            //
            // This allows us to write and maintain a single block of code to parse each list.

            // The list type.
            const object::type_t ot =
               (word == key::word::apps ? object::app :
                     (word == key::word::libs ? object::lib : object::test) );
            // A reference to the list we want to populate.
            object::list_t& list =
               (ot == object::app ? rv.m_apps :
                     (ot == object::lib ? rv.m_libs : rv.m_tests) );

            // For each object in the list, call parse object.
            for (auto node : i) {
               auto optional_obj = parse_object(node, ot, os);
               // sanity check before storing.
               if (!optional_obj)
                  return return_type();
               if (rv.object_exists(optional_obj.value().name(), ot)) {
                  os << "Multiple object with the same name in " << word << " list: " << optional_obj.value().name() << "\n";
                  return return_type();
               }
               list.emplace_back(optional_obj.value());
            }
         } break;


         case key::word::command:
         case key::word::depends:
         case key::word::from:
         case key::word::hash:
         case key::word::lang:
         case key::word::name:
         case key::word::options:
         case key::word::patch:
         case key::word::release:
         case key::word::tag: {
            os << "Unexpected tag at root level: " << word << "\n";
            return return_type();
         }

         case key::word::none: {
            os << "Unknown tag at root level: " << i.key() << "\n";
            return return_type();
         }
      }
   }


   // Validate here.
   if (!rv.is_valid(os))
      return return_type();

   return rv;
}


} // namespace project
} // namespace antler
