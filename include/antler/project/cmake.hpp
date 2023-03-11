#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/project.hpp>

#include <filesystem> // path
#include <fstream>
#include <stdexcept>
#include <string_view>

#include <mustache.hpp>


namespace antler::project {

   namespace km = kainjow::mustache;

   /// @brief struct to encapsulate a CMakeLists.txt file
   struct cmake_lists {
      constexpr inline static std::string_view cmake_lists_fn = "CMakeLists.txt";

      /// @brief constructor
      /// @param p base path for where this CMakeLists.txt will reside
      cmake_lists(std::filesystem::path p)
         : path(p / cmake_lists_fn),
           outs(path) {
         std::filesystem::create_directories(p);
         system::debug_log("cmake_lists(std::filesystem::path) constructed.");
         system::debug_log("path = {0} ", path.string());
      }

      ~cmake_lists() { outs.close(); }

      /// @brief stream operator insertion overload
      /// @tparam T type of object will be inserted
      /// @param v object that will be inserted in this CMakeLists.txt
      /// @return CMakeLists stream
      template<typename T>
      inline std::ostream& operator<<(T&& v) { 
         outs << std::forward<T>(v); 
         return outs;
      }

      std::filesystem::path path;
      std::ofstream outs;
   };

   /// @brief Object to house emission of CMake for antler-proj projects
   class cmake {
      public:
         constexpr inline static uint16_t minimum_major = 3;
         constexpr inline static uint16_t minimum_minor = 11;
         constexpr inline static std::string_view build_dir_name  = "build";
         constexpr inline static std::string_view apps_dir_name   = "build";
         constexpr inline static std::string_view libs_dir_name   = "build";
         constexpr inline static std::string_view tests_dir_name  = "build";

         // `mustache` templates for the cmake
         static km::mustache add_subdirectory_template;
         static km::mustache preamble_template;
         static km::mustache project_stub_template;
         static km::mustache target_compile_template;
         static km::mustache target_include_template;
         static km::mustache target_link_libs_template;
         static km::mustache entry_template;
         static km::mustache add_action_template;
         static km::mustache add_library_template;

         cmake(const project& proj)
            : proj(&proj),
            base_path(proj.path().parent_path() / build_dir_name),
            base_lists(base_path),
            apps_lists(base_path / apps_dir_name),
            libs_lists(base_path / apps_dir_name),
            tests_lists(base_path / apps_dir_name) {
            system::debug_log("cmake(const project&) constructed");
            system::debug_log("project name = {0}", proj.name());
         }

         template <typename T>
         inline std::string target_name(T&& obj) {
            using namespace std::literals;
            return std::string(proj->name()) + "-"s + std::string(obj.name());
         }

         template <typename Stream>
         inline void emit_add_subdirectory(Stream& s, std::filesystem::path path, std::string_view name) noexcept { 
            s << add_subdirectory_template.render(datum{"path", (path / name).string()});
         }

         template <typename Stream>
         inline void emit_preamble(Stream& s) noexcept {
            s << preamble_template.render(datum{"tool", "antler-proj"}
                                               ("major", minimum_major)
                                               ("minor", minimum_minor)
                                               ("proj_name", proj->name())
                                               ("proj_major", proj->version().major())
                                               ("proj_minor", proj->version().minor())
                                               ("proj_patch", proj->version().patch()));
         }

         template <typename Stream>
         inline void emit_project_stub(Stream& s) noexcept { s << project_stub_template.render({}); }

         template <typename Stream, typename Tag>
         inline void emit_dependencies(Stream& s, const object<Tag>& obj) noexcept {
            for (const auto& [k, dep] : obj.dependencies()) {
               std::string dep_name = dep.location().empty() ? target_name(dep) : std::string(dep.name());
               s << target_link_libs_template.render(datum{"target_name", target_name(obj)}
                                                          ("dep_name", dep_name));
            }
            s << "\n";
         }
         template <typename Stream>
         inline void emit_entry(Stream& s) noexcept { s << entry_template.render(datum{"proj", proj->name()}); }
   
         template <typename Stream, typename Tag>
         inline void emit_object(Stream& s, const object<Tag>& obj) {
            km::mustache& temp = std::is_same_v<Tag, app_t> ? add_action_template : add_library_template;

            s << temp.render(datum{"obj_name", obj.name()}
                                  ("target_name", target_name(obj))
                                  ("obj_source", std::string(obj.name())+system::extension(obj.language())));

            for (const auto& o : obj.compile_options()) {
               s << target_compile_template.render(datum{"target_name", target_name(obj)}
                                                        ("opt", o));
            }

            s << "\n";

            for (const auto& o : obj.link_options()) {
               s << target_link_libs_template.render(datum{"target_name", target_name(obj)}
                                                          ("dep_name", o));
            }

            s << "\n";

            emit_dependencies(s, obj);
            s << "\n";
         }

         template <typename Stream, typename Objs>
         void emit_objects(Stream& s, std::string_view dir, Objs&& objs) {
            for (const auto& [k, obj] : objs) {
               emit_add_subdirectory(s, dir, obj.name());
               emit_object(s, obj);
            }
         }

         template <typename Stream, typename Tag>
         inline void emit_dependency(Stream& s, const object<Tag>& dep) noexcept {
         }

         void emit() {
            if (!proj) {
               throw std::runtime_error("internal failure, proj is null");
            }

            emit_preamble(base_lists);
            emit_entry(base_lists);

            emit_preamble(apps_lists);
            emit_project_stub(apps_lists);

            emit_objects(apps_lists, ".", proj->apps());
            emit_objects(libs_lists, "../libs", proj->libs());
         }

      private:
         
         // simple helper to clean km::data usage
         struct datum {
            template <typename T>
            decltype(auto) fwrd(T&& val) {
               std::stringstream ss;
               ss << std::forward<T>(val);
               return ss.str();
            }

            template <typename Str, typename T>
            inline datum(Str&& key, T&& val) 
               : data(std::forward<Str>(key), fwrd(std::forward<T>(val))) {}
            
            template <typename Str, typename T>
            datum& operator()(Str&& key, T&& val) {
               data.set(std::forward<Str>(key), fwrd(std::forward<T>(val)));
               return *this;
            }

            operator km::data() const { return data; }
            operator km::data&() { return data; }

            km::data data;
         };

         const project* proj = nullptr; // non-owning pointer to project
         std::filesystem::path base_path;
         cmake_lists           base_lists;
         cmake_lists           apps_lists;
         cmake_lists           libs_lists;
         cmake_lists           tests_lists;
   };

} // namespace antler::project
