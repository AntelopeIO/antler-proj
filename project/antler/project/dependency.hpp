#pragma once

/// @copyright See `LICENSE` in the root directory of this project.


#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <utility> // std::pair
#include <filesystem>

#include <antler/project/version.hpp>


namespace antler {
namespace project {

/// This class models and containts the dependency portion of an ANTLER project.
class dependency {
public:
   using list_t = std::vector<dependency>; ///< Alias for the list type.

   using patch_list_t = std::vector<std::filesystem::path>; ///< Alias for the patch file list type.

public:
   // use default constructors, copy and move constructors and assignments


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
   std::string_view name() const noexcept;
   /// Set the dependency name.
   /// @param s  The new name for this dependency.
   void name(std::string_view s) noexcept;

   /// Get the location field of this dependency.
   /// @return the from location of this dependency.
   std::string_view location() const noexcept;
   /// Set the location field of this dependency.
   /// @param s  The new from location of this dependency.
   void location(std::string_view s) noexcept;

   /// Report on the status of this dependencies from field: does it look like an archive?
   /// @return true if location ends in an archive format (e.g. ".tar.gz", ".tgz", etc")
   bool is_archive() const noexcept;

   /// Report on the status of version information.
   /// @return true if both tag() and rel() would return empty.
   bool empty_version() const noexcept;

   /// Get the github tag/commit hash.
   /// @return github tag/commit hash.
   std::string_view tag() const noexcept;
   /// Set github tag/commit hash.
   /// @param s  The new github tag/commit hash.
   void tag(std::string_view s) noexcept;

   /// Get the github release version.
   /// @return The possibly empty release version.
   std::string_view release() const noexcept;
   /// Set the new github release version.
   /// @param s  The new, possibly empty, release version.
   void release(std::string_view s) noexcept;

   /// Get the archive or release hash.
   /// @note For commit hash, see tag()
   /// @note only valid for archive and release.
   /// @return The archive or release hash.
   std::string_view hash() const noexcept;
   /// Set the archive or release hash.
   /// @note For commit hash, see tag()
   /// @note only valid for archive and release.
   /// @param s  The new archive or release hash.
   void hash(std::string_view s) noexcept;

   /// Return the list of patch files.
   /// @note These should be in a location relative to the `project.yaml` file.
   /// @return  The list of patch files.
   const patch_list_t& patch_files() const noexcept;
   /// Add a new file to the patch list.
   /// @note this should be in a location relative to the `project.yaml` file.
   /// @param path  The path to the patch file to add.
   void patch_add(const std::filesystem::path& path) noexcept;
   /// @note this should be in a location relative to the `project.yaml` file.
   /// @param path  The path to the patch file to remove.
   void patch_remove(const std::filesystem::path& path) noexcept;


   /// Test to see if a location is valid.
   /// @return true if s is an archive, a github repo, or an organization shorthand for a github repo.
   static bool validate_location(std::string_view s);
   /// Test to see if a combination of values are valid.
   /// @param loc  The from/location field of a dependency. Empty is valid.
   /// @param tag  The tag field of a dependency. Empty is valid.
   /// @param rel  The rel field of a dependency. Empty is valid.
   /// @param hash  The hash field of a dependency. Empty is valid.
   /// @return true indicates the values passed in are a valid combination.
   static bool validate_location(std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash,
         std::ostream& os=std::cerr);


private:
   std::string m_name;          ///< Name of the dependency.
   std::string m_loc;           ///< Location of the dep: local or remote archive, github repo (https: or org shorthand)
   std::string m_tag_or_commit; ///< github tag or commit hash. Always prefer a commit hash.
   std::string m_rel;           ///< github release version. Not valid with tag_or_commit.
   std::string m_hash;          ///< valid when m_loc is an archive (including github release version).
   patch_list_t m_patchfiles;   ///< List of patch files.
};


} // namespace project
} // namespace antler
