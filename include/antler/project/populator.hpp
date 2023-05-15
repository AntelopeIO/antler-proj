#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include "cmake.hpp"

#include <unordered_map>
#include <string_view>
#include <utility>

namespace antler::project {
class populator {
public:
   inline populator(project& proj)
      : emitter(proj), proj(&proj) {
      system::debug_log("populator created from project : ", proj.name());
   }

   populator(const populator&) = delete;
   populator(populator&&)      = default;

   /// Populate the directory by generating files for build and gathering the dependencies.
   /// @return true for success; false for failure.
   [[nodiscard]] bool populate();

   template <typename P>
   void emit_cmake(P& pops) { emitter.emit(pops); }

private:
   /// Populate a given project.
   /// @param proj The project to populate from.
   /// @return true for success; false for failure.
   [[nodiscard]] bool populate_project(project& proj);

   /// Populate a particular dependency.
   /// @param dep The dependency to populate.
   /// @return true for success; false for failure.
   [[nodiscard]] bool populate_dependency(const dependency& dep, const project& proj);


   cmake    emitter;
   project* proj;  // non-owning pointer
};

class populators {
public:
   static populators& instance() {
      static populators instance;
      return instance;
   }

   static populator& get(project& proj) {
      populators& inst = instance();
      if (inst.m_populators.count(proj.name()) <= 0) {
         inst.m_populators.emplace(proj.name(), populator(proj));
      }
      return inst.m_populators.at(proj.name());
   }

   static populator& get(std::string_view name) {
      return instance().m_populators.at(name);
   }

   static void add_mapping(std::string label, std::string mapping) {
      populators& inst = instance();
      auto        it   = inst.m_dependency_mapping.find(label);
      if (it == inst.m_dependency_mapping.end()) {
         inst.m_dependency_mapping.emplace(std::move(label), std::move(mapping));
      } else {
         ANTLER_CHECK(mapping == it->second, "Multiple mappings of project name {0}", mapping);
      }
   }

   inline static void add_mapping(const dependency& dep, std::string mapping) {
      add_mapping(dep.location(), std::move(mapping));
   }

   static std::string get_mapping(const dependency& dep) {
      populators& inst = instance();
      return inst.m_dependency_mapping.at(dep.location());
   }

   static bool mapping_exists(const dependency& dep) {
      return instance().m_dependency_mapping.count(dep.location()) > 0;
   }

   static inline void emit_cmake(project& proj) {
      populators::get(proj).emit_cmake(instance());
   }

private:
   populators() = default;
   std::unordered_map<std::string_view, populator> m_populators;
   std::unordered_map<std::string, std::string>    m_dependency_mapping;
};

}  // namespace antler::project
