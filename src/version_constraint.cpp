/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/version_constraint.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <regex>

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

static inline std::string_view consume(std::string_view s, std::size_t sz) { return {s.data() + sz, s.size() - sz}; }

constexpr uint16_t max_component = std::numeric_limits<uint16_t>::max();

static inline const version min_version{};
static inline const version max_version{max_component, max_component, max_component};

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
   return std::string_view{first, static_cast<std::string_view::size_type>(last.base() - first)};
}


} // anonymous namespace



//--- constructors/destructor ------------------------------------------------------------------------------------------

bool version_constraint::is_unique() const noexcept {
   return m_constraints.size() == 1 && m_constraints[0].lower_bound.rel == relation::eq;
}


std::pair<relation, std::string_view> version_constraint::parse_inequality(std::string_view vc) {
   using sv = std::string_view;

   ANTLER_CHECK(vc.size() >= 2, "invalid version_constraint: {0} expecting = or a number", vc);

   if (vc[0] == '>') {
      if (vc.size() > 1 && vc[1] == '=')
         return std::make_pair(relation::ge, consume(vc, 2));
      else
         return std::make_pair(relation::gt, consume(vc, 1));
   } else if (vc[0] == '<') {
      if (vc.size() > 1 && vc[1] == '=')
         return std::make_pair(relation::le, consume(vc, 2));
      else
         return std::make_pair(relation::lt, consume(vc, 1));
   }

   __builtin_unreachable();
}

std::pair<version, std::string_view> version_constraint::parse_version(std::string_view s) {
   version v;

   int64_t consumed = v.parse(s);
   return std::make_pair(v, consume(s, consumed));
}

// simple recursive descent parser for version constraint
bool version_constraint::parse(std::string_view vc) {
   std::size_t sanity = 0;
   std::size_t orig_size = vc.size();

   m_constraints.emplace_back();
   bound* current_bound = &m_constraints.back().lower_bound;
   
   try {
      while (vc.size() > 0) {

         ANTLER_CHECK(sanity < orig_size, "internal failure of version constraint parsing, called with {0}", vc);

         char c = vc[0];
         if (std::isspace(c)) {
            consume(vc, 1);
            continue;
         }

         switch (c) {
            case 'v':
            case 'V':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                  auto [ver, rest] = parse_version(vc);
                  current_bound->rel = relation::eq;
                  current_bound->ver = ver;
                  vc = rest;
               }
               break;
            case '<':
            case '>': {
                  auto [rel, next] = parse_inequality(vc);
                  auto [ver, rest] = parse_version(next);
                  current_bound->rel = rel;
                  current_bound->ver = ver;
                  vc = rest;
               }
               break;
            case ';':
            case '|': {
                  m_constraints.emplace_back();
                  current_bound = &m_constraints.back().lower_bound;
                  consume(vc, 1);
               }
               break;
            case ',':
               m_constraints.back().upper_bound = bound{};
               current_bound = &(*m_constraints.back().upper_bound);
            default:
               system::error_log("invalid character {0} in version constraint : {1}", c, vc);
               clear();
               return false;
         }

         sanity++;
      }
   } catch (...) {
      clear();
      return false;
   }
   return true;
}

std::string version_constraint::to_string() const noexcept {
   std::string result;

   for (auto itr=m_constraints.begin(); itr != m_constraints.end(); ++itr) {
      result += itr->to_string();
      if (itr == m_constraints.end() - 1) {
         result += " | ";
      }
   }
   //for (const auto& vc : m_constraints) {
   //   result += vc.to_string();
   //   if (vc.upper_bound) {
   //      result += ", ";
   //      result += vc.upper_bound->to_string();
   //   }
   //}

   return result;
}

//void version_constraint::parse2(std::string_view sin) {
//
//   // Trim whitespace from both ends and set the raw value. Then clear constraints in preparation for further parsing.
//   m_raw = trim(sin);
//   m_constraints.clear();
//
//   // But return if s is now empty.
//   if (m_raw.empty())
//      return;
//
//   // Start by splitting on '|'
//   std::vector<std::string> splits = system::split<'|', ';'>(m_raw);
//   for (auto split : splits) {
//      // Now split on ','
//      std::vector<std::string> element = system::split<','>(split);
//      if (element.size() == 1) {
//         // If there's only one constraint, we need to decide if it's an upper bound, a lower bound, or unique.
//         auto trimmed_el = trim(element[0]);
//         std::vector<std::string> el_list = system::split<' '>(trimmed_el);
//
//         if (el_list.size() == 1) {
//            // One member MUST be a unique.
//            auto ver_str = std::string(trim(el_list[0]));
//
//            if(std::isdigit(ver_str[0])) {
//               m_constraints.emplace_back(constraint{ version(ver_str), version(), bounds_inclusivity::unique });
//               continue;
//            }
//
//            // first char must have been an operation, right?
//            auto first_digit = ver_str.find_first_of("0123456789");
//            if(first_digit != ver_str.npos) {
//               el_list.push_back( ver_str.substr(first_digit) );
//               el_list[0] = ver_str.substr(0,first_digit);
//            }
//         }
//
//         if (el_list.size() == 2) {
//            // Two members is a bound.
//            auto op_str = trim(el_list[0]);
//            auto ver_str = trim(el_list[1]);
//
//            if (op_str == "<")
//               m_constraints.emplace_back(constraint{ min_version, version(ver_str), bounds_inclusivity::lower }); // inclusive of the min!
//            else if (op_str == "<=")
//               m_constraints.emplace_back(constraint{ min_version, version(ver_str), bounds_inclusivity::both });
//            else if (op_str == ">")
//               m_constraints.emplace_back(constraint{ version(ver_str), max_version, bounds_inclusivity::upper }); // inclusive of the max!
//            else if (op_str == ">=")
//               m_constraints.emplace_back(constraint{ version(ver_str), max_version, bounds_inclusivity::both });
//            else {
//               system::error_log("Failed to decode version constraint: {0}", sin);
//               for (const auto& el : el_list) {
//                  system::error_log("Bad op: {0}", el);
//               }
//               clear();
//               return;
//            }
//
//            continue;
//         }
//
//         system::error_log("Failed to decode version constraint: {0}", sin);
//         for (const auto& el : el_list) {
//            system::error_log("Too many or too few elements in: {0}", el);
//         }
//
//         clear();
//         return;
//      }
//
//      if (element.size() == 2) {
//         std::vector<std::string> lower_list = system::split<' '>(trim(element[0]));
//         std::vector<std::string> upper_list = system::split<' '>(trim(element[1]));
//
//         if (lower_list.size() != 2 || upper_list.size() != 2) {
//            system::error_log("Failed to decode version constraint: {0}", sin);
//            for (const auto& el : el_list) {
//               system::error_log("Too many or too few elements in: {0}", el);
//            }
//
//            clear();
//            return;
//         }
//
//         auto lop = trim(lower_list[0]);
//         auto lver = trim(lower_list[1]);
//         auto uop = trim(upper_list[0]);
//         auto uver = trim(upper_list[1]);
//         if (lop == ">") {
//            if (uop == "<") {
//               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::none });
//               continue;
//            }
//            if (uop == "<=") {
//               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::upper });
//               continue;
//            }
//
//            system::error_log("Failed to decode version constraint: {0}", sin);
//
//            for (const auto& e : element) {
//               system::error_log("Bad upper bound operator in: {0}", e);
//            }
//            clear();
//            return;
//         }
//
//         if (lop == ">=") {
//            if (uop == "<") {
//               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::lower });
//               continue;
//            }
//            if (uop == "<=") {
//               m_constraints.emplace_back(constraint{ version(lver), version(uver), bounds_inclusivity::both });
//               continue;
//            }
//
//            system::error_log("Failed to decode version constraint: {0}", sin);
//
//            for (const auto& e : element) {
//               system::error_log("Bad upper bound operator in: {0}", e);
//            }
//
//            clear();
//            return;
//         }
//
//         os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Bad lower limit operator in: \"";
//         for (const auto& e : element) {
//            os << e;
//         }
//         os << std::endl;
//         clear();
//         return;
//      }
//
//      os << __FILE__ << ":" << __LINE__ << " Failed to decode version_constraint: \"" << sin << "\" Too many elements in: \"";
//
//      for (const auto& e : element) {
//         os << e;
//      }
//      os << std::endl;
//      clear();
//      return;
//   }
//}


void version_constraint::print(std::ostream& os) const noexcept {

   if (m_constraints.empty()) {
      os << "unconstrained";
      return;
   }

   //for (size_t i = 0; i < m_constraints.size(); ++i) {
   //   if (i)
   //      os << " | ";
   //   const auto& a = m_constraints[i];
   //   switch (a.inclusivity) {
   //      case bounds_inclusivity::none:   os << ">" << a.lower_bound << ", < " << a.upper_bound; break;
   //      case bounds_inclusivity::lower:  os << ">=" << a.lower_bound << ", <" << a.upper_bound; break;
   //      case bounds_inclusivity::upper:  os << ">" << a.lower_bound << ", <=" << a.upper_bound; break;
   //      case bounds_inclusivity::both:   os << ">=" << a.lower_bound << ", <=" << a.upper_bound; break;
   //      case bounds_inclusivity::unique: os << a.lower_bound; break;
   //   };
   //}
}


bool version_constraint::test(const version& ver) const noexcept {
   if (m_constraints.empty())
      return true;

   //for (const auto& a : m_constraints) {
   //   switch (a.inclusivity) {
   //      case bounds_inclusivity::none:
   //         if (a.lower_bound < ver && ver < a.upper_bound)
   //            return true;
   //         break;
   //      case bounds_inclusivity::lower:
   //         if (a.lower_bound <= ver && ver < a.upper_bound)
   //            return true;
   //         break;
   //      case bounds_inclusivity::upper:
   //         if (a.lower_bound < ver && ver <= a.upper_bound)
   //            return true;
   //         break;
   //      case bounds_inclusivity::both:
   //         if (a.lower_bound <= ver && ver <= a.upper_bound)
   //            return true;
   //         break;
   //      case bounds_inclusivity::unique:
   //         if (a.lower_bound == ver)
   //            return true;
   //         break;
   //   }
   //}
   return false;
}


} // namespace antler::project
