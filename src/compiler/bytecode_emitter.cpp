//
// Created by 12132 on 2022/4/14.
//

#include "bytecode_emitter.h"
#include "code_dom_formatter.h"
#include "parser/ast_formatter.h"
#include "elements/program.h"
#include <fmt/format.h>
namespace gfx::compiler {
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
	auto module_node = alpha_parser.parse(r);
	if (module_node != nullptr && !r.is_failed()) {
		if (options_.verbose) {
			alpha_parser.write_ast_graph(options_.ast_graph_file_name, module_node);
		}

		compiler::program program_element(&terp_);
		if (program_element.compile(r, module_node)) {
			if (options_.verbose) {
				write_code_dom_graph(options_.code_dom_graph_file_name, &program_element);
			}

		}
	}
	return !r.is_failed();
}

void bytecode_emitter::write_code_dom_graph(const std::filesystem::path &path, const program *program)
{
	auto close_required = false;
	FILE* code_dom_output_file = stdout;
	if (!path.empty()) {
		code_dom_output_file = fopen(path.string().c_str(), "wt");
		close_required = true;
	}

	code_dom_formatter formatter(program, code_dom_output_file);
	formatter.format(fmt::format("Code DOM Graph: {}", path.string()));

	if (close_required) {
		fclose(code_dom_output_file);
	}

}

}