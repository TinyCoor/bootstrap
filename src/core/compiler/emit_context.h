//
// Created by 12132 on 2022/7/23.
//

#ifndef COMPILER_EMIT_CONTEXT_H_
#define COMPILER_EMIT_CONTEXT_H_
#include <any>
#include <stack>
#include <string>
#include "variable.h"
#include "vm/terp.h"
#include "vm/assembler.h"
#include "vm/instruction.h"
#include "compiler_types.h"

namespace gfx::compiler {

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

    void free_variable(const std::string& name);

    variable_t* variable(const std::string& name);

    variable_t* variable_for_element(compiler::element* element);

    variable_t* allocate_variable(result& r, const std::string& name, compiler::type* type,
        identifier_usage_t usage, stack_frame_entry_t* frame_entry = nullptr);

    void pop();

    void push_scratch_register(register_t reg);

    [[nodiscard]] bool has_scratch_register() const;

    void clear_scratch_registers();

    register_t pop_scratch_register();

    terp* terp = nullptr;
    assembler* assembler = nullptr;
    program* program = nullptr;
    std::stack<std::any> data_stack {};
    std::stack<register_t> scratch_registers {};
    std::unordered_map<std::string, variable_t> variables {};
};
}

#endif // COMPILER_EMIT_CONTEXT_H_
