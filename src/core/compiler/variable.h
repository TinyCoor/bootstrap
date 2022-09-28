//
// Created by 12132 on 2022/9/24.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_VARIABLE_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_VARIABLE_H_
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
    union {
        i_registers_t i;
        f_registers_t f;
    } value;
};

struct variable_t {
    bool init(gfx::assembler* assembler, instruction_block* block);

    bool read(assembler* assembler, instruction_block* block);

    bool write(assembler* assembler, instruction_block* block);

    void make_live(assembler* assembler);

    void make_dormat(assembler* assembler);

    std::string name;
    bool live = false;
    bool written = false;
    bool requires_read = false;
    bool address_loaded = false;
    identifier_usage_t usage;
    int64_t address_offset = 0;
    variable_register_t value_reg;
    compiler::type *type = nullptr;
    variable_register_t address_reg;
    stack_frame_entry_t *frame_entry = nullptr;
};
}
#endif //BOOTSTRAP_SRC_CORE_COMPILER_VARIABLE_H_
