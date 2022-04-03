//
// Created by 12132 on 2022/3/30.
//

#include "alpha_compiler.h"
#include "lexer.h"
#include <fmt/format.h>
namespace gfx {
alpha_compiler::alpha_compiler(size_t heap_size)
	: 	terp_(heap_size)
{

}

alpha_compiler::~alpha_compiler() {
}

bool alpha_compiler::initialize(result& r) {
	return terp_.initialize(r);
}

bool alpha_compiler::compile(result& r, std::istream& input) {
	compile_stream(r, input);
	return !r.is_failed();
}

bool alpha_compiler::compile_stream(result& r,std::istream& input) {
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