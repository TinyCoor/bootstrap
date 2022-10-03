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
#include <configure.h>

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

bool directive::evaluate(result& r, compiler::session& session, compiler::program* program)
{
	auto it = s_evaluate_handlers.find(name_);
	if (it == s_evaluate_handlers.end()) {
		return true;
	}
	return it->second(this, r, session, program);
}

bool directive::execute(result &r, compiler::session& session, compiler::program *program)
{
	auto it = s_execute_handlers.find(name_);
	if (it == s_execute_handlers.end()) {
		return true;
	}
	return it->second(this, r, session, program);
}

bool directive::on_execute_run(result &r, compiler::session& session, compiler::program *program)
{
	return true;
}

bool directive::on_evaluate_run(result &r, compiler::session& session, compiler::program *program)
{
	return true;
}

bool directive::on_evaluate_foreign(result &r, compiler::session& session, compiler::program *program)
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

bool directive::on_execute_foreign(result &r, compiler::session& session, compiler::program *program)
{
	auto terp = program->terp();
	std::string library_name;
	auto library_attribute = attributes().find("library");
	if (library_attribute != nullptr) {
        if (!library_attribute->as_string(library_name)) {
            program->error(r, this, "P004", "unable to convert library name.", location());
            return false;
        }
	}

    if (library_name.empty()) {
        program->error(r, this, "P005","library attribute required for foreign directive.",
            location());
        return false;
    }

    auto platform_name = fmt::format("{}{}{}", SHARED_LIBRARY_PREFIX, library_name, SHARED_LIBRARY_SUFFIX);
    std::filesystem::path library_path(platform_name);
	auto library = terp->load_shared_library(r, library_path);
	if (library == nullptr) {
        // XXX: revisit this at some point
        auto msg = r.find_code("B062");
        if (msg != nullptr) {
            program->error(r, this, "P006", msg->message(), location());
            r.remove_code("B062");
        }
		return false;
	}

	auto ffi_identifier = dynamic_cast<compiler::identifier*>(expression_);
	std::string symbol_name = ffi_identifier->symbol()->name();
	auto alias_attribute = attributes().find("alias");
	if (alias_attribute != nullptr) {
        if (!alias_attribute->as_string(symbol_name)) {
            program->error(r, this, "P004", "unable to convert alias attribute's name.", location());
            return false;
        }
	}

	function_signature_t signature {
		.symbol = symbol_name,
		.library = library,
	};

    auto proc_identifier = dynamic_cast<compiler::identifier*>(expression_);
    auto proc_type = proc_identifier->initializer()->procedure_type();
    if (proc_type != nullptr) {
        for (auto param : proc_type->parameters().as_list()) {
            // XXX: need to figure out how to best handle this
            if (param->identifier()->type()->element_type() == element_type_t::any_type) {
                continue;
            }
            function_value_t value;
            value.name = param->identifier()->symbol()->name();
            value.type = ffi_types_t::pointer_type;
            signature.arguments.push_back(value);
        }

        if (proc_type->returns().size() == 0) {
            signature.return_value.type = ffi_types_t::void_type;
        }

        // XXX: this is a hack
        if (proc_type->is_foreign()) {
            signature.calling_mode = ffi_calling_mode_t::c_ellipsis;
        }
    }

    auto result = terp->register_foreign_function(r, signature);
	if (!result) {
		program->error(r, this, "P004", fmt::format("unable to find foreign function symbol: {}", symbol_name),
                       location());
        return false;
    } else {
        if (proc_type != nullptr) {
            proc_type->foreign_address(reinterpret_cast<uint64_t>(signature.func_ptr));
        }
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