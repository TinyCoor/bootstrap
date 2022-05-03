//
// Created by 12132 on 2022/4/14.
//

#ifndef BYTECODE_EMITTER_H_
#define BYTECODE_EMITTER_H_
#include <string>
#include <cstdint>
#include <vm/terp.h>
#include <filesystem>
#include <parser/parser.h>
#include "scope.h"
namespace gfx {
struct bytecode_emitter_options_t {
	bool verbose = false;
	size_t heap_size = 0;
	size_t stack_size = 0;
	std::filesystem::path ast_graph_file_name{};
};

class bytecode_emitter {
public:
	explicit bytecode_emitter(const bytecode_emitter_options_t &options);

	virtual ~bytecode_emitter();

	bool compile_files(result &r, const std::vector<std::filesystem::path> &source_files);

	bool initialize(result &r);

	bool compile(result &r, std::istream &input);

	bool compile_stream(result &r, std::istream &input);

private:
	terp terp_;
	bytecode_emitter_options_t options_{};
};

}
#endif // BYTECODE_EMITTER_H_
