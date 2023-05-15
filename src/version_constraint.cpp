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

static inline std::string_view consume(std::string_view s, std::size_t sz) {
   return {s.data() + sz, s.size() - sz};
}

constexpr uint16_t max_component = std::numeric_limits<uint16_t>::max();

inline const version min_version{};
inline const version max_version{max_component, max_component, max_component};

/// Trim whitespace from the front and back of string.
/// @param s  The string to trim
/// @return  An empty string or a string that starts with non-whitespace and ends with non-whitespace
inline std::string_view trim(std::string_view s) {
   // Find an iterator to the firs non-space char; if we went to the end, return an empty string.
   auto first = std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); });
   if (first == s.end())
      return {};
   // Find the last non space character.
   auto last = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); });
   // Convert the reverse iter last to the forward iterator containing end using base().
   //    See: https://en.cppreference.com/w/cpp/iterator/reverse_iterator/base
   return std::string_view{first, static_cast<std::string_view::size_type>(last.base() - first)};
}


} // anonymous namespace



//--- constructors/destructor ------------------------------------------------------------------------------------------

bool version_constraint::is_unique() const noexcept {
   return m_constraints.size() == 1 && m_constraints[0].lower_bound.rel == relation::eq;
}

std::string_view consume_ws(std::string_view s) {
   std::size_t i = 0;
   for (; i < s.size(); ++i) {
      if (!std::isspace(s[i]))
         break;
   }
   return consume(s, i);
}

std::pair<version, std::string_view> version_constraint::parse_version(std::string_view s) {
   version v;

   std::string_view vs       = consume_ws(s);
   int64_t          consumed = v.parse(vs);
   return std::make_pair(v, consume(vs, consumed));
}

// simple recursive descent parser for version constraint
bool version_constraint::parse(std::string_view vc) {
   std::size_t sanity    = 0;
   std::size_t orig_size = vc.size();

   if (vc.empty())
      return true;

   m_constraints.emplace_back();
   bound* current_bound = &m_constraints.back().lower_bound;

   try {
      while (vc.size() > 0) {

         ANTLER_CHECK(sanity < orig_size, "internal failure of version constraint parsing, called with {0}", vc);

         vc = consume_ws(vc);

         char c = vc[0];

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
            auto [ver, rest]   = parse_version(vc);
            current_bound->rel = relation::eq;
            current_bound->ver = ver;
            vc                 = rest;
         } break;
         case '<': {
            if (vc.size() > 1 && vc[1] == '=') {
               current_bound->rel = relation::le;
               auto [ver, rest]   = parse_version(consume(vc, 2));
               current_bound->ver = ver;
               vc                 = rest;
            } else {
               current_bound->rel = relation::lt;
               auto [ver, rest]   = parse_version(consume(vc, 1));
               current_bound->ver = ver;
               vc                 = rest;
            }
         } break;
         case '>': {
            if (vc.size() > 1 && vc[1] == '=') {
               current_bound->rel = relation::ge;
               auto [ver, rest]   = parse_version(consume(vc, 2));
               current_bound->ver = ver;
               vc                 = rest;
            } else {
               current_bound->rel = relation::gt;
               auto [ver, rest]   = parse_version(consume(vc, 1));
               current_bound->ver = ver;
               vc                 = rest;
            }
         } break;
         case ';':
         case '|': {
            m_constraints.emplace_back();
            current_bound = &m_constraints.back().lower_bound;
            vc            = consume(vc, 1);
         } break;
         case ',': {
            m_constraints.back().upper_bound = bound();
            current_bound                    = &(*m_constraints.back().upper_bound);
            vc                               = consume(vc, 1);
         } break;
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

   for (auto itr = m_constraints.begin(); itr != m_constraints.end(); ++itr) {
      result += itr->to_string();
      if (itr == m_constraints.end() - 1) {
         result += " | ";
      }
   }

   return result;
}

bool version_constraint::test(const version& ver) const noexcept {
   if (m_constraints.empty())
      return true;

   const auto& test_bound = [&](auto& b) {
      switch (b.rel) {
      case relation::lt:
         return ver < b.ver;
      case relation::le:
         return ver <= b.ver;
      case relation::gt:
         return ver > b.ver;
      case relation::ge:
         return ver >= b.ver;
      case relation::eq:
         return ver == b.ver;
      default:
         return false;
      }
   };

   for (const auto& cons : m_constraints) {
      bool lb = test_bound(cons.lower_bound);
      bool ub = true;
      if (cons.upper_bound)
         ub = test_bound(*cons.upper_bound);
      if (lb && ub) {
         return true;
      }
   }
   return false;
}


} // namespace antler::project
