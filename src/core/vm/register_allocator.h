//
// Created by 12132 on 2022/8/5.
//

#ifndef VM_REGISTER_ALLOCATOR_H_
#define VM_REGISTER_ALLOCATOR_H_
#include <set>
#include <stack>
#include <cstdint>
#include "instruction.h"

namespace gfx {

struct target_register_t {
    op_sizes size;
    register_t reg;
};

struct register_allocator_t {
    register_allocator_t();

    void reset();

    void free(const register_t &reg);

    bool allocate(register_t &reg);

    std::stack<register_t> available_float{};
    std::stack<register_t> available_integer{};
    std::set<register_t, register_comparator> used{};

};
}


#endif // VM_REGISTER_ALLOCATOR_H_
