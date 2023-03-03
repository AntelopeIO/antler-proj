#pragma once

/// @copyright See `LICENSE` in the root directory of this project.

#include "version.hpp"
#include <vector>
#include <iostream>


namespace antler::project {

/// This class encapsulates a version constraint. (e.g `package > 3.0.2`, `package > 3.0.2 < 4 | > 4.1`)
class version_constraint {
public:
   using self = version_constraint; ///< Alias for self.

   /// Default constructor.
   version_constraint();
   /// Construct from a string.
   /// @param ver  The string to parse into a constraint.
   explicit version_constraint(std::string_view ver);

   /// @param ver  The string to parse into a constraint.
   self& operator=(std::string_view ver);

   /// Clear this constraint.
   void clear();

   /// @return true if the constraint is a iunque value (e.g. `== 3.0.2` as opposed to `>= 3.0.2` or `== 3.0.2 | == 4.0.)
   [[nodiscard]] bool is_unique() const noexcept;

   /// @return true if this constraint is empty or invalid.
   [[nodiscard]] bool empty() const noexcept;
   /// @return The raw string this constraint was built from.
   [[nodiscard]] std::string_view raw() const noexcept;

   /// @note if empty() would return true, this function will ALWAYS return true.
   /// @param ver  The version to test against this constraint.
   /// @return true if ver met this constraint; false, otherwise.
   [[nodiscard]] bool test(const version& ver) const noexcept;

   /// Print this constraint to a stream.
   void print(std::ostream& os) const noexcept;

private:
   /// Attempt to parse and load this version from a string. Print warnings and errors to os.
   void load(std::string_view s, std::ostream& os = std::cerr);


   std::string m_raw;

   /// enum clas to indicate inclusivity of bounds.
   enum class bounds_inclusivity {
      none,                     ///< None, neither bound is inclusive. (lower,upper)
      lower,                    ///< Lower only. [lower,upper)
      upper,                    ///< Upper only. (lower,upper]
      both,                     ///< Both are inclusive. [lower,upper]
      unique,                   ///< Unique: only lower is valid. [lower,lower]
   };
   struct constraint {
      version lower_bound;
      version upper_bound;      // Invalid/unset *if* inclusivity is `unique`.
      bounds_inclusivity inclusivity;
   };

   std::vector<constraint> m_constraints; // The list of constraints.
};


} // namespace antler::project


inline std::ostream& operator<<(std::ostream& os, const antler::project::version_constraint& o) { o.print(os); return os; }
//std::istream& operator>>(std::istream& is, antler::project::object::version& e);
