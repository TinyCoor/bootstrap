//
//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include <fmt/format.h>
#include "module.h"
#include "cast.h"
#include "label.h"
#include "alias.h"
#include "import.h"
#include "comment.h"
#include "directive.h"
#include "attribute.h"
#include "if_element.h"
#include "expression.h"
#include "identifier.h"
#include "statement.h"
#include "common/bytes.h"
#include "element_types.h"
#include "common/defer.h"
#include "argument_list.h"
#include "boolean_literal.h"
#include "string_literal.h"
#include "float_literal.h"
#include "integer_literal.h"
#include "return_element.h"
#include "unary_operator.h"
#include "symbol_element.h"
#include "procedure_call.h"
#include "binary_operator.h"
#include "namespace_element.h"
#include "procedure_instance.h"
#include "identifier_reference.h"
#include "module_reference.h"
#include "types/type.h"
#include "types/any_type.h"
#include "types/bool_type.h"
#include "types/tuple_type.h"
#include "types/type_info.h"
#include "types/string_type.h"
#include "types/numeric_type.h"
#include "types/unknown_type.h"
#include "types/pointer_type.h"
#include "types/module_type.h"
#include "types/array_type.h"
#include "types/procedure_type.h"
#include "types/namespace_type.h"

namespace gfx::compiler {

program::program(class terp* terp ,assembler *assembler)
	: element(nullptr, element_type_t::program), assembler_(assembler), terp_(terp)
    , builder_(this) , ast_evaluator_(&builder_, this)
{
}

program::~program() = default;

bool program::compile(result& r, compiler::session& session)
{
    block_ = push_new_block();
    block_->parent_element(this);
    top_level_stack_.push(block_);
    initialize_core_types(r);
    for (const auto &source_file : session.source_files()) {
        auto module = compile_module(r, session, source_file);
        if (module == nullptr) {
            return false;
        }
    }
    // process directives
    auto directives = elements().find_by_type(element_type_t::directive);
    for (auto directive : directives) {
        auto directive_element = dynamic_cast<compiler::directive*>(directive);
        if (!directive_element->execute(r, session, this)) {
            return false;
        }
    }

    if (!resolve_unknown_identifiers(r)) {
        return false;
    }

    if (!resolve_unknown_types(r))  {
        return false;
    }

    if (!type_check(r, session)) {
        return false;
    }

    if (!r.is_failed()) {
        auto& listing = assembler_->listing();
        listing.add_source_file("top_level.basm");
        listing.select_source_file("top_level.basm");

        emit_context_t context(terp_, assembler_, this);
        emit(r, context);
        context.assembler->apply_addresses(r);
        context.assembler->resolve_labels(r);
        if (context.assembler->assemble(r)) {
            context.terp->run(r);
        }
    }
    top_level_stack_.pop();
    return !r.is_failed();
}

module *program::compile_module(result& r, compiler::session& session, source_file *source)
{
    auto is_root = session.current_source_file() == nullptr;
    session.push_source_file(source);
    defer({session.pop_source_file();});
    session.raise_phase(session_compile_phase_t::start, source->path());
    auto module_node = session.parse(r, source->path());
    compiler::module* module = nullptr;
    if (module_node != nullptr) {
        module = dynamic_cast<compiler::module*>(ast_evaluator_.evaluate(r, session, module_node.get()));
        if (module != nullptr) {
            module->parent_element(this);
            module->is_root(is_root);
        }
    }
    if (!r.is_failed()) {
        session.raise_phase(session_compile_phase_t::success, source->path());
    } else {
        session.raise_phase(session_compile_phase_t::failed, source->path());
    }
    return module;
}

bool program::run(result &r)
{
    while (!terp_->has_exited()) {
        if (!terp_->step(r)) {
            return false;
        }
    }
    return true;
}

void program::disassemble(FILE * file)
{
    auto root_block = assembler_->root_block();
    if (root_block == nullptr) {
        return;
    }
    root_block->disassemble();
    if (file != nullptr) {
        assembler_->listing().write(file);
    }
}



block *program::push_new_block(element_type_t type)
{
	auto parent_scope = current_scope();
	auto scope_block = builder_.make_block(parent_scope, type);

	if (parent_scope != nullptr) {
        scope_block->parent_element(parent_scope);
		parent_scope->blocks().push_back(scope_block);
	}

	push_scope(scope_block);
	return scope_block;
}

block *program::pop_scope()
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	auto top = scope_stack_.top();
	scope_stack_.pop();
	return top;
}

void program::initialize_core_types(result &r)
{
	auto parent_scope = current_scope();
    auto numeric_types = numeric_type::make_types(r, parent_scope, this);

    auto string_type = builder_.make_string_type(r, parent_scope,builder_.make_block(parent_scope, element_type_t::block));
	add_type_to_scope(string_type);
    add_type_to_scope(builder_.make_bool_type(r, parent_scope));
    auto module_type =builder_.make_module_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(module_type);

    auto namespace_type =builder_.make_namespace_type(r, parent_scope);
    add_type_to_scope(namespace_type);

    auto type_info_type =builder_.make_type_info_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(type_info_type);

    auto tuple_type = builder_.make_tuple_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(tuple_type);

    auto any_type = builder_.make_any_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(any_type);
}

block *program::current_scope() const
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	return scope_stack_.top();
}

void program::push_scope(class block *block)
{
	scope_stack_.push(block);
}

compiler::identifier *program::find_identifier(const qualified_symbol_t& symbol, compiler::block* scope) const
{
    if (symbol.is_qualified()) {
        return dynamic_cast<compiler::identifier*>(walk_qualified_symbol(symbol, scope,
            [&symbol](compiler::block* scope) {
              return scope->identifiers().find(symbol.name);
            }));
	} else {
        return dynamic_cast<compiler::identifier*>(walk_parent_scopes(
            scope != nullptr ? scope : current_scope(),
            [&](compiler::block* scope) -> compiler::element* {
              auto var = scope->identifiers().find(symbol.name);
              if (var != nullptr) {
                  return var;
              }
              for (auto import : scope->imports()) {
                  auto identifier_reference = dynamic_cast<compiler::identifier_reference*>(import->expression());
                  auto qualified_symbol = identifier_reference->symbol();
                  qualified_symbol.namespaces.push_back(qualified_symbol.name);
                  qualified_symbol.name = symbol.name;
                  qualified_symbol.fully_qualified_name = make_fully_qualified_name(qualified_symbol);
                  var = find_identifier(qualified_symbol, import->module()->scope());
                  if (var != nullptr) {
                      return var;
                  }
              }
              return nullptr;
            }));
	}
}


bool program::resolve_unknown_types(result& r)
{
	auto it = identifiers_with_unknown_types_.begin();
	while (it != identifiers_with_unknown_types_.end()) {
        auto var = *it;
        if (var->type()!=nullptr && var->type()->element_type()!=element_type_t::unknown_type) {
            it = identifiers_with_unknown_types_.erase(it);
            continue;
        }

        compiler::type *identifier_type = nullptr;
        if (var->is_parent_element(element_type_t::binary_operator)) {
            auto binary_operator = dynamic_cast<compiler::binary_operator *>(var->parent_element());
            if (binary_operator->operator_type()==operator_type_t::assignment) {
                identifier_type = binary_operator->rhs()->infer_type(this);
                var->type(identifier_type);
            }
        } else {
            if (var->initializer()==nullptr) {
                auto unknown_type = dynamic_cast<compiler::unknown_type *>(var->type());
                type_find_result_t find_result {};
                find_result.type_name = unknown_type->symbol()->qualified_symbol();
                find_result.is_array = unknown_type->is_array();
                find_result.is_pointer = unknown_type->is_pointer();
                find_result.array_size = unknown_type->array_size();
                identifier_type = make_complete_type(r, find_result, var->parent_scope());
                auto type_name = unknown_type->symbol()->name();
                identifier_type = find_type(qualified_symbol_t{.name = type_name});
                if (identifier_type != nullptr) {
                    var->type(identifier_type);
                    elements_.remove(unknown_type->id());
                }
            } else {
                identifier_type = var->initializer()->expression()->infer_type(this);
                var->type(identifier_type);
            }
        }
        if (identifier_type!=nullptr) {
            var->inferred_type(true);
            it = identifiers_with_unknown_types_.erase(it);
        } else {
            ++it;
            error(r, var, "P004", fmt::format("unable to resolve type for identifier: {}", var->symbol()->name()),
                  var->symbol()->location());
        }
    }
	return identifiers_with_unknown_types_.empty();
}

terp *program::terp()
{
	return terp_;
}

void program::add_type_to_scope(compiler::type *value)
{
    current_scope()->types().add(value);
}

bool program::find_identifier_type(result& r, type_find_result_t &result, const ast_node_shared_ptr &type_node,
    compiler::block* parent_scope)
{
    if (type_node == nullptr) {
        return false;
    }
    builder_.make_qualified_symbol(result.type_name, type_node->lhs.get());
    result.array_size = 0;
    result.is_array = type_node->is_array();
    result.is_spread = type_node->is_spread();
    result.is_pointer = type_node->is_pointer();
    make_complete_type(r, result, parent_scope);
    return result.type !=nullptr;
}

unknown_type *program::unknown_type_from_result(result &r,compiler::block *scope,
    compiler::identifier *identifier, type_find_result_t &result)
{
    auto symbol = builder_.make_symbol(scope, result.type_name.name, result.type_name.namespaces);
    auto type = builder_.make_unknown_type(r, scope, symbol, result.is_pointer, result.is_array, result.array_size);
    identifiers_with_unknown_types_.push_back(identifier);
    return type;
}

bool program::within_procedure_scope(compiler::block* parent_scope) const
{
    auto block_scope = parent_scope == nullptr ? current_scope() : parent_scope;
    while (block_scope != nullptr) {
        if (block_scope->element_type() == element_type_t::proc_type_block
            ||  block_scope->element_type() == element_type_t::proc_instance_block) {
            return true;
        }

        block_scope = block_scope->parent_scope();
    }
    return false;
}

bool program::visit_blocks(result &r, const block_visitor_callable &callable,
    compiler::block* root_block)
{
    std::function<bool (compiler::block*)> recursive_execute =
        [&](compiler::block* scope) -> bool {
          if (!callable(scope)) {
              return false;
          }
          for (auto &block : scope->blocks()) {
              if (!recursive_execute(block)) {
                  return false;
              }
          }
          return true;
        };
    return recursive_execute(root_block != nullptr ? root_block : top_level_stack_.top());
}


bool program::on_emit(result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->make_basic_block();
    instruction_block->jump_direct("_initializer");

    std::map<section_t, element_list_t> vars_by_section {};
    vars_by_section.insert(std::make_pair(section_t::bss,     element_list_t()));
    auto ro   = vars_by_section.insert(std::make_pair(section_t::ro_data, element_list_t()));
    auto data = vars_by_section.insert(std::make_pair(section_t::data,    element_list_t()));
    vars_by_section.insert(std::make_pair(section_t::text,    element_list_t()));

    auto identifiers = elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        auto var_type = var->type();
        if (var_type == nullptr || var_type->element_type() == element_type_t::namespace_type ) {
            continue;
        }

        if (within_procedure_scope(var->parent_scope()) || var->is_parent_element(element_type_t::field)) {
            continue;
        }

        switch (var->type()->element_type()) {
            case element_type_t::bool_type:
            case element_type_t::numeric_type: {
                if (var->is_constant()) {
                    auto& list = ro.first->second;
                    list.emplace_back(var);
                } else {
                    auto& list = data.first->second;
                    list.emplace_back(var);
                }
                break;
            }
            case element_type_t::any_type:
            case element_type_t::array_type:
            case element_type_t::tuple_type:
            case element_type_t::string_type:
            case element_type_t::composite_type: {
                if (var->initializer() != nullptr) {
                    if (var->is_constant()) {
                        auto& list = ro.first->second;
                        list.emplace_back(var);
                    } else {
                        auto& list = data.first->second;
                        list.emplace_back(var);
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    auto& ro_list = ro.first->second;

    for (const auto& it : interned_string_literals_) {
        compiler::string_literal* str = it.second.front();
        if (!str->is_parent_element(element_type_t::argument_list)) {
            continue;
        }
        ro_list.emplace_back(str);
    }
    std::vector<variable_t*> literals {};
    for (const auto& section : vars_by_section) {
        instruction_block->section(section.first);
        instruction_block->current_entry()->blank_lines(1);
        for (auto e : section.second) {
            switch (e->element_type()) {
                case element_type_t::string_literal: {
                    auto string_literal = dynamic_cast<compiler::string_literal*>(e);
                    instruction_block->memo();
                    instruction_block->align(4);
                    auto it = interned_string_literals_.find(string_literal->value());
                    if (it != interned_string_literals_.end()) {
                        auto current_entry = instruction_block->current_entry();
                        string_literal_list_t& str_list = it->second;
                        for (auto str : str_list) {
                            auto var_label = instruction_block->make_label(str->label_name());
                            current_entry->label(var_label);
                            auto var = context.allocate_variable(r, var_label->name(),
                                context.program->find_type({.name = "string"}), identifier_usage_t::heap);
                            if (var != nullptr) {
                                var->address_offset = 4;
                                literals.emplace_back(var);
                            }
                        }
                        current_entry->blank_lines(1);
                    }
                    instruction_block->current_entry()->comment(fmt::format("\"{}\"", string_literal->value()),
                        context.indent);
                    instruction_block->string(string_literal->escaped_value());
                    break;
                }
                case element_type_t::identifier: {
                    auto var = dynamic_cast<compiler::identifier*>(e);
                    auto init = var->initializer();
                    instruction_block->memo();
                    instruction_block->current_entry()->blank_lines(1);
                    auto type_alignment = static_cast<uint8_t>(var->type()->alignment());
                    if (type_alignment > 1) {
                        instruction_block->align(type_alignment);
                    }

                    auto var_label = instruction_block->make_label(var->symbol()->name());
                    instruction_block->current_entry()->label(var_label);
                    context.allocate_variable(r, var_label->name(), var->type(),
                        identifier_usage_t::heap, nullptr);
                    switch (var->type()->element_type()) {
                        case element_type_t::bool_type: {
                            bool value = false;
                            var->as_bool(value);
                            if (init == nullptr) {
                                instruction_block->reserve_byte(1);
                            } else {
                                instruction_block->bytes({static_cast<uint8_t>(value ? 1 : 0)});
                            }
                            break;
                        }
                        case element_type_t::numeric_type: {
                            uint64_t value = 0;
                            if (var->type()->number_class() == type_number_class_t::integer) {
                                var->as_integer(value);
                            } else {
                                double temp = 0;
                                if (var->as_float(temp)) {
                                    register_value_alias_t alias {};
                                    alias.qwf = temp;
                                    value = alias.qw;
                                }
                            }
                            auto symbol_type = integer_symbol_type_for_size(var->type()->size_in_bytes());
                            switch (symbol_type) {
                                case symbol_type_t::u8:
                                    if (init == nullptr) {
                                        instruction_block->reserve_byte(1);
                                    } else {
                                        instruction_block->bytes({static_cast<uint8_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::u16:
                                    if (init == nullptr) {
                                        instruction_block->reserve_word(1);
                                    } else {
                                        instruction_block->words({static_cast<uint16_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::f32:
                                case symbol_type_t::u32:
                                    if (init == nullptr) {
                                        instruction_block->reserve_dword(1);
                                    } else {
                                        instruction_block->dwords({static_cast<uint32_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::f64:
                                case symbol_type_t::u64:
                                    if (init == nullptr) {
                                        instruction_block->reserve_qword(1);
                                    } else {
                                        instruction_block->qwords({value});
                                    }
                                    break;
                                case symbol_type_t::bytes:
                                default:
                                    break;
                            }
                            break;
                        }
                        case element_type_t::string_type: {
                            if (init != nullptr) {
                                auto string_literal = dynamic_cast<compiler::string_literal*>(init->expression());
                                instruction_block->current_entry()->comment(fmt::format("\"{}\"", string_literal->value()),
                                    context.indent);
                                instruction_block->string(string_literal->value());
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    context.assembler->push_block(instruction_block);

    auto procedure_types = elements().find_by_type(element_type_t::proc_type);
    procedure_type_list_t proc_list {};
    for (auto p : procedure_types) {
        auto procedure_type = dynamic_cast<compiler::procedure_type*>(p);
        if (procedure_type->parent_scope()->element_type() == element_type_t::proc_instance_block) {
            proc_list.emplace_back(procedure_type);
        }
    }

    for (auto p : procedure_types) {
        auto procedure_type = dynamic_cast<compiler::procedure_type*>(p);
        if (procedure_type->parent_scope()->element_type() != element_type_t::proc_instance_block) {
            proc_list.emplace_back(procedure_type);
        }
    }

    for (auto procedure_type : proc_list) {
        procedure_type->emit(r, context);
    }

    auto top_level_block = context.assembler->make_basic_block();
    top_level_block->align(instruction_t::alignment);
    top_level_block->current_entry()->blank_lines(1);
    top_level_block->memo();
    top_level_block->current_entry()->label(top_level_block->make_label("_initializer"));
//    for (auto var : literals) {
//        var->init(context.assembler, top_level_block);
//    }

    block_list_t implicit_blocks {};
    auto module_blocks = elements().find_by_type(element_type_t::module_block);
    for (auto block : module_blocks) {
        implicit_blocks.emplace_back(dynamic_cast<compiler::block*>(block));
    }

    context.assembler->push_block(top_level_block);
    for (auto block : implicit_blocks) {
        block->emit(r, context);
    }

    auto finalizer_block = context.assembler->make_basic_block();
    finalizer_block->align(instruction_t::alignment);
    finalizer_block->current_entry()->blank_lines(1);
    finalizer_block->exit();
    finalizer_block->current_entry()->label(finalizer_block->make_label("_finalizer"));

    context.assembler->pop_block();
    context.assembler->pop_block();

    return true;
}

compiler::type *program::find_type(const qualified_symbol_t &symbol, compiler::block* scope) const
{
    if (symbol.is_qualified()) {
        return dynamic_cast<compiler::type*>(walk_qualified_symbol(symbol,
            const_cast<compiler::program*>(this)->current_top_level(),
            [&](compiler::block* scope) -> compiler::element* {
              auto matching_type = scope->types().find(symbol.name);
              if (matching_type != nullptr) {
                  return matching_type;
              }
              auto type_identifier = find_identifier(symbol, scope);
              if (type_identifier != nullptr) {
                  return type_identifier->type();
              }
              return nullptr;
            }));
    } else {
        return dynamic_cast<compiler::type*>(walk_parent_scopes(scope != nullptr ? scope : current_scope(),
            [&](compiler::block* scope) -> compiler::element* {
              auto type = scope->types().find(symbol.name);
              if (type != nullptr) {
                  return type;
              }
              auto type_identifier = find_identifier(symbol, scope);
              if (type_identifier != nullptr) {
                  return type_identifier->type();
              }
              return nullptr;
            }));
    }
}

bool program::resolve_unknown_identifiers(result &r)
{
    auto it = unresolved_identifier_references_.begin();
    while (it != unresolved_identifier_references_.end()) {
        auto unresolved_reference = *it;
        if (unresolved_reference->resolved()) {
            it = unresolved_identifier_references_.erase(it);
            continue;
        }

        auto identifier = find_identifier(unresolved_reference->symbol(),
            unresolved_reference->parent_scope());
        if (identifier == nullptr) {
            ++it;
            error(r, unresolved_reference, "P004",  fmt::format("unable to resolve identifier: {}", unresolved_reference->symbol().name),
                  unresolved_reference->symbol().location);
            continue;
        }
        unresolved_reference->identifier(identifier);
        it = unresolved_identifier_references_.erase(it);
    }

    return unresolved_identifier_references_.empty();
}

compiler::block *program::block() const
{
    return block_;
}

compiler::block *program::current_top_level()
{
    if (top_level_stack_.empty()) {
        return nullptr;
    }
    return top_level_stack_.top();
}

compiler::module *program::find_module(compiler::element *element)
{
    return dynamic_cast<compiler::module*>(walk_parent_elements(element,
        [](compiler::element* each) -> compiler::element* {
          if (each->element_type() == element_type_t::module)
              return each;
          return nullptr;
        }));
}

void program::error(result &r, compiler::session &session, const std::string &code,
                    const std::string &message, const source_location &location)
{
    auto source_file = session.current_source_file();
    if (source_file != nullptr) {
        source_file->error(r, code, message, location);
    }
}

void program::error(result &r, compiler::element *element, const std::string &code,
                    const std::string &message, const source_location &location)
{
    auto module = find_module(element);
    if (module != nullptr) {
        module->source_file()->error(r, code, message, location);
    }
}

element *program::walk_parent_scopes(compiler::block *scope, const program::scope_visitor_callable &callable)
{
    while (scope != nullptr) {
        auto* result = callable(scope);
        if (result != nullptr) {
            return result;
        }
        scope = scope->parent_scope();
    }
    return nullptr;
}

element *program::walk_parent_elements(compiler::element *element,
    const program::element_visitor_callable &callable)
{
    auto current = element;
    while (current != nullptr) {
        auto* result = callable(current);
        if (result != nullptr) {
            return result;
        }
        current = current->parent_element();
    }
    return nullptr;
}

element *program::walk_qualified_symbol(const qualified_symbol_t &symbol, compiler::block *scope,
                                        const program::namespace_visitor_callable &callable) const
{
    auto non_const_this = const_cast<compiler::program*>(this);
    auto block_scope = scope != nullptr ? scope : non_const_this->current_top_level();
    for (const auto& namespace_name : symbol.namespaces) {
        auto var = block_scope->identifiers().find(namespace_name);
        if (var == nullptr || var->initializer() == nullptr)
            return nullptr;
        auto expr = var->initializer()->expression();
        if (expr->element_type() == element_type_t::namespace_e) {
            auto ns = dynamic_cast<namespace_element*>(expr);
            block_scope = dynamic_cast<compiler::block*>(ns->expression());
        } else if (expr->element_type() == element_type_t::module_reference) {
            auto module_reference = dynamic_cast<compiler::module_reference*>(expr);
            block_scope = module_reference->module()->scope();
        } else {
            return nullptr;
        }
    }
    return callable(block_scope);
}

compiler::type *program::find_pointer_type(compiler::type *base_type, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::pointer_type::name_for_pointer(base_type)
    }, scope);
}

compiler::type *program::find_array_type(compiler::type *entry_type, size_t size, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::array_type::name_for_array(entry_type, size)
    }, scope);
}

compiler::type *program::make_complete_type(result &r, type_find_result_t &result, compiler::block *parent_scope)
{
    result.type = find_type(result.type_name, parent_scope);
    if (result.type != nullptr) {
        if (result.is_array) {
            auto array_type = find_array_type(result.type, result.array_size, parent_scope);
            if (array_type == nullptr) {
                array_type =builder_.make_array_type(r,parent_scope, builder_.make_block(parent_scope, element_type_t::block),
                    result.type, result.array_size);
            }
            result.type = array_type;
        }

        if (result.is_pointer) {
            auto pointer_type = find_pointer_type(result.type, parent_scope);
            if (pointer_type == nullptr) {
                pointer_type = builder_.make_pointer_type(r, parent_scope, result.type);
            }
            result.type = pointer_type;
        }
    }
    return result.type;
}

element_builder &program::builder()
{
    return builder_;
}

element_map &program::elements()
{
    return elements_;
}

bool program::type_check(result &r, session &session)
{
    auto identifiers = elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        auto init = var->initializer();
        if (init == nullptr) {
            continue;
        }
        auto rhs_type = init->infer_type(this);
        if (!var->type()->type_check(rhs_type)) {
            error(r, init, "C051",
                fmt::format("type mismatch: cannot assign {} to {}.",
                    rhs_type->symbol()->name(),var->type()->symbol()->name()),
                var->location());
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
        auto rhs_type = binary_op->rhs()->infer_type(this);
        if (!var->type()->type_check(rhs_type)) {
            error(r, binary_op, "C051", fmt::format( "type mismatch: cannot assign {} to {}.",
                    rhs_type->symbol()->name(), var->type()->symbol()->name()), binary_op->rhs()->location());
        }
    }
    return !r.is_failed();
}

}