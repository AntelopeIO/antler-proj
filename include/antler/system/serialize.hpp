#pragma once


namespace YAML {
template <typename Tag>
struct convert<antler::project::object<Tag>> {
   static Node encode(const antler::project::object<Tag>& o) {
      Node n;
      // TODO we will need to readdress when adding support for tests
      n["name"] = o.name();
      n["lang"] = o.language();
      if (!o.compile_options().empty())
         n["compile_options"] = o.compile_options();
      if (!o.link_options().empty())
         n["link_options"] = o.link_options();
      return n;
   }

   static bool decode(const YAML::Node& n, antler::project::object<Tag>& v) {
      v = {n["version"].as<std::string>()};
      return true;
   }
};
} // namespace YAML