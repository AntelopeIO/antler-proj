#ifndef antler_project_version_constraint_h
#define antler_project_version_constraint_h

#include <antler/project/version.h>
#include <vector>
#include <iostream>


namespace antler {
namespace project {

class version_constraint {
public:
   using self = version_constraint;

   version_constraint();
   version_constraint(std::string_view ver);

   self& operator=(std::string_view ver);

   void clear();

   bool is_unique() const noexcept;

   bool empty() const noexcept;
   std::string_view raw() const noexcept;

   /// @note if empty() would return true, this function will ALWAYS return true.
   /// @return true if ver met this constraint; false, otherwise.
   bool test(const version& ver) const noexcept;

   void print(std::ostream& os) const noexcept;

private:

   void load(std::string_view s, std::ostream& os=std::cerr);


   std::string m_raw;

   enum class bounds_inclusivity {
      none,
      lower,
      upper,
      both,
      unique,
   };

   struct constraint {
      version lower_bound;
      version upper_bound;  // Unset *if* inclusivity is `unique`.
      bounds_inclusivity inclusivity;
   };
   std::vector<constraint> m_constraints;
};


} // namespace project
} // namespace antler

inline std::ostream& operator<<(std::ostream& os, const antler::project::version_constraint& o) { o.print(os); return os; }
//std::istream& operator>>(std::istream& is, antler::project::object::version& e);

#endif
