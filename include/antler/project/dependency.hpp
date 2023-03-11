#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <utility> // std::pair
#include <filesystem>

#include "version.hpp"
#include "../system/utils.hpp"


namespace antler::project {

/// This class models and containts the dependency portion of an ANTLER project.
class dependency {
public:
   using list_t = std::vector<dependency>; ///< Alias for the list type.

   using patch_list_t = std::vector<std::filesystem::path>; ///< Alias for the patch file list type.

public:
   // use default constructors, copy and move constructors and assignments
   dependency() = default;
   inline dependency(std::string_view name, std::string_view loc, std::string_view tag="", 
                     std::string_view rel="", std::string_view hash="") {
      set(name, loc, tag, rel, hash);
   }

   dependency(const dependency&) = default;
   dependency(dependency&&) = default;
   
   dependency& operator=(const dependency&) = default;
   dependency& operator=(dependency&&) = default;


   /// Sets the internal values (regardless of the validity).
   /// @note Check that validate_location() returns true before setting these values.
   /// @note It's preferable to populate `tag` with the github commit value than use `rel`. Note that `rel` + `hash` works as well...
   /// @param name  The name of the dependency.
   /// @param loc  The "location" of the project.  Possibly a github repo, release or tag archive, or a local file.
   /// @param tag  This is either the github repo tag or commit hash, commit hash being preferable. This may be empty if rel is populated or loc points to an archive.
   /// @param rel  This is a github repo version. Using a commit hash tag is preferable. This may be empty.
   /// @param hash  If loc points to an archive, this should be populated with the sha256 hash.
   void set(std::string_view name, std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash);

   /// Get the dependency name.
   /// @return The name of this dependency.
   [[nodiscard]] inline const std::string& name() const noexcept { return m_name; }
   /// Set the dependency name.
   /// @param s  The new name for this dependency.
   inline void name(std::string s) noexcept { m_name = std::move(s); }

   /// Get the location field of this dependency.
   /// @return the from location of this dependency.
   [[nodiscard]] inline std::string_view location() const noexcept { return m_loc; }
   /// Set the location field of this dependency.
   /// @param s  The new from location of this dependency.
   void location(std::string s) noexcept { m_loc = std::move(s); }

   /// Report on the status of this dependencies from field: does it look like an archive?
   /// @return true if location ends in an archive format (e.g. ".tar.gz", ".tgz", etc")
   [[nodiscard]] bool is_archive() const noexcept;

   /// Report on the status of version information.
   /// @return true if both tag() and rel() would return empty.
   [[nodiscard]] bool empty_version() const noexcept;

   /// Get the github tag/commit hash.
   /// @return github tag/commit hash.
   [[nodiscard]] std::string_view tag() const noexcept;
   /// Set github tag/commit hash.
   /// @param s  The new github tag/commit hash.
   void tag(std::string_view s) noexcept;

   /// Get the github release version.
   /// @return The possibly empty release version.
   [[nodiscard]] std::string_view release() const noexcept;
   /// Set the new github release version.
   /// @param s  The new, possibly empty, release version.
   void release(std::string_view s) noexcept;

   /// Get the archive or release hash.
   /// @note For commit hash, see tag()
   /// @note only valid for archive and release.
   /// @return The archive or release hash.
   [[nodiscard]] std::string_view hash() const noexcept;
   /// Set the archive or release hash.
   /// @note For commit hash, see tag()
   /// @note only valid for archive and release.
   /// @param s  The new archive or release hash.
   void hash(std::string_view s) noexcept;

   /// Return the list of patch files.
   /// @note These should be in a location relative to the `project.yaml` file.
   /// @return  The list of patch files.
   [[nodiscard]] const patch_list_t& patch_files() const noexcept;
   /// Add a new file to the patch list.
   /// @note this should be in a location relative to the `project.yaml` file.
   /// @param path  The path to the patch file to add.
   void patch_add(const std::filesystem::path& path) noexcept;
   /// @note this should be in a location relative to the `project.yaml` file.
   /// @param path  The path to the patch file to remove.
   void patch_remove(const std::filesystem::path& path) noexcept;

   /// Test to see if the dependency is valid.
   /// @return true if dependency is an archive, github repo, or local and is reachable
   [[nodiscard]] bool is_valid_location() const noexcept;


   /// Test to see if a location is valid.
   /// @return true if s is an archive, a github repo, or an organization shorthand for a github repo.
   [[nodiscard]] static bool validate_location(std::string_view s);
   /// Test to see if a combination of values are valid.
   /// @param loc  The from/location field of a dependency. Empty is valid.
   /// @param tag  The tag field of a dependency. Empty is valid.
   /// @param rel  The rel field of a dependency. Empty is valid.
   /// @param hash  The hash field of a dependency. Empty is valid.
   /// @return true indicates the values passed in are a valid combination.
   [[nodiscard]] static bool validate_location(std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash,
         std::ostream& os=std::cerr);
   
private:
   std::string m_name;          ///< Name of the dependency.
   std::string m_loc;           ///< Location of the dep: local or remote archive, github repo (https: or org shorthand)
   std::string m_tag_or_commit; ///< github tag or commit hash. Always prefer a commit hash.
   std::string m_rel;           ///< github release version. Not valid with tag_or_commit.
   std::string m_hash;          ///< valid when m_loc is an archive (including github release version).
   patch_list_t m_patchfiles;   ///< List of patch files.
};

} // namespace antler::project

// TODO in the future use something like meta_refl to simply reflect
// the objects and one overload in manifest
/// Overloads for our datatype conversions
namespace YAML {
   template<>
   struct convert<antler::project::dependency> {
      static Node encode(const antler::project::dependency& o) {
         Node n;
         //TODO we will need to readdress when adding support for tests
         n["name"] = std::string(o.name());
         n["location"] = std::string(o.location());
         n["tag"] = std::string(o.tag());
         n["release"] = std::string(o.release());
         n["hash"] = std::string(o.hash());

         return n;
      }

      static bool decode(const YAML::Node& n, antler::project::dependency& d) {
         try {
            d.name(n["name"].as<std::string>());
            d.location(n["location"].as<std::string>());
            d.tag(n["tag"].as<std::string>());
            d.release(n["release"].as<std::string>());
            d.hash(n["hash"].as<std::string>());
         } catch(const YAML::Exception& ex) {
            antler::system::print_error(ex);
            return false;
         }
         return true;
      }
   };
}

namespace std {
   template <>
   struct hash<antler::project::dependency> {
      std::size_t operator()(const antler::project::dependency& d) const {
         return std::hash<std::string>{}(d.name());
      }
   };
}