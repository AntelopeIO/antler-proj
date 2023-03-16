#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include "version.hpp"
#include <iostream>
#include <optional>
#include <vector>
#include <utility>


namespace antler::project {

   enum class relation {
      lt, ///< less than
      le, ///< less than or equal
      gt, ///< greater than
      ge, ///< greater than or equal
      eq  ///< equal
   };

   static std::string relation_to_string(relation rel) {
      switch(rel) {
         case relation::lt:
            return "<";
         case relation::le:
            return "<=";
         case relation::gt:
            return ">";
         case relation::ge:
            return ">=";
         default:
            return "";
      }
   }

   struct bound {
      relation rel;
      version  ver;

      inline std::string to_string() const { return relation_to_string(rel) + " " + ver.to_string(); }
   };

   struct constraint {
      bound lower_bound;
      std::optional<bound> upper_bound;

      inline std::string to_string() const { return lower_bound.to_string() + (upper_bound ? ", " + upper_bound->to_string() : ""); }
   };

   /// This class encapsulates a version constraint. (e.g `package > 3.0.2`, `package > 3.0.2 < 4 | > 4.1`)
   class version_constraint {
   public:

      /// Default constructor.
      version_constraint() = default;

      /// Construct from a string.
      /// @param ver  The string to parse into a constraint.
      explicit version_constraint(std::string_view ver) {
         system::debug_log("version_constraint(std::string_view ver) called with constraint : {0}", ver);
         ANTLER_CHECK(parse(ver), "invalid version constraint : {0}", ver);
      }

      /// @param ver  The string to parse into a constraint.
      version_constraint& operator=(std::string_view ver) {
         system::debug_log("operator=(std::string_view ver) called with constraint : {0}", ver);
         parse(ver);
         ANTLER_CHECK(parse(ver), "invalid version constraint : {0}", ver);
         return *this;
      }

      /// Clear this constraint.
      inline void clear() { m_constraints.clear(); }

      /// @return true if the constraint is a unque value (e.g. `== 3.0.2` as opposed to `>= 3.0.2` or `== 3.0.2 | == 4.0.)
      [[nodiscard]] bool is_unique() const noexcept;

      /// @return true if this constraint is empty or invalid.
      [[nodiscard]] bool empty() const noexcept { return !m_constraints.empty(); }

      /// @return the string representation of this constraint.
      [[nodiscard]] std::string to_string() const noexcept;

      /// @note if empty() would return true, this function will ALWAYS return true.
      /// @param ver  The version to test against this constraint.
      /// @return true if ver met this constraint; false, otherwise.
      [[nodiscard]] bool test(const version& ver) const noexcept;

      /// Print this constraint to a stream.
      void print(std::ostream& os) const noexcept;

      /// get the constraints
      /// @return a vector of constraints.
      [[nodiscard]] inline const std::vector<constraint>& constraints() const noexcept { return m_constraints; }

      /// Serialization function from version to yaml node
      [[nodiscard]] inline yaml::node_t to_yaml() const noexcept { return yaml::node_t{to_string()}; }

      /// Deserialization function from yaml node to version
      [[nodiscard]] inline bool from_yaml(const yaml::node_t& n) noexcept {
         std::string s = n.as<std::string>();
         return parse({s.c_str(), s.size()});
      }


   private:
      /// Attempt to parse and load this version from a string. Print warnings and errors to os.
      bool parse(std::string_view s);

      std::pair<relation, std::string_view> parse_inequality(std::string_view s);
      std::pair<version, std::string_view> parse_version(std::string_view s);


      std::vector<constraint> m_constraints; // The list of constraints.
   };

   inline std::ostream& operator<<(std::ostream& os, const antler::project::version_constraint& o) { o.print(os); return os; }

} // namespace antler::project

