//
// Created by 12132 on 2022/4/23.
//

#include "directive.h"
#include "program.h"
#include "string_literal.h"
#include "attribute.h"
#include "procedure_type.h"
#include <fmt/format.h>

namespace gfx::compiler {
directive::directive(block* parent, const std::string& name, element* expression)
	: element(parent, element_type_t::directive),name_(name),expression_(expression)
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

bool directive::execute(result &r, compiler::program *program)
{
	auto it = s_directive_handlers.find(name_);
	if (it == s_directive_handlers.end()) {
		return true;
	}
	return it->second(this, r, program);
}

bool directive::on_run(result &r, compiler::program *program)
{
	return false;
}
bool directive::on_load(result &r, compiler::program *program)
{
	return false;
}
bool directive::on_foreign(result &r, compiler::program *program)
{
	auto terp = program->terp();

	// XXX: this sucks; it must be fixed!
	// "/Users/jeff/src/basecode-lang/bootstrap/build/debug/bin/bootstrap");
	auto library = terp->load_shared_library(r, "bootstrap");
	if (library == nullptr) {
		return false;
	}
	terp->dump_shared_libraries();

	auto ffi_identifier = dynamic_cast<compiler::identifier*>(expression_);
	auto proc_type = dynamic_cast<compiler::procedure_type*>(ffi_identifier->initializer()->expression());

	std::string symbol_name = ffi_identifier->name();
	auto alias_attribute = proc_type->attributes().find("alias");
	if (alias_attribute != nullptr) {
		symbol_name = dynamic_cast<compiler::string_literal*>(alias_attribute->expression())->value();
	}

	function_signature_t signature {
		.symbol = symbol_name,
		.library = library,
	};

	auto result = terp->register_foreign_function(r, signature);
	if (!result) {
		r.add_message(
			"P004",
			fmt::format("unable to find foreign function symbol: {}", symbol_name),
			true);
	}

	return result;
}

}