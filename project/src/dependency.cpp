/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/dependency.hpp>
#include <antler/project/location.hpp>

#include <algorithm> // std::sort, std::find()


namespace {

inline bool is_valid_hash(std::string_view s, size_t byte_count = 32) noexcept {
   if (s.size() != byte_count)
      return false;
   for(auto a : s) {
      if( !(a >= '0' && a <= '9')
            && !(a >= 'a' && a <= 'f')
            &&  !(a >= 'A' && a <= 'F') ) {
         return false;
      }
   }
   return true;
}


} // anonymous namespace


namespace antler::project {


//--- constructors/destructor ------------------------------------------------------------------------------------------



//--- alphabetic --------------------------------------------------------------------------------------------------------

bool dependency::empty_version() const noexcept {
   return m_tag_or_commit.empty() && m_rel.empty();
}


std::string_view dependency::hash() const noexcept {
   return m_hash;
}


void dependency::hash(std::string_view s) noexcept {
   m_hash = s;
}


bool dependency::is_archive() const noexcept {
   return location::is_archive(m_loc);
}


std::string_view dependency::location() const noexcept {
   return m_loc;
}


void dependency::location(std::string_view s) noexcept {
   m_loc = s;
}


std::string_view dependency::name() const noexcept {
   return m_name;
}


void dependency::name(std::string_view s) noexcept {
   m_name = s;
}


void dependency::patch_add(const std::filesystem::path& path) noexcept {
   // Only add if it doesn't already exist.
   auto i = std::find(m_patchfiles.begin(), m_patchfiles.end(), path);
   if (i != m_patchfiles.end())
      return;
   m_patchfiles.push_back(path);
   std::sort(m_patchfiles.begin(), m_patchfiles.end()); // <-- this could be optimized with a binary search...
}


const dependency::patch_list_t& dependency::patch_files() const noexcept {
   return m_patchfiles;
}


void dependency::patch_remove(const std::filesystem::path& path) noexcept {

   auto i = std::find(m_patchfiles.begin(), m_patchfiles.end(), path);
   if (i != m_patchfiles.end())
      m_patchfiles.erase(i);
}


std::string_view dependency::release() const noexcept {
   return m_rel;
}


void dependency::release(std::string_view s) noexcept {
   m_rel = s;
}


void dependency::set(std::string_view name, std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash) {

   m_name = name;

   m_loc = loc;

   m_tag_or_commit = tag;
   m_rel = rel;
   m_hash = hash;
   m_patchfiles.clear();


   if (!m_tag_or_commit.empty() && !m_rel.empty()) {
      std::cerr << "Unexpectedly have tag AND release. ";
      if (is_valid_hash(m_tag_or_commit)) {
         std::cerr << "Discarding release info.\n";
         m_rel.clear();
      } else {
         std::cerr << "Discarding tag info.\n";
         m_tag_or_commit.clear();
      }
   }
}


std::string_view dependency::tag() const noexcept {
   return m_tag_or_commit;
}


void dependency::tag(std::string_view s) noexcept {
   m_tag_or_commit = s;
}

bool dependency::is_valid() const noexcept {
   if (validate_location(m_loc)) {
      //if (validate_location)
      return true;
   } else {
      return false;
   }
}

bool dependency::validate_location(std::string_view s) {

   return
      location::is_archive(s)
      || location::is_github_repo(s)
      || location::is_github_org_repo_shorthand(s)
      ;
}


bool dependency::validate_location(std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash, std::ostream& os) {

   if (!tag.empty()) {
      if (!rel.empty()) {
         os << "release AND tag/commit flags are not valid at the same time for location.";
         return false;
      }
      if (!hash.empty()) {
         os << "hash AND tag/commit flags are not valid at the same time for location.";
         return false;
      }
   }

   if (location::is_archive(loc)) {
      if (hash.empty())
         os << "Warning: archive locations should have a SHA256 hash.";
   } else if (location::is_github_repo(loc) || location::is_github_org_repo_shorthand(loc)) {
      if (rel.empty() && tag.empty())
         os << "Warning: github locations should have either a tag/commit or release field.";
   } else {
      os << "Unknown location type.";
      return false;
   }

   return true;
}


} // namespace antler::project
