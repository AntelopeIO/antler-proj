#ifndef antler_project_location_h
#define antler_project_location_h

#include <string_view>

namespace antler {
namespace project {
namespace location {


bool is_archive(std::string_view s);
bool is_github_repo(std::string_view s);
bool is_github_archive(std::string_view s);
bool is_local_file(std::string_view s);
bool is_org_repo_shorthand(std::string_view s);

} // namespace location
} // namespace project
} // namespace antler

#endif
