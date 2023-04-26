/// @copyright See `LICENSE` in the root directory of this project.

#include <antler/project/object.hpp>

#include <algorithm> // find_if()
#include <magic_enum.hpp>


namespace antler::project {

//--- constructors/detractor --------------------------------------------------------------------------------------------

object::object(type_t ot)
   : m_type{ ot } {}


object::object(type_t ot, std::string_view name, const std::string& lang, std::string_view copts, std::string_view lopts)
   : m_type{ ot }
   , m_name{ name }
   , m_language{ system::to_upper(lang) }
   , m_comp_options{ system::split<';'>(copts) }
   , m_link_options{ system::split<';'>(lopts) }
{}


object::object(std::string_view name, std::string_view command)
   : m_type{ type_t::test }
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
   // If possible, find a dependency with matching name and return it.
   auto i = std::find_if(m_dependencies.begin(), m_dependencies.end(), [dep](const antler::project::dependency& d) { return d.name() == dep.name(); } );
   if( i == m_dependencies.end() )
      m_dependencies.emplace_back(dep);
   else
      *i = dep;
}

} // namespace antler::project