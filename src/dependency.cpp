/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/dependency.hpp>
#include <antler/project/location.hpp>

#include <algorithm> // std::sort, std::find()


namespace {

inline bool is_valid_hash(std::string_view s, size_t byte_count = 32) noexcept {
   if (s.size() != byte_count)
      return false;
   for (auto a : s) {
      if (!(a >= '0' && a <= '9') && !(a >= 'a' && a <= 'f') && !(a >= 'A' && a <= 'F')) {
         return false;
      }
   }
   return true;
}


} // anonymous namespace


namespace antler::project {



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


void dependency::patch_add(const system::fs::path& path) noexcept {
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


void dependency::patch_remove(const system::fs::path& path) noexcept {

   auto i = std::find(m_patchfiles.begin(), m_patchfiles.end(), path);
   if (i != m_patchfiles.end())
      m_patchfiles.erase(i);
}

void dependency::set(std::string nm, std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash) {

   m_loc = location::strip_github_com(loc);
   if (nm.empty()) {
      m_name = github::get_repo(m_loc);
   } else {
      m_name = std::move(nm);
   }



   m_tag_or_commit = tag;
   m_rel           = rel;
   m_hash          = hash;
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


const std::string& dependency::tag() const noexcept {
   return m_tag_or_commit;
}


void dependency::tag(std::string_view s) noexcept {
   m_tag_or_commit = s;
}

bool dependency::is_valid_location() const noexcept {
   if (!m_tag_or_commit.empty()) {
      if (!m_rel.empty()) {
         std::cerr << "release AND tag/commit flags are not valid at the same time for location.";
         return false;
      }
      if (!m_hash.empty()) {
         std::cerr << "hash AND tag/commit flags are not valid at the same time for location.";
         return false;
      }
   }
   return location::is_reachable(m_loc);
}

bool dependency::validate_location(std::string_view s) {

   return location::is_archive(s) || location::is_github_repo(s) || location::is_github_shorthand(s);
}


bool dependency::validate_location(std::string_view loc, std::string_view tag, std::string_view rel, std::string_view hash) {

   if (!tag.empty()) {
      if (!hash.empty()) {
         system::warn_log("tag and hash flags are not valid at the same time for location.");
         return false;
      }
   }

   return location::is_reachable(loc);
}

bool dependency::retrieve() {
   return false;
}

} // namespace antler::project
