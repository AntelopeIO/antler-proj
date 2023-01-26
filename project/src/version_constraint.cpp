#include <antler/project/version.h>
#include <antler/project/version_constraint.h>
#include <antler/string/from.h>
#include <antler/string/split.h>
#include <antler/string/trim.h>

#include <iostream>
#include <limits>
#include <sstream>


std::ostream& operator<<(std::ostream& os, std::vector<std::string_view> v) {
   os << "[";
   if(!v.empty()) {
      auto i = v.begin();
      os << '"' << *i << '"';
      for(++i; i != v.end(); ++i)
         os << ",\"" << *i << '"';
   }
   os << "]";
   return os;
}


namespace antler {
namespace project {


namespace { // anonymous

constexpr semver::value_type min_semver_val = std::numeric_limits<semver::value_type>::min();
constexpr semver::value_type max_semver_val = std::numeric_limits<semver::value_type>::max();

const semver min_semver{ min_semver_val, min_semver_val, min_semver_val };
const semver max_semver{ max_semver_val, max_semver_val, max_semver_val };

const version min_version{ min_semver };
const version max_version{ max_semver };

} // anonymous namespace




//--- constructors/destructor ------------------------------------------------------------------------------------------

version_constraint::version_constraint() = default;


version_constraint::version_constraint(std::string_view ver)
{
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

   // Trim whitespace from both ends.
   auto s = string::trim(sin);

   // Set the raw value and clear constraints in preparation for further parsing.
   m_raw = s;
   m_constraints.clear();

   // But return if s is now empty.
   if(s.empty())
      return;

   // Start by splitting on '|'
   for(auto split : string::split(s,"|;")) {
      // Now split on ','
      auto element = string::split(split,",");
      if(element.size() == 1) {
         // If there's only one constraint, we need to decide if it's an upper bound, a lower bound, or unique.
         auto trimmed_el = string::trim(element[0]);
         auto el_list = string::split(trimmed_el," ");

         if(el_list.size() == 1) {
            // One member MUST be a unique.
            auto ver_str = string::trim(el_list[0]);
            m_constraints.emplace_back( constraint{version(ver_str), version(), bounds_inclusivity::unique} );
            continue;
         }

         if(el_list.size() == 2) {
            // Two members is a bound.
            auto op_str = string::trim(el_list[0]);
            auto ver_str = string::trim(el_list[1]);
            if(op_str == "<")
               m_constraints.emplace_back( constraint{min_version, version(ver_str), bounds_inclusivity::lower} ); // inclusive of the min!
            else if(op_str == "<=")
               m_constraints.emplace_back( constraint{min_version, version(ver_str), bounds_inclusivity::both} );
            else if(op_str == ">")
               m_constraints.emplace_back( constraint{version(ver_str), max_version, bounds_inclusivity::upper} ); // inclusive of the max!
            else if(op_str == ">=")
               m_constraints.emplace_back( constraint{version(ver_str), max_version, bounds_inclusivity::both} );
            else
            {
               os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad op: \"" << el_list << "\"\n";
               clear();
               return;
            }

            continue;
         }

         os << __FILE__ << ":" << __LINE__
            << " Failed to decode version_constraint: \"" << sin << "\" Too many or too few elements in: \"" << el_list << "\"\n";
         clear();
         return;
      }

      if(element.size() == 2) {
         auto lower_list = string::split( string::trim(element[0]), " ");
         auto upper_list = string::split( string::trim(element[1]), " ");

         if(lower_list.size() != 2 || upper_list.size() != 2) {
            os << __FILE__ << ":" << __LINE__
                      << " Failed to decode version_constraint: \"" << sin << "\" Too many or too few elements in: \"" << element << "\"\n";
            clear();
            return;
         }

         auto lop = string::trim(lower_list[0]);
         auto lver = string::trim(lower_list[1]);
         auto uop = string::trim(upper_list[0]);
         auto uver = string::trim(upper_list[1]);
         if(lop == ">") {
            if(uop == "<") {
               m_constraints.emplace_back( constraint{version(lver), version(uver), bounds_inclusivity::none} );
               continue;
            }
            if(uop == "<=") {
               m_constraints.emplace_back( constraint{version(lver), version(uver), bounds_inclusivity::upper} );
               continue;
            }

            os << __FILE__ << ":" << __LINE__
               << " Failed to decode version_constraint: \"" << sin << "\" Bad upper limit operator in: \"" << element << "\"\n";
            clear();
            return;
         }

         if(lop == ">=") {
            if(uop == "<") {
               m_constraints.emplace_back( constraint{version(lver), version(uver), bounds_inclusivity::lower} );
               continue;
            }
            if(uop == "<=") {
               m_constraints.emplace_back( constraint{version(lver), version(uver), bounds_inclusivity::both} );
               continue;
            }

            os << __FILE__ << ":" << __LINE__
               << " Failed to decode version_constraint: \"" << sin << "\" Bad upper limit operator in: \"" << element << "\"\n";
            clear();
            return;
         }

         os << __FILE__ << ":" << __LINE__
            << " Failed to decode version_constraint: \"" << sin << "\" Bad lower limit operator in: \"" << element << "\"\n";
         clear();
         return;
      }

      os << __FILE__ << ":" << __LINE__
         << " Failed to decode version_constraint: \"" << sin << "\" Too many elements in: \"" << element << "\"\n";
      clear();
      return;
   }


}


void version_constraint::print(std::ostream& os) const noexcept {

   if(m_constraints.empty()) {
      os << "unconstrained";
      return;
   }

   for(size_t i=0; i < m_constraints.size(); ++i) {
      if(i)
         os << " | ";
      const auto& a = m_constraints[i];
      switch(a.inclusivity) {
         case bounds_inclusivity::none:  os << ">" << a.lower_bound << ", < " << a.upper_bound; break;
         case bounds_inclusivity::lower: os << ">=" << a.lower_bound << ", <" << a.upper_bound; break;
         case bounds_inclusivity::upper: os << ">" << a.lower_bound << ", <=" << a.upper_bound; break;
         case bounds_inclusivity::both:  os << ">=" << a.lower_bound << ", <=" << a.upper_bound; break;
         case bounds_inclusivity::unique:  os << a.lower_bound; break;
      };
   }
}


std::string_view version_constraint::raw() const noexcept {
   return m_raw;
}


bool version_constraint::test(const version& ver) const noexcept {
   if(m_constraints.empty())
      return true;

   for(const auto& a : m_constraints) {
      switch(a.inclusivity) {
         case bounds_inclusivity::none:
            if(a.lower_bound < ver && ver < a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::lower:
            if(a.lower_bound <= ver && ver < a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::upper:
            if(a.lower_bound < ver && ver <= a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::both:
            if(a.lower_bound <= ver && ver <= a.upper_bound)
               return true;
            break;
         case bounds_inclusivity::unique:
            if(a.lower_bound == ver)
               return true;
            break;
      }
   }
   return false;
}



} // namespace project
} // namespace antler
