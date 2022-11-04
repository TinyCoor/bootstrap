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
    bool reserve(compiler::session& session);

    void release(compiler::session& session);

    bool integer = true;
    bool allocated = false;
    register_t reg;
};

struct emit_context_t;

struct variable_t {
    bool init(compiler::session& session);

    bool read(compiler::session& session);

    bool write(compiler::session& session);

    void make_live(compiler::session& session);

    void make_dormat(compiler::session& session);

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
