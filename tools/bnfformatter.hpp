#pragma once

#include "CLI11.hpp"

class BnfFormatter : public CLI::Formatter {
  public:
    std::string make_option_opts(const CLI::Option* opt) const override;
};

std::string BnfFormatter::make_option_opts(const CLI::Option* opt) const {
    std::stringstream out;

    if(!opt->get_option_text().empty()) {
        out << " " << opt->get_option_text();
    } else {
        if(opt->get_type_size() != 0) {
            if(!opt->get_type_name().empty()) {
                out << " " << (opt->get_required() ? "<" : "[");
                out << get_label(opt->get_type_name());
                out << (opt->get_required() ? ">" : "]");
            }
            if(!opt->get_default_str().empty())
                out << ", default: " << opt->get_default_str();
            if(opt->get_expected_max() == CLI::detail::expected_max_vector_size)
                out << " ...";
            else if(opt->get_expected_min() > 1)
                out << " x " << opt->get_expected();
        }
        if(!opt->get_envname().empty())
            out << " (" << get_label("Env") << ":" << opt->get_envname() << ")";
        if(!opt->get_needs().empty()) {
            out << " " << get_label("Needs") << ":";
            for(const auto* op : opt->get_needs())
                out << " " << op->get_name();
        }
        if(!opt->get_excludes().empty()) {
            out << " " << get_label("Excludes") << ":";
            for(const auto* op : opt->get_excludes())
                out << " " << op->get_name();
        }
    }
    return out.str();
}
