//
// Created by 12132 on 2022/8/21.
//

#include <fstream>
#include "session.h"
#include <fmt/format.h>
#include <common/defer.h>
#include <parser/parser.h>
#include "elements/program.h"
#include "code_dom_formatter.h"
namespace gfx::compiler {
session::session(const session_options_t& options, const path_list_t& source_files)
        : source_files_(source_files), options_(options)
{
}

session::~session() = default;

void session::raise_phase(session_compile_phase_t phase, const fs::path& source_file)
{
    if (options_.compile_callback == nullptr) {
        return;
    }
    options_.compile_callback(phase, source_file);
}

void session::finalize()
{
    listing_.write(stdout);
}

void session::write_code_dom_graph(compiler::program* program, const fs::path& path)
{
    FILE* output_file = stdout;
    if (!path.empty()) {
        output_file = fopen(path.string().c_str(), "wt");
//        defer({fclose(output_file);});
    }

    compiler::code_dom_formatter formatter(program, output_file);
    formatter.format(fmt::format("Code DOM Graph: {}", path.string()));
    fclose(output_file);
}

ast_node_shared_ptr session::parse(result& r, const fs::path& source_file)
{
    std::ifstream input_stream(source_file);
    if (input_stream.is_open()) {
        parser alpha_parser(input_stream);
        auto module_node = alpha_parser.parse(r);
        if (module_node != nullptr && !r.is_failed()) {
            if (options_.verbose && !options_.ast_graph_file.empty()) {
                alpha_parser.write_ast_graph(options_.ast_graph_file, module_node);
            }
        }
        return module_node;
    } else {
        r.add_message("S001", fmt::format("unable to open source file: {}", source_file.string()), true);
    }
    return nullptr;
}

assembly_listing& session::listing()
{
    return listing_;
}

const path_list_t& session::source_files() const
{
    return source_files_;
}

const session_options_t& session::options() const
{
    return options_;
}

void session::post_processing(compiler::program* program)
{
    if (options_.verbose && !options_.dom_graph_file.empty()) {
        write_code_dom_graph(program, options_.dom_graph_file);
        program->disassemble(listing_);
    }
}

}