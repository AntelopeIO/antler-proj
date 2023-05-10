#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include "project.hpp"
#include "net_utils.hpp"

#include <fstream>
#include <stdexcept>
#include <string_view>


namespace antler::project {

   /// @brief struct to encapsulate a CMakeLists.txt file
   struct cmake_lists {
      constexpr inline static std::string_view filename = "CMakeLists.txt";

      /// @brief constructor
      /// @param p base path for where this CMakeLists.txt will reside
      cmake_lists(system::fs::path p)
         : path(p / filename) {
         system::fs::create_directories(p);
         outs.open(path);
         ANTLER_CHECK(outs.is_open(), "Error creating/opening CMakeLists.txt at {0}", path.string());
         system::debug_log("cmake_lists constructed at {0}.", path.string());
      }

      cmake_lists(const cmake_lists&) = delete;
      cmake_lists& operator=(const cmake_lists&) = delete;

      cmake_lists(cmake_lists&& o) {
         path = std::move(o.path);
         outs = std::move(o.outs);
         system::debug_log("cmake_lists moved at {0}.", path.string());
      }
      cmake_lists& operator=(cmake_lists&& o) {
         path = std::move(o.path);
         outs = std::move(o.outs);
         system::debug_log("cmake_lists moved at {0}.", path.string());
         return *this;
      }

      ~cmake_lists() { outs.close(); }

      inline system::fs::path base_path() const noexcept { return path.parent_path(); }

      /// @brief stream operator insertion overload
      /// @tparam T type of object will be inserted
      /// @param v object that will be inserted in this CMakeLists.txt
      /// @return CMakeLists stream
      template<typename T>
      inline std::ostream& operator<<(T&& v) {
         outs << std::forward<T>(v);
         outs.flush();
         return outs;
      }

      inline void flush() { outs.flush(); }

      system::fs::path path;
      std::ofstream outs;
   };

   /// @brief Object to house emission of CMake for antler-proj projects
   class cmake {
      public:
         constexpr inline static uint16_t minimum_major = 3;
         constexpr inline static uint16_t minimum_minor = 10;
         constexpr inline static std::string_view build_dir_name  = "build";
         constexpr inline static std::string_view apps_dir_name   = "apps";
         constexpr inline static std::string_view libs_dir_name   = "libs";
         constexpr inline static std::string_view tests_dir_name  = "tests";

         // std::format templates for the cmake
         static std::string_view add_subdirectory_template;
         static std::string_view add_subdirectory2_template;
         static std::string_view preamble_template;
         static std::string_view project_stub_template;
         static std::string_view target_compile_template;
         static std::string_view target_include_template;
         static std::string_view target_link_libs_template;
         static std::string_view entry_template;
         static std::string_view add_contract_template;
         static std::string_view add_library_template;

         cmake(const project& proj)
            : proj(&proj),
            base_path(proj.path() / build_dir_name),
            base_lists(base_path),
            apps_lists(base_path / apps_dir_name),
            libs_lists(base_path / libs_dir_name),
            tests_lists(base_path / tests_dir_name) {
            system::debug_log("cmake(const project&) constructed");
            system::debug_log("project name = {0}", proj.name());
         }

         cmake(const cmake&) = delete;
         cmake(cmake&& o)
            : proj(o.proj),
            base_path(std::move(o.base_path)),
            base_lists(std::move(o.base_lists)),
            apps_lists(std::move(o.apps_lists)),
            libs_lists(std::move(o.libs_lists)),
            tests_lists(std::move(o.tests_lists)) {
            system::debug_log("cmake(cmake&&) constructed");
        }

         template <typename T>
         inline std::string target_name(T&& obj) {
            using namespace std::literals;
            return std::string(proj->name()) + "-"s + std::string(obj.name());
         }

         template <typename Stream>
         inline void emit_add_subdirectory(Stream& s, system::fs::path path, std::string_view name) noexcept {
            if (path.empty()) 
               return;
            s << fmt::format(add_subdirectory_template, (path / name).string());
         }

         template <typename Stream>
         inline void emit_preamble(Stream& s) noexcept {
            s << fmt::format(preamble_template,
                             "antler-proj",
                             minimum_major,
                             minimum_minor,
                             proj->name(),
                             proj->version().major(),
                             proj->version().minor(),
                             proj->version().patch());
         }

         template <typename Stream>
         inline void emit_project_stub(Stream& s) noexcept {
               s << fmt::format(project_stub_template);
         }

         template <typename Populators, typename Stream, typename Tag>
         inline void emit_dependencies(Populators&& pops, Stream& s, const object<Tag>& obj) noexcept {
            std::string obj_target = "";
            std::string dep_target = "";
            for (const auto& [k, dep] : obj.dependencies()) {
               system::debug_log("emitting dependencies for {0} at {1}", dep.name(), dep.location().empty() ? "local" : dep.location());
               if (!dep.location().empty()) {
                  std::string repo = std::string(github::get_repo(dep.location()));
                  s << fmt::format(add_subdirectory2_template,
                                   "../../dependencies/"+dep.name()+"/build/apps/",
                                   repo);
                  obj_target = target_name(obj);
                  dep_target = pops.get_mapping(dep)+"-"+dep.name();
               } else {
                  obj_target = target_name(obj);
                  dep_target = target_name(dep);
               }

               s << fmt::format(target_link_libs_template,
                                obj_target,
                                dep_target);
            }
            s << "\n";
         }

         template <typename Stream>
         inline void emit_entry(Stream& s) noexcept { s << fmt::format(entry_template, proj->name()); }

         template <typename Pops, typename Stream, typename Tag>
         inline void emit_object(Pops& pops, Stream& s, const object<Tag>& obj) {
            std::string_view temp = std::is_same_v<Tag, app_tag> ? add_contract_template : add_library_template;

            s << fmt::format(temp,
                             obj.name(),
                             target_name(obj),
                             system::extension(obj.language()));

            s << fmt::format(target_include_template,
                             target_name(obj),
                             obj.name());

            for (const auto& o : obj.compile_options()) {
               s << fmt::format(target_compile_template,
                                target_name(obj),
                                o);
            }

            s << "\n";

            for (const auto& o : obj.link_options()) {
               s << fmt::format(target_link_libs_template,
                                target_name(obj),
                                                              o);
            }

            s << "\n";

            emit_dependencies(pops, s, obj);
            s << "\n";
         }

         template <typename Pops, typename Stream, typename Objs>
         void emit_objects(Pops& pops, Stream& s, std::string_view dir, Objs&& objs) {
            for (const auto& [k, obj] : objs) {
               emit_add_subdirectory(s, dir, obj.name());
               cmake_lists obj_lists(s.base_path() / obj.name());
               emit_object(pops, obj_lists, obj);
            }
         }

         template <typename Pops>
         void emit(Pops& pops) {
            system::info_log("Emitting CMake for project {0}", proj->name());
            ANTLER_CHECK(proj, "internal failure, proj is null");

            emit_preamble(base_lists);
            emit_entry(base_lists);

            emit_preamble(apps_lists);
            emit_project_stub(apps_lists);

            emit_objects(pops, apps_lists, ".", proj->apps());
            emit_objects(pops, libs_lists, "../libs", proj->libs());

            base_lists.flush();
            apps_lists.flush();
            libs_lists.flush();
            tests_lists.flush();
         }

      private:

         const project*   proj = nullptr; // non-owning pointer to project
         system::fs::path base_path;
         cmake_lists      base_lists;
         cmake_lists      apps_lists;
         cmake_lists      libs_lists;
         cmake_lists      tests_lists;
   };

} // namespace antler::project
