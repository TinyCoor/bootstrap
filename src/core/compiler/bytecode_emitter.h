//
// Created by 12132 on 2022/4/14.
//

#ifndef BYTECODE_EMITTER_H_
#define BYTECODE_EMITTER_H_
#include <string>
#include <cstdint>
#include "vm/terp.h"
#include <filesystem>
#include "parser/parser.h"
#include <vm/assembly_listing.h>
namespace gfx::compiler {
class program;
struct bytecode_emitter_options_t {
	bool verbose = false;
	size_t heap_size = 0;
	size_t stack_size = 0;
	std::filesystem::path ast_graph_file_name{};
	std::filesystem::path code_dom_graph_file_name {};
};

class bytecode_emitter {
public:
	explicit bytecode_emitter(const bytecode_emitter_options_t &options);

	virtual ~bytecode_emitter();

    bool initialize(result &r);

	bool compile_files(result &r, assembly_listing& listing, const std::vector<std::filesystem::path> &source_files);

	bool compile(result &r,assembly_listing& listing, std::istream &input);

	bool compile_stream(result &r, assembly_listing& listing, std::istream &input);

private:
	void write_code_dom_graph(const std::filesystem::path& path, const program* program);
private:
	terp terp_;
	bytecode_emitter_options_t options_{};
};

}
#endif // BYTECODE_EMITTER_H_
