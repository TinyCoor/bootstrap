//
// Created by 12132 on 2022/8/21.
//

#include <fstream>
#include "session.h"
#include <fmt/format.h>
#include <common/defer.h>
#include <parser/parser.h>
#include "elements/program.h"
#include "elements/module.h"
#include "code_dom_formatter.h"
namespace gfx::compiler {
session::session(const session_options_t& options, const path_list_t& source_files)
    : terp_(options.heap_size, options.stack_size), builder_(*this),
      ast_evaluator_(*this), assembler_(&terp_), options_(options)
{
    for (const auto &path : source_files) {
        if (path.is_relative()) {
            add_source_file(absolute(path));
        } else {
            add_source_file(path);
        }
    }
}

session::~session() = default;

void session::raise_phase(session_compile_phase_t phase, const fs::path& source_file) const
{
    if (options_.compile_callback == nullptr) {
        return;
    }
    options_.compile_callback(phase, source_file);
}

void session::finalize()
{
    if (options_.verbose) {
        try  {
            program_.disassemble(*this, stdout);
            if (!options_.dom_graph_file.empty()) {
                write_code_dom_graph(options_.dom_graph_file);
            }
        } catch (const fmt::format_error& e) {
            fmt::print("fmt::format_error caught: {}\n", e.what());
        }
    }
}

bool session::run()
{
    while (!terp_.has_exited()) {
        if (!terp_.step(r)) {
            return false;
        }
    }
    return true;
}

void session::write_code_dom_graph(const fs::path& path)
{
    FILE* output_file = stdout;
    if (!path.empty()) {
        output_file = fopen(path.string().c_str(), "wt");
    }
    defer({ if (output_file !=nullptr) {
            fclose(output_file);
    };});

    compiler::code_dom_formatter formatter(*this, output_file);
    formatter.format(fmt::format("Code DOM Graph: {}", path.string()));
}

ast_node_shared_ptr session::parse(const fs::path& path)
{
    auto source_file = find_source_file(path);
    if (source_file == nullptr) {
        source_file = add_source_file(path);
    }
    return parse(source_file);
}

ast_node_shared_ptr session::parse(source_file *source)
{
    raise_phase(session_compile_phase_t::start, source->path());
    defer({
          if (r.is_failed()) {
              raise_phase(session_compile_phase_t::failed, source->path());
          } else {
              raise_phase(session_compile_phase_t::success, source->path());
          }
    });
    if (source->empty()) {
        if (!source->load(r)) {
            return nullptr;
        }
    }
    parser alpha_parser(source);
    auto module_node = alpha_parser.parse(r);
    if (module_node != nullptr && !r.is_failed()) {
        if (options_.output_ast_graph) {
            std::filesystem::path ast_file_path(source->path().parent_path());
            auto file_name = source->path().filename().replace_extension("").string();
            file_name+= "-ast";
            ast_file_path.append(file_name);
            ast_file_path.replace_extension(".dot");
            gfx::parser::write_ast_graph(ast_file_path, module_node);
        }
    }
    return module_node;
}


const session_options_t& session::options() const
{
    return options_;
}

bool session::initialize()
{
    terp_.initialize(r);
    assembler_.initialize(r);
    return !r.is_failed();
}

assembler &session::assembler()
{
    return assembler_;
}

const compiler::program &session::program() const
{
    return program_;
}

bool session::compile()
{
    return program_.compile(*this);
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

gfx::result &session::result()
{
    return r;
}
void session::error(const std::string &code, const std::string &message, const source_location &location)
{
    auto source_file = current_source_file();
    if (source_file == nullptr) {
        return;
    }
    source_file->error(r, code, message, location);

}
void session::error(compiler::element *element, const std::string &code, const std::string &message,
                    const source_location &location)
{
    element->module()->source_file() ->error(r, code, message, location);
}
emit_context_t &session::emit_context()
{
    return context_;
}

compiler::program &session::program()
{
    return program_;
}

const element_map &session::elements() const
{
    return elements_;
}
element_builder &session::builder()
{
    return builder_;
}

element_map &session::elements()
{
    return elements_;
}

ast_evaluator &session::evaluator()
{
    return ast_evaluator_;
}

}