//
// Created by 12132 on 2022/4/14.
//

#include "bytecode_emitter.h"
#include "parser/ast_formatter.h"
#include "compiler/elements/program.h"
#include "code_dom_formatter.h"

#include <fmt/format.h>
namespace gfx {
bytecode_emitter::bytecode_emitter( const bytecode_emitter_options_t& options)
	: terp_(options.heap_size, options.stack_size), options_(options)
{
}

bytecode_emitter::~bytecode_emitter()
{
}

bool bytecode_emitter::compile_files(result& r, const std::vector<std::filesystem::path>& source_files)
{
	for (const auto& source_file : source_files) {
		fmt::print("Compile: {} ... ", source_file.string());
		if (!std::filesystem::exists(source_file)) {
			fmt::print("FAILED, file does not exist.\n");
			continue;
		}
		std::ifstream input_stream(source_file);
		if (input_stream.is_open()) {
			if (!compile(r, input_stream)) {
				fmt::print("FAILED.\n");
			} else {
				fmt::print("PASSED.\n");
			}
			input_stream.close();
		}
	}
	return !r.is_failed();
}

bool bytecode_emitter::initialize(result& r)
{
	return terp_.initialize(r);
}

bool bytecode_emitter::compile(result& r, std::istream& input)
{
	compile_stream(r, input);
	return !r.is_failed();
}

bool bytecode_emitter::compile_stream(result& r, std::istream& input)
{
	parser alpha_parser(input);
	auto program_node = alpha_parser.parse(r);
	if (program_node != nullptr && !r.is_failed()) {
		if (options_.verbose) {
			auto close_required = false;
			FILE* ast_output_file = stdout;
			if (!options_.ast_graph_file_name.empty()) {
				auto file_name  = options_.ast_graph_file_name.string();
				ast_output_file = fopen(file_name.c_str(), "wt");
				close_required = true;
			}
			ast_formatter formatter(program_node, ast_output_file);
			formatter.format(fmt::format("AST Graph: {}", options_.ast_graph_file_name.string()));

			if (close_required) {
				fclose(ast_output_file);
			}
		}

		compiler::program program {};
		if (program.initialize(r, program_node)) {
			if (options_.verbose) {
				auto close_required = false;
				FILE* code_dom_output_file = stdout;
				if (!options_.code_dom_graph_file_name.empty()) {
					code_dom_output_file = fopen(options_.code_dom_graph_file_name.string().c_str(), "wt");
					close_required = true;
				}

				compiler::code_dom_formatter formatter(&program, code_dom_output_file);
				formatter.format(fmt::format("Code DOM Graph: {}", options_.code_dom_graph_file_name.string()));

				if (close_required) {
					fclose(code_dom_output_file);
				}
			}

		}
	}
	return !r.is_failed();
}

}