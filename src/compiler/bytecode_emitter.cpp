//
// Created by 12132 on 2022/4/14.
//

#include "bytecode_emitter.h"
#include "constant_expression_evaluator.h"
#include "parser/ast_formatter.h"

#include <fmt/format.h>
namespace gfx{
bytecode_emitter::bytecode_emitter(
	const bytecode_emitter_options_t& options): terp_(options.heap_size, options.stack_size),
												global_scope_(nullptr, nullptr),
												options_(options) {
}

bytecode_emitter::~bytecode_emitter() {
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

void bytecode_emitter::build_scope_tree(result& r, scope* scope, const ast_node_shared_ptr& node) {
	if (scope == nullptr || node == nullptr) {
		return;
	}

	for (auto& child_node : node->children) {
		auto child_scope = scope->add_child_scope(child_node);
		build_scope_tree(r, child_scope, child_node);
	}
}

bool bytecode_emitter::initialize(result& r) {
	return terp_.initialize(r);
}

bool bytecode_emitter::compile(result& r, std::istream& input) {
	compile_stream(r, input);
	return !r.is_failed();
}

bool bytecode_emitter::compile_stream(result& r, std::istream& input) {
	parser alpha_parser(input);
	auto program_node = alpha_parser.parse(r);
	if (program_node != nullptr && !r.is_failed()) {
		build_scope_tree(r, &global_scope_, program_node);
		apply_constant_folding(r, program_node);
		if (options_.verbose) {
			auto close_required = false;
			FILE* ast_output_file = stdout;
			if (!options_.ast_graph_file_name.empty()) {
				auto file_name  = options_.ast_graph_file_name.string();
				ast_output_file = fopen(file_name.c_str(), "wt");
				close_required = true;
			}
			ast_formatter formatter(program_node, ast_output_file);
			formatter.format_graph_viz();

			if (close_required){
				fclose(ast_output_file);
			}
		}
	}
	return !r.is_failed();
}

void bytecode_emitter::apply_constant_folding(result &r, const ast_node_shared_ptr &node)
{
	constant_expression_evaluator evaluator(&global_scope_);
	auto result_node = evaluator.fold_literal_expression(r, node);
}
}