//
// Created by 12132 on 2022/3/30.
//

#include "alpha_compiler.h"
namespace gfx {
alpha_compiler::alpha_compiler(size_t heap_size)
	: 	terp_(heap_size),
		evaluator_(this)
{

}

alpha_compiler::~alpha_compiler() {
}

bool alpha_compiler::initialize(result& r) {
	return terp_.initialize(r);
}

bool alpha_compiler::compile(result& r, const parser_input_t& input) {
	compile_stream(r, input);
	return !r.is_failed();
}

bool alpha_compiler::compile_stream(result& r, const parser_input_t& input) {
	auto program_node = parser_.parse(input);
	auto parser_result = parser_.result();
	if (parser_result.is_failed()) {
		for (const auto& msg : parser_result.messages())
			r.add_message(msg.code(), msg.message(), msg.details(), msg.is_error());
		return false;
	}

	if (program_node == nullptr)
		return !r.is_failed();

	if (!evaluator_.evaluate_program(r, program_node)) {

	}

	return !r.is_failed();
}

}