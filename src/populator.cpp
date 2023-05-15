#include <antler/project/populator.hpp>
#include <antler/project/location.hpp>
#include <antler/project/version_constraint.hpp>

namespace antler::project {

bool populator::populate_dependency(const dependency& d, const project& p) {
   using namespace antler::project::location;
   system::fs::path depends_dir = p.path() / p.dependencies_dir / d.name();
   system::debug_log("populating dependency {0} from {1}", d.name(), d.location());
   std::string tag = d.tag();

   if (populators::mapping_exists(d))
      return true;

   // this dependency is a local dependency
   if (d.location().empty())
      return true;

   if (is_github_shorthand(d.location())) {
      std::string org  = std::string{github::get_org(d.location())};
      std::string repo = std::string{github::get_repo(d.location())};

      if (d.tag().empty()) {
         tag = get_github_default_branch(org, repo);
      }

      system::debug_log("Cloning {0} with branch {1}", d.location(), tag);
      if (system::fs::exists(depends_dir))
         return pull_git_repo(depends_dir);
      else

         return clone_github_repo(org, repo, tag, depends_dir);
   } else {
      system::error_log("Dependency {0} is not a github shorthand.", d.name());
      return false;
   }

   // TODO still need to support recursion into general git repos and archives.
   // if (is_reachable(d.location())) {
   //   if (is_github_repo(d.location())) {
   //      if (d.tag().empty()) {
   //         tag = "main";
   //      }
   //      return clone_git_repo(d.location(), tag, depends_dir / d.name());
   //   } else if (is_github_shorthand(d.location())) {
   //      std::string org = std::string{github::get_org(d.location())};
   //      std::string repo = std::string{github::get_repo(d.location())};

   //      if (d.tag().empty()) {
   //         tag = get_github_default_branch(org, repo);
   //      }
   //      system::debug_log("Cloning {0} with branch {1}", d.location(), tag);
   //      return clone_github_repo(org, repo, tag, depends_dir / d.name());
   //   }
   //}
   // return false;
}

bool populator::populate_project(project& p) {
   system::fs::path depends_dir = p.path() / p.dependencies_dir;

   system::fs::create_directories(depends_dir);

   system::debug_log("populating project {0}", p.name());


   ANTLER_CHECK(system::fs::exists(depends_dir), "internal failure, dependencies directory was not created {0}", depends_dir.string());

   const auto& pop_deps = [&](auto& objs) {
      for (const auto& [_, o] : objs) {
         for (const auto& [_, d] : o.dependencies()) {
            if (!populate_dependency(d, p))
               return false;
            try {
               if (d.location().empty())
                  continue;
               if (!populators::mapping_exists(d)) {
                  system::info_log("Grabbing dependency location {0}", d.location());
                  project next_proj(depends_dir / d.name());  // github::get_repo(d.location()));

                  // check that the version we grabbed is what we can work with
                  if (!d.release().empty()) {
                     version_constraint vc(d.release());
                     ANTLER_CHECK(vc.test(next_proj.version()),
                                  "Project {0} version is too low {1}, given version {2} as a constraint",
                                  next_proj.name(),
                                  next_proj.version().to_string(),
                                  d.release());
                  }

                  populators::add_mapping(d, std::string(next_proj.name()));
                  if (!populate_project(next_proj))
                     return false;
               }
            } catch (const std::exception& e) {
               system::error_log("Failed to load project: {0}", e.what());
               return false;
            } catch (...) {
               system::error_log("Failed to load project");
               return false;
            }
         }
      }
      return true;
   };

   if (!pop_deps(p.apps())) {
      system::error_log("Failed to populate apps");
      return false;
   }
   if (!pop_deps(p.libs())) {
      system::error_log("Failed to populate libs");
      return false;
   }

   populators::emit_cmake(p);
   return true;
}


bool populator::populate() {
   populators::add_mapping("", std::string(proj->name()));  // add the default empty location to this project mapping
   return populate_project(*proj);
}

}  // namespace antler::project