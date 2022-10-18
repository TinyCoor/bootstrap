//
// Created by 12132 on 2022/9/24.
//

#ifndef COMPILER_VARIABLE_H_
#define COMPILER_VARIABLE_H_
#include <string>
#include "elements/element_types.h"
#include "vm/terp.h"
#include "vm/assembler.h"
#include "vm/instruction.h"
#include "compiler_types.h"
namespace gfx::compiler {
struct variable_register_t {
    bool reserve(assembler* assembler);

    void release(assembler* assembler);

    bool integer = true;
    bool allocated = false;
    register_t reg;
};

struct emit_context_t;

struct variable_t {
    bool init(emit_context_t& context, instruction_block* block);

    bool read(emit_context_t& context, instruction_block* block);

    bool write(emit_context_t& context, instruction_block* block);

    void make_live(emit_context_t& context);

    void make_dormat(emit_context_t& context);

    std::string name;
    bool live = false;
    bool written = false;
    bool requires_read = false;
    bool address_loaded = false;
    identifier_usage_t usage;
    int64_t address_offset = 0;
    variable_register_t value_reg;
    compiler::type *type = nullptr;
    variable_register_t address_reg{.reg = {
        .size = op_sizes::qword,
        .type = register_type_t::integer,}
    };
    stack_frame_entry_t *frame_entry = nullptr;
};
}
#endif // COMPILER_VARIABLE_H_
