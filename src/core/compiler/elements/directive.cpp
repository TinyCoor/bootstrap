//
// Created by 12132 on 2022/4/23.
//

#include "directive.h"
#include "program.h"
#include "string_literal.h"
#include "attribute.h"
#include "types/procedure_type.h"
#include "symbol_element.h"
#include "fmt/format.h"

namespace gfx::compiler {
directive::directive(block* parent, const std::string& name, element* expression)
	: element(parent, element_type_t::directive), name_(name), expression_(expression)
{
}

std::string directive::name() const
{
	return name_;
}

element *directive::expression()
{
	return expression_;
}

bool directive::evaluate(result& r, compiler::program* program)
{
	auto it = s_evaluate_handlers.find(name_);
	if (it == s_evaluate_handlers.end()) {
		return true;
	}
	return it->second(this, r, program);
}

bool directive::execute(result &r, compiler::program *program)
{
	auto it = s_execute_handlers.find(name_);
	if (it == s_execute_handlers.end()) {
		return true;
	}
	return it->second(this, r, program);
}

bool directive::on_execute_run(result &r, compiler::program *program)
{
	return false;
}

bool directive::on_evaluate_run(result &r, compiler::program *program)
{
	return false;
}

bool directive::on_execute_load(result &r, compiler::program *program)
{
	return false;
}

bool directive::on_evaluate_load(result &r, compiler::program *program)
{
	return false;
}

bool directive::on_evaluate_foreign(result &r, compiler::program *program)
{
	auto proc_identifier = dynamic_cast<compiler::identifier*>(expression_);
	auto proc_type = proc_identifier->initializer()->procedure_type();
	if (proc_type != nullptr) {
		auto attrs = proc_type->attributes().as_list();
		for (auto attr : attrs) {
			attributes().add(attr);
			proc_type->attributes().remove(attr->name());
		}
		proc_type->is_foreign(true);
		return true;
	}
	return false;
}

bool directive::on_execute_foreign(result &r, compiler::program *program)
{
	auto terp = program->terp();
	std::string library_name = "compiler-rt.dll";
	auto library_attribute = attributes().find("library");
	if (library_attribute != nullptr) {
        if (!library_attribute->as_string(library_name)) {
            r.add_message("P005",
              "unable to convert library attribute's name.", true);
            return false;
        }
	}
    std::filesystem::path library_path(library_name);
	auto library = terp->load_shared_library(r, library_path);
	if (library == nullptr) {
		return false;
	}

	auto ffi_identifier = dynamic_cast<compiler::identifier*>(expression_);
	std::string symbol_name = ffi_identifier->symbol()->name();
	auto alias_attribute = attributes().find("alias");
	if (alias_attribute != nullptr) {
        if (!alias_attribute->as_string(symbol_name)) {
            r.add_message("P006",
                "unable to convert alias attribute's name.", true);
            return false;
        }
	}

	function_signature_t signature {
		.symbol = symbol_name,
		.library = library,
	};

	auto result = terp->register_foreign_function(r, signature);
	if (!result) {
		r.add_message("P004", fmt::format("unable to find foreign function symbol: {}", symbol_name), false);
	}

	return !r.is_failed();
}

void directive::on_owned_elements(element_list_t& list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}


}