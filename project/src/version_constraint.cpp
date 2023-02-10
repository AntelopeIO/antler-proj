/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version.hpp>
#include <antler/project/version_constraint.hpp>
#include <antler/string/from.hpp>

#include <iostream>
#include <limits>

#include <boost/algorithm/string.hpp> // boost::split()


std::ostream& operator<<(std::ostream& os, const std::vector<std::string_view>& v) {
   os << "[";
   if (!v.empty()) {
      auto i = v.begin();
      os << '"' << *i << '"';
      for (++i; i != v.end(); ++i)
         os << ",\"" << *i << '"';
   }
   os << "]";
   return os;
}


namespace antler::project {


namespace { // anonymous

constexpr semver::value_type min_semver_val = std::numeric_limits<semver::value_type>::min();
constexpr semver::value_type max_semver_val = std::numeric_limits<semver::value_type>::max();

const semver min_semver{ min_semver_val, min_semver_val, min_semver_val };
const semver max_semver{ max_semver_val, max_semver_val, max_semver_val };

const version min_version{ min_semver };
const version max_version{ max_semver };


/// Trim whitespace from the front and back of string.
/// @param s  The string to trim
/// @return  An empty string or a string that starts with non-whitespace and ends with non-whitespace
inline std::string_view trim(std::string_view s) {
   // Find an iterator to the firs non-space char; if we went to the end, return an empty string.
   auto first = std::find_if(s.begin(), s.end(), [](unsigned char ch) {return !std::isspace(ch);});
   if(first == s.end())
      return {};
   // Find the last non space character.
   auto last = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {return !std::isspace(ch);});
   // Convert the reverse iter last to the forward iterator containing end using base().
   //    See: https://en.cppreference.com/w/cpp/iterator/reverse_iterator/base
   return std::string_view{first, last.base()};
}


} // anonymous namespace



//--- constructors/destructor ------------------------------------------------------------------------------------------

version_constraint::version_constraint() = default;


version_constraint::version_constraint(std::string_view ver) {
   load(ver);
}


//--- operators ---------------------------------------------------------------------------------------------------------

version_constraint& version_constraint::operator=(std::string_view ver) {
   load(ver);
   return *this;
}


//--- alphabetic --------------------------------------------------------------------------------------------------------

void version_constraint::clear() {
   m_raw.clear();
   m_constraints.clear();
}


bool version_constraint::empty() const noexcept {
   return m_constraints.empty();
}


bool version_constraint::is_unique() const noexcept {
   return m_constraints.size() == 1 && m_constraints[0].inclusivity == bounds_inclusivity::unique;
}


void version_constraint::load(std::string_view sin, std::ostream& os) {

   // Trim whitespace from both ends and set the raw value. Then clear constraints in preparation for further parsing.
   m_raw = trim(sin);
   m_constraints.clear();

   // But return if s is now empty.
   if (m_raw.empty())
      return;

   // Start by splitting on '|'
   std::vector<std::string_view> splits;
   boost::split(splits, m_raw, boost::is_any_of("|;"));
   for (auto split : splits) {
      // Now split on ','
      std::vector<std::string_view> element;
      boost::split(element, split, boost::is_any_of(","));
      if (element.size() == 1) {
         // If there's only one constraint, we need to decide if it's an upper bound, a lower bound, or unique.
         auto trimmed_el = trim(element[0]);
         std::vector<std::string_view> el_list;
         boost::split(el_list, trimmed_el, boost::is_any_of(" "));

         if (el_list.size() == 1) {
            // One member MUST be a unique.
            auto ver_str = trim(el_list[0]);
            m_constraints.emplace_back(constraint{ version(ver_str), version(), bounds_inclusivity::unique });
            continue;
         }

         if (el_list.size() == 2) {
            // Two members is a bound.
            auto op_str = trim(el_list[0]);
            auto ver_str = trim(el_list[1]);

            if (op_str == "<")
               m_constraints.emplace_back(constraint{ min_version, version(ver_str), bounds_inclusivity::lower }); // inclusive of the min!
            else if (op_str == "<=")
               m_constraints.emplace_back(constraint{ min_version, version(ver_str), bounds_inclusivity::both });
            else if (op_str == ">")
               m_constraints.emplace_back(constraint{ version(ver_str), max_version, bounds_inclusivity::upper }); // inclusive of the max!
            else if (op_str == ">=")
               m_constraints.emplace_back(constraint{ version(ver_str), max_version, bounds_inclusivity::both });
            else {
               os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad op: \"" << el_list << "\"\n";
               clear();
               return;
            }

            continue;
         }

         os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Too many or too few elements in: \"" << el_list << "\"\n";
         clear();
         return;
      }

      if (element.size() == 2) {
         std::vector<std::string_view> lower_list;
         boost::split(lower_list, trim(element[0]), boost::is_any_of(" "));
         std::vector<std::string_view> upper_list;
         boost::split(upper_list, trim(element[1]), boost::is_any_of(" "));

         if (lower_list.size() != 2 || upper_list.size() != 2) {
            os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin
               << "\" Too many or too few elements in: \"" << element << "\"\n";
            clear();
            return;
         }

         auto lop = trim(lower_list[0]);
         auto lver = trim(lower_list[1]);
         auto uop = trim(upper_list[0]);
         auto uver = trim(upper_list[1]);
         if (lop == ">") {
            if (uop == "<") {
               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::none });
               continue;
            }
            if (uop == "<=") {
               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::upper });
               continue;
            }

            os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad upper limit operator in: \""
               << element << "\"\n";
            clear();
            return;
         }

         if (lop == ">=") {
            if (uop == "<") {
               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::lower });
               continue;
            }
            if (uop == "<=") {
               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::both });
               continue;
            }

            os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad upper limit operator in: \""
               << element << "\"\n";
            clear();
            return;
         }

         os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad lower limit operator in: \""
            << element << "\"\n";
         clear();
         return;
      }

      os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Too many elements in: \"" << element
         << "\"\n";
      clear();
      return;
   }
}


void version_constraint::print(std::ostream& os) const noexcept {

   if (m_constraints.empty()) {
      os << "unconstrained";
      return;
   }

   for (size_t i = 0; i < m_constraints.size(); ++i) {
      if (i)
         os << " | ";
      const auto& a = m_constraints[i];
      switch (a.inclusivity) {
         case bounds_inclusivity::none:   os << ">" << a.lower_bound << ", < " << a.upper_bound; break;
         case bounds_inclusivity::lower:  os << ">=" << a.lower_bound << ", <" << a.upper_bound; break;
         case bounds_inclusivity::upper:  os << ">" << a.lower_bound << ", <=" << a.upper_bound; break;
         case bounds_inclusivity::both:   os << ">=" << a.lower_bound << ", <=" << a.upper_bound; break;
         case bounds_inclusivity::unique: os << a.lower_bound; break;
      };
   }
}


std::string_view version_constraint::raw() const noexcept {
   return m_raw;
}


bool version_constraint::test(const version& ver) const noexcept {
   if (m_constraints.empty())
      return true;

   for (const auto& a : m_constraints) {
      switch (a.inclusivity) {
         case bounds_inclusivity::none:
            if (a.lower_bound < ver && ver < a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::lower:
            if (a.lower_bound <= ver && ver < a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::upper:
            if (a.lower_bound < ver && ver <= a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::both:
            if (a.lower_bound <= ver && ver <= a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::unique:
            if (a.lower_bound == ver)
               return true;
            break;
      }
   }
   return false;
}


} // namespace antler::project
