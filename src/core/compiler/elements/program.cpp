//
//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include <fmt/format.h>
#include "core/compiler/session.h"
#include "core/compiler/compiler_types.h"
#include "core/compiler/elements/initializer.h"
#include "core/compiler/elements/string_literal.h"
#include "core/compiler/elements/symbol_element.h"
#include "core/compiler/elements/types/procedure_type.h"


namespace gfx::compiler {

program::program()
	: element(nullptr, nullptr, element_type_t::program)
{
}

program::~program() = default;

bool program::on_emit(compiler::session &session)
{
    auto& assembler = session.assembler();
    auto& scope_manager = session.scope_manager();
    auto instruction_block = session.assembler().make_basic_block();
    instruction_block->jump_direct(assembler.make_label_ref("_initializer"));

    std::map<section_t, element_list_t> vars_by_section {};
    vars_by_section.insert(std::make_pair(section_t::bss,  element_list_t()));
    auto ro   = vars_by_section.insert(std::make_pair(section_t::ro_data, element_list_t()));
    auto data = vars_by_section.insert(std::make_pair(section_t::data,    element_list_t()));
    vars_by_section.insert(std::make_pair(section_t::text, element_list_t()));

    auto identifiers = session.elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        if (scope_manager.within_procedure_scope(var->parent_scope())) {
            continue;
        }
        if (var->is_parent_element(element_type_t::field)) {
            continue;
        }
        auto var_type = var->type();
        if  (var_type == nullptr) {
            return false;
        }
        if (var_type->element_type() == element_type_t::namespace_type ) {
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
                if (var->is_constant()) {
                    auto& list = ro.first->second;
                    list.emplace_back(var);
                } else {
                    auto& list = data.first->second;
                    list.emplace_back(var);
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    auto& ro_list = ro.first->second;
    auto& interned_strings = session.scope_manager().interned_string_literals();
    for (const auto& it : interned_strings) {
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
                    auto it = interned_strings.find(string_literal->value());
                    if (it != interned_strings.end()) {
                        auto current_entry = instruction_block->current_entry();
                        string_literal_list_t& str_list = it->second;
                        for (auto str : str_list) {
                            auto var_label = assembler.make_label(str->label_name());
                            current_entry->label(var_label);
                            auto var = session.emit_context().allocate_variable(var_label->name(),
                                session.scope_manager().find_type({.name = "string"}), identifier_usage_t::heap,
                                                                                nullptr);
                            if (var != nullptr) {
                                var->address_offset = 4;
                            }
                        }
                        current_entry->blank_lines(1);
                    }
                    instruction_block->current_entry()->comment(fmt::format("\"{}\"", string_literal->value()),
                        session.emit_context().indent);
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

                    auto var_label = assembler.make_label(var->symbol()->name());
                    instruction_block->current_entry()->label(var_label);
                    session.emit_context().allocate_variable(var_label->name(), var->type(),
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
                                    session.emit_context().indent);
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

    session.assembler().push_block(instruction_block);

    auto procedure_types = session.elements().find_by_type(element_type_t::proc_type);
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
        procedure_type->emit(session);
    }

    auto top_level_block = session.assembler().make_basic_block();
    top_level_block->align(instruction_t::alignment);
    top_level_block->current_entry()->blank_lines(1);
    top_level_block->memo();
    top_level_block->current_entry()->label(assembler.make_label("_initializer"));

    block_list_t implicit_blocks {};
    auto module_blocks = session.elements().find_by_type(element_type_t::module_block);
    for (auto block : module_blocks) {
        implicit_blocks.emplace_back(dynamic_cast<compiler::block*>(block));
    }

    session.assembler().push_block(top_level_block);
    for (auto block : implicit_blocks) {
        block->emit(session);
    }

    auto finalizer_block =session.assembler().make_basic_block();
    finalizer_block->align(instruction_t::alignment);
    finalizer_block->current_entry()->blank_lines(1);
    finalizer_block->exit();
    finalizer_block->current_entry()->label(assembler.make_label("_finalizer"));

    session.assembler().pop_block();
    session.assembler().pop_block();

    return true;
}

compiler::block *program::block() const
{
    return block_;
}

void program::block(compiler::block *value)
{
    block_ =  value;
}


}