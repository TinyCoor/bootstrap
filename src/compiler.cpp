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
	parser alpha_parser(input);
	auto program_node = alpha_parser.parse(r);
	if (program_node != nullptr) {

	}
	return !r.is_failed();
}


}