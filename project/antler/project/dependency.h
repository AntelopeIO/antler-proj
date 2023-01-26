#ifndef antler_project_dependency_h
#define antler_project_dependency_h

#include <string>
#include <string_view>
#include <antler/project/version.h>
#include <vector>
#include <iostream>
#include <utility> // std::pair
#include <filesystem>


namespace antler {
namespace project {

class dependency {
public:
   using list_t = std::vector<dependency>;

   using patch_list_t = std::vector<std::filesystem::path>;

public:
   // use default constructors, copy and move constructors and assignments


   /// Sets the internal values regardless of the validity.
   /// @note Check that validate_location() returns true before setting these values.
   void set(std::string_view name, std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash);


   std::string_view name() const noexcept;
   void name(std::string_view s) noexcept;

   /// @return the from location of this dependency.
   std::string_view location() const noexcept;
   void location(std::string_view s) noexcept;

   /// @return true if location ends in an archive format (e.g. ".tar.gz", ".tgz", etc")
   bool is_archive() const noexcept;

   /// @return true if version is empty
   bool empty_version() const noexcept;

   /// Sets github tag/commit hash.
   std::string_view tag() const noexcept;
   void tag(std::string_view s) noexcept;

   /// Sets github release version.
   std::string_view release() const noexcept;
   void release(std::string_view s) noexcept;


   /// hash - only valid for archive and release.
   std::string_view hash() const noexcept;
   void hash(std::string_view s) noexcept;

   /// patch list
   const patch_list_t& patch_files() const noexcept;
   void patch_add(const std::filesystem::path& path) noexcept;
   void patch_remove(const std::filesystem::path& path) noexcept;


   /// @todo implement this function!
   static bool validate_location(std::string_view s);
   /// @param loc  The from/location field of a dependency. Empty is valid.
   /// @param tag  The tag field of a dependency. Empty is valid.
   /// @param rel  The rel field of a dependency. Empty is valid.
   /// @param hash  The hash field of a dependency. Empty is valid.
   /// @return true indicates the values passed in are a valid combination.
   static bool validate_location(std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash,
         std::ostream& os=std::cerr);


private:
   std::string m_name;
   std::string m_loc;           ///< often a url?
   std::string m_tag_or_commit; ///< github tag or commit hash. Always prefer a commit hash.
   std::string m_rel;           ///< github release version. Not valid with tag_or_commit.
   std::string m_hash;          ///< valid when m_loc is an archive (including github release version).
   patch_list_t m_patchfiles;
};


} // namespace project
} // namespace antler



#endif
