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
#include "elements/cast.h"
#include "elements/label.h"
#include "elements/transmute.h"
#include "elements/types/type.h"
#include "elements/import.h"
#include "elements/comment.h"
#include "elements/types/any_type.h"
#include "elements/types/bool_type.h"
#include "elements/types/tuple_type.h"
#include "elements/types/array_type.h"
#include "elements/if_element.h"
#include "elements/expression.h"
#include "elements/attribute.h"
#include "elements/identifier.h"
#include "elements/directive.h"
#include "elements/statement.h"
#include "elements/types/type_info.h"
#include "elements/types/string_type.h"
#include "elements/types/numeric_type.h"
#include "elements/types/procedure_type.h"
#include "elements/types/unknown_type.h"
#include "elements/types/pointer_type.h"
#include "elements/types/module_type.h"
#include "elements/argument_list.h"
#include "elements/boolean_literal.h"
#include "elements/string_literal.h"
#include "elements/float_literal.h"
#include "elements/integer_literal.h"
#include "elements/return_element.h"
#include "elements/unary_operator.h"
#include "elements/symbol_element.h"
#include "elements/types/composite_type.h"
#include "elements/procedure_call.h"
#include "elements/binary_operator.h"
#include "elements/namespace_element.h"
#include "elements/types/namespace_type.h"
#include "elements/procedure_instance.h"
#include "elements/identifier_reference.h"
#include "elements/module_reference.h"

namespace gfx::compiler {
session::session(const session_options_t& options, const path_list_t& source_files)
    : terp_(options.heap_size, options.stack_size), builder_(*this),
      ast_evaluator_(*this),  assembler_(&terp_), scope_manager_(*this), options_(options)
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
            disassemble(stdout);
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
bool session::resolve_unknown_identifiers()
{
    auto& unresolved = scope_manager().unresolved_identifier_references();
    auto it = unresolved.begin();
    while (it != unresolved.end()) {
        auto unresolved_reference = *it;
        if (unresolved_reference->resolved()) {
            it = unresolved.erase(it);
            continue;
        }

        auto identifier = scope_manager().find_identifier(unresolved_reference->symbol(),
                                                                  unresolved_reference->parent_scope());
        if (identifier == nullptr) {
            ++it;
            error(unresolved_reference, "P004",  fmt::format("unable to resolve identifier: {}", unresolved_reference->symbol().name),
                 unresolved_reference->symbol().location);
            continue;
        }
        unresolved_reference->identifier(identifier);
        it = unresolved.erase(it);
    }

    return unresolved.empty();
}

bool session::type_check()
{
    auto identifiers = elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        auto init = var->initializer();
        if (init == nullptr) {
            continue;
        }
        type_inference_result_t rhs_type;
        init->infer_type(*this, rhs_type);
        if (!var->type()->type_check(rhs_type.type)) {
            error(init, "C051", fmt::format("type mismatch: cannot assign {} to {}.",
                 rhs_type.name(), var->type()->symbol()->name()), var->location());
        }
    }

    auto binary_ops = elements().find_by_type(element_type_t::binary_operator);
    for (auto op : binary_ops) {
        auto binary_op = dynamic_cast<compiler::binary_operator*>(op);
        if (binary_op->operator_type() != operator_type_t::assignment) {
            continue;
        }
        // XXX: revisit this for destructuring/multiple assignment
        auto var = dynamic_cast<compiler::identifier*>(binary_op->lhs());
        type_inference_result_t rhs_type;
        binary_op->rhs()->infer_type(*this, rhs_type);
        if (!var->type()->type_check(rhs_type.type)) {
            error(binary_op, "C051", fmt::format( "type mismatch: cannot assign {} to {}.",
                rhs_type.name(), var->type()->symbol()->name()), binary_op->rhs()->location());
        }
    }
    return !r.is_failed();
}

bool session::compile()
{
    auto& top_level_stack = scope_manager_.top_level_stack();
    program_.block(scope_manager_.push_new_block());
    program_.block()->parent_element(&program_);
    top_level_stack.push(program_.block());
    initialize_core_types();
    initialize_built_in_procedures();
    for (const auto &source_file : source_files()) {
        auto module = compile_module(source_file);
        if (module == nullptr) {
            return false;
        }
    }
    // process directives
    auto directives = elements().find_by_type(element_type_t::directive);
    for (auto directive : directives) {
        auto directive_element = dynamic_cast<compiler::directive*>(directive);
        if (!directive_element->execute(*this)) {
            error(directive_element, "P044",
                fmt::format("directive failed to execute: {}", directive_element->name()),
                directive->location());
            return false;
        }
    }

    if (!resolve_unknown_identifiers()) {
        return false;
    }

    if (!resolve_unknown_types())  {
        return false;
    }

    if (!type_check()) {
        return false;
    }

    if (!r.is_failed()) {
        auto& listing = assembler().listing();
        listing.add_source_file("top_level.basm");
        listing.select_source_file("top_level.basm");

        program_. emit(*this);
        assembler().apply_addresses(r);
        assembler().resolve_labels(r);
        if (assembler().assemble(r)) {
            run();
        }
    }
    top_level_stack.pop();
    return !r.is_failed();
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
compiler::scope_manager &session::scope_manager()
{
    return scope_manager_;
}

const compiler::scope_manager &session::scope_manager() const
{
    return scope_manager_;
}

void session::disassemble(FILE * file)
{
    auto root_block = assembler_.root_block();
    if (root_block == nullptr) {
        return;
    }
    root_block->disassemble();
    if (file != nullptr) {
        assembler_.listing().write(file);
    }
}

module *session::compile_module(source_file *source)
{
    auto is_root = current_source_file() == nullptr;
    push_source_file(source);
    defer({
        pop_source_file();
    });
    auto module_node = parse(source->path());
    compiler::module* module = nullptr;
    if (module_node != nullptr) {
        module = dynamic_cast<compiler::module*>(evaluator().evaluate(module_node.get()));
        if (module != nullptr) {
            module->parent_element(&program_);
            module->is_root(is_root);
        }
    }
    return module;
}

void session::initialize_core_types()
{
    auto parent_scope = scope_manager_.current_scope();
    auto numeric_types = numeric_type::make_types(*this, parent_scope);

    auto string_type = builder_.make_string_type(parent_scope,builder_.make_block(parent_scope, element_type_t::block));
    scope_manager_.add_type_to_scope(string_type);
    scope_manager_.add_type_to_scope(builder_.make_bool_type(parent_scope));
    auto module_type =builder_.make_module_type(parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    scope_manager_.add_type_to_scope(module_type);

    auto namespace_type = builder_.make_namespace_type(parent_scope);
    scope_manager_.add_type_to_scope(namespace_type);

    auto type_info_type = builder_.make_type_info_type(parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    scope_manager_.add_type_to_scope(type_info_type);

    auto tuple_type = builder_.make_tuple_type(parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    scope_manager_.add_type_to_scope(tuple_type);

    auto any_type = builder_.make_any_type(parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    scope_manager_.add_type_to_scope(any_type);
}

bool session::resolve_unknown_types()
{
    auto& identifiers = scope_manager().identifiers_with_unknown_types();
    auto it = identifiers.begin();
    while (it != identifiers.end()) {
        auto var = *it;
        if (var->type()!=nullptr && var->type()->element_type()!=element_type_t::unknown_type) {
            it = identifiers.erase(it);
            continue;
        }

        compiler::type *identifier_type = nullptr;
        type_inference_result_t inference_result;
        if (var->is_parent_element(element_type_t::binary_operator)) {
            auto binary_operator = dynamic_cast<compiler::binary_operator *>(var->parent_element());
            if (binary_operator->operator_type() == operator_type_t::assignment) {
                binary_operator->rhs()->infer_type(*this, inference_result);
                identifier_type = inference_result.type;
                var->type(identifier_type);
            }
        } else {
            if (var->initializer() == nullptr) {
                auto unknown_type = dynamic_cast<compiler::unknown_type *>(var->type());
                type_find_result_t find_result {};
                find_result.type_name = unknown_type->symbol()->qualified_symbol();
                find_result.is_array = unknown_type->is_array();
                find_result.is_pointer = unknown_type->is_pointer();
                find_result.array_size = unknown_type->array_size();
                identifier_type = scope_manager().make_complete_type(*this, find_result, var->parent_scope());
                if (identifier_type != nullptr) {
                    var->type(identifier_type);
                    elements().remove(unknown_type->id());
                }
            } else {
                var->initializer()->expression()->infer_type(*this, inference_result);
                identifier_type = inference_result.type;
                var->type(identifier_type);
            }
        }
        if (identifier_type!=nullptr) {
            var->inferred_type(true);
            it = identifiers.erase(it);
        } else {
            ++it;
            error(var, "P004", fmt::format("unable to resolve type for identifier: {}", var->symbol()->name()),
                var->symbol()->location());
        }
    }
    return identifiers.empty();
}

void session::initialize_built_in_procedures()
{
   // auto parent_scope = scope_manager_.current_scope();
}
}