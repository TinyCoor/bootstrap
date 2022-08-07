//
// Created by 12132 on 2022/7/23.
//

#ifndef COMPILER_EMIT_CONTEXT_H_
#define COMPILER_EMIT_CONTEXT_H_
#include <any>
#include <stack>
#include <string>
#include "vm/terp.h"
#include "vm/assembler.h"
#include "vm/instruction.h"

namespace gfx::compiler {

enum class emit_access_type_t {
    read,
    write
};

struct block_data_t {
    bool recurse = true;
};

struct if_data_t {
    std::string true_branch_label;
    std::string false_branch_label;
};
class program;

struct emit_context_t {
    emit_context_t(gfx::terp* terp, gfx::assembler* assembler, compiler::program* program);

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

    void push_if(const std::string& true_label_name, const std::string& false_label_name);

    void push_block(bool recurse);

    void push_access(emit_access_type_t type);

    [[nodiscard]] emit_access_type_t current_access() const;

    void pop_access();

    void pop();

    void push_scratch_register(i_registers_t reg);

    [[nodiscard]] bool has_scratch_register() const;

    void clear_scratch_registers();

    i_registers_t pop_scratch_register();

    terp* terp = nullptr;
    assembler* assembler = nullptr;
    program* program = nullptr;
    std::stack<std::any> data_stack {};
    std::stack<i_registers_t> scratch_registers {};
    std::stack<emit_access_type_t> access_stack {};
};
}

#endif // COMPILER_EMIT_CONTEXT_H_
