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
struct variable_t {
    bool init(gfx::assembler* assembler, instruction_block* block);

    bool read(assembler* assembler, instruction_block* block);

    bool write(assembler* assembler, instruction_block* block);

    std::string name;
    bool written = false;
    identifier_usage_t usage;
    int64_t address_offset = 0;
    i_registers_t address_reg;
    union {
        i_registers_t i;
        f_registers_t f;
    } value_reg;
    bool requires_read = false;
    bool address_loaded = false;
    compiler::type *type = nullptr;
    stack_frame_entry_t *frame_entry = nullptr;
};
}
#endif //BOOTSTRAP_SRC_CORE_COMPILER_VARIABLE_H_
