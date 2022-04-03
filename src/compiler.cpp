//
// Created by 12132 on 2022/3/30.
//

#include "compiler.h"
#include "lexer.h"
#include <fmt/format.h>
namespace gfx {
compiler::compiler(size_t heap_size)
	: 	terp_(heap_size)
{

}

compiler::~compiler() {
}

bool compiler::initialize(result& r) {
	return terp_.initialize(r);
}

bool compiler::compile(result& r, std::istream& input) {
	compile_stream(r, input);
	return !r.is_failed();
}

bool compiler::compile_stream(result& r, std::istream& input) {
	lexer alpha_lexer(input);
	token_t token;
	while (alpha_lexer.next(token)) {
		fmt::print("token.type = {}\n", token.name());
		fmt::print("token.value = {}\n", token.value);
		if (token.is_numeric())
			fmt::print("token.radix = {}\n", token.radix);
		fmt::print("token.line = {}\n", token.line);
		fmt::print("token.column = {}\n\n", token.column);
	}
	return true;
}


}