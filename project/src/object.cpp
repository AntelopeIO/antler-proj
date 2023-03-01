/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/object.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm> // find_if()
#include <magic_enum.hpp>


namespace antler::project {

//--- constructors/detractor --------------------------------------------------------------------------------------------

object::object(type_t ot)
   : m_type{ ot } {}


object::object(type_t ot, std::string_view name, const std::string& lang, std::string_view opts)
   : m_type{ ot }
   , m_name{ name }
   , m_language{ boost::algorithm::to_upper_copy(lang) }
   , m_options{ opts }
{
}


object::object(std::string_view name, std::string_view command)
   : m_type{ object::test }
   , m_name{ name }
   , m_command{ command }
{
}


//--- alphabetic --------------------------------------------------------------------------------------------------------


std::string_view object::command() const noexcept {
   return m_command;
}


void object::command(std::string_view s) noexcept {
   m_command = s;
}


const antler::project::dependency::list_t& object::dependencies() const noexcept {
   return m_dependencies;
}


std::optional<antler::project::dependency> object::dependency(std::string_view name) {
   auto rv = std::find_if(m_dependencies.begin(), m_dependencies.end(), [name](const auto& d) { return d.name() == name; });
   if (rv != m_dependencies.end())
      return *rv;

   return std::optional<antler::project::dependency>{};
}


bool object::dependency_exists(std::string_view name) const noexcept {
   return std::find_if(m_dependencies.begin(), m_dependencies.end(), [name](const auto& d) { return d.name() == name; }) != m_dependencies.end();
}


std::string_view object::language() const noexcept {
   return m_language;
}

void object::language(std::string_view lang) noexcept {
   m_language = lang;
}


std::string_view object::name() const noexcept {
   return m_name;
}


void object::name(std::string_view s) noexcept {
   m_name = s;
}


std::string_view object::options() const noexcept {
   return m_options;
}


void object::options(std::string_view options) noexcept {
   m_options = options;
}


bool object::remove_dependency(std::string_view name) noexcept {
   // If possible, find a dependency with matching name and return it.
   auto i = std::find_if(m_dependencies.begin(), m_dependencies.end(), [name](const antler::project::dependency& d) { return d.name() == name; } );
   if( i == m_dependencies.end() )
      return false;
   m_dependencies.erase(i);
   return true;
}


object::type_t object::type() const noexcept {
   return m_type;
}


void object::upsert_dependency(antler::project::dependency&& dep) noexcept {
   // If possible, find a dependency with matching name and reutrn it.
   auto i = std::find_if(m_dependencies.begin(), m_dependencies.end(), [dep](const antler::project::dependency& d) { return d.name() == dep.name(); } );
   if( i == m_dependencies.end() )
      m_dependencies.emplace_back(dep);
   else
      *i = dep;
}


} // namespace antler::project


//--- global operators ------------------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& os, const antler::project::object::type_t& e) {
   os << magic_enum::enum_name(e);
   return os;
}


std::istream& operator>>(std::istream& is, antler::project::object::type_t& e) {

   std::string temp;
   if (is >> temp) {
      // Try string as is.
      auto opt = magic_enum::enum_cast<antler::project::object::type_t>(temp);
      if (opt.has_value()) {
         e = opt.value();
         return is;
      }
   }

   // This might be an exceptional state and so maybe we should throw an exception?
   e = antler::project::object::type_t::none;
   return is;
}
