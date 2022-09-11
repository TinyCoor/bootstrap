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
        : terp_(options.heap_size, options.stack_size),
          assembler_(&terp_), program_(&terp_, &assembler_),
          options_(options)
{
    for (const auto &path : source_files) {
        add_source_file(path);
    }
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
    if (options_.verbose) {
        program_.disassemble(stdout);
        if (!options_.dom_graph_file.empty())
            write_code_dom_graph(options_.dom_graph_file);
    }
}

void session::write_code_dom_graph(const fs::path& path)
{
    FILE* output_file = stdout;
    if (!path.empty()) {
        output_file = fopen(path.string().c_str(), "wt");
    }

    compiler::code_dom_formatter formatter(&program_, output_file);
    formatter.format(fmt::format("Code DOM Graph: {}", path.string()));
    fclose(output_file);
}

ast_node_shared_ptr session::parse(result& r, const fs::path& path)
{
    auto source_file = find_source_file(path);
    if (source_file == nullptr) {
        source_file = add_source_file(path);
    }
    return parse(r, source_file);
}

ast_node_shared_ptr session::parse(result &r, source_file *source)
{
    if (source->empty()) {
        if (!source->load(r)) {
            return nullptr;
        }
    }
    parser alpha_parser(source);
    auto module_node = alpha_parser.parse(r);
    if (module_node != nullptr && !r.is_failed()) {
        if (options_.verbose && !options_.ast_graph_file.empty()) {
            gfx::parser::write_ast_graph(options_.ast_graph_file, module_node);
        }
    }
    return module_node;
}


const session_options_t& session::options() const
{
    return options_;
}

bool session::initialize(result &r)
{
    terp_.initialize(r);
    assembler_.initialize(r);
    return !r.is_failed();
}

assembler &session::assembler()
{
    return assembler_;
}

compiler::program &session::program()
{
    return program_;
}

bool session::compile(result &r)
{
    return program_.compile(r, *this);
}

class terp &session::terp()
{
    return terp_;
}

source_file* session::add_source_file(const fs::path &path)
{
    auto it = source_files_.insert(std::make_pair(path.string(), source_file(path)));
    if (!it.second) {
        return nullptr;
    }
    return &it.first->second;
}

std::vector<source_file *> session::source_files()
{
    std::vector<source_file *> list {};
    for (auto &it : source_files_) {
        list.push_back(&it.second);
    }
    return list;
}

source_file *session::find_source_file(const fs::path &path)
{
    auto res = source_files_.find(path.string());
    if (res == source_files_.end()) {
        return nullptr;
    }
    return &res->second;
}
source_file *session::current_source_file()
{
    if (source_file_stack_.empty()) {
        return nullptr;
    }
    return source_file_stack_.top();
}

void session::push_source_file(source_file *source)
{
    source_file_stack_.push(source);
}

source_file *session::pop_source_file()
{
    if (source_file_stack_.empty()) {
        return nullptr;
    }
    auto source = source_file_stack_.top();
    source_file_stack_.pop();
    return source;
}

}