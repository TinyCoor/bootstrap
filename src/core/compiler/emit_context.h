//
// Created by 12132 on 2022/7/23.
//

#ifndef COMPILER_EMIT_CONTEXT_H_
#define COMPILER_EMIT_CONTEXT_H_
#include <any>
#include <stack>
#include <string>
#include "vm/instruction.h"

namespace gfx::compiler {
enum class emit_context_type_t {
    empty,
    if_element,
    procedure_type
};

enum class emit_access_type_t {
    read,
    write
};

struct procedure_type_data_t {
    std::string identifier_name;
};

struct if_data_t {
    enum class logical_group_t {
        no_group,
        and_group,
        or_group,

    };
    std::string true_branch_label;
    std::string false_branch_label;
    logical_group_t group_type = logical_group_t::no_group;
};

struct emit_context_t {
    template <typename T>
    T* top() {
        if (data_stack.empty()) {
            return nullptr;
        }
        try {
            return std::any_cast<T>(&data_stack.top());
        } catch (const std::bad_any_cast& e) {
            return nullptr;
        }
    }

    void push_if(const std::string& true_label_name, const std::string& false_label_name)
    {
        data_stack.push(std::any(if_data_t {
            .true_branch_label = true_label_name,
            .false_branch_label = false_label_name,
        }));
    }

    void push_procedure_type(const std::string& name)
    {
        data_stack.push(std::any(procedure_type_data_t {
            .identifier_name = name
        }));
    }

    void push_access(emit_access_type_t type)
    {
        access_stack.push(type);
    }

    emit_access_type_t current_access() const {
        if (access_stack.empty()) {
            return emit_access_type_t::read;
        }
        return access_stack.top();
    }

    void pop_access()
    {
        if (access_stack.empty()) {
            return;
        }
        access_stack.pop();
    }

    bool has_scratch_register() const
    {
        return !scratch_registers.empty();
    }

    void clear_scratch_registers()
    {
        while (!scratch_registers.empty()) {
            scratch_registers.pop();
        }
    }

    i_registers_t pop_scratch_register() {
        if (scratch_registers.empty()) {
            return i_registers_t::i0;
        }
        auto reg = scratch_registers.top();
        scratch_registers.pop();
        return reg;
    }

    void push_scratch_register(i_registers_t reg)
    {
        scratch_registers.push(reg);
    }

    std::stack<std::any> data_stack {};
    std::stack<i_registers_t> scratch_registers {};
    std::stack<emit_access_type_t> access_stack {};
};
}

#endif // COMPILER_EMIT_CONTEXT_H_
