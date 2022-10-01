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
    register_type_t type;
    registers_t i;
};

struct register_allocator_t {
    register_allocator_t()
    {
        reset();
    }

    void reset()
    {
        used.clear();
        while (!available.empty()) {
            available.pop();
        }
        for (int8_t r = 63; r >= 0; r--) {
            available.push(static_cast<registers_t>(r));
        }
    }

    void free(registers_t reg)
    {
        if (used.erase(reg) > 0) {
            available.push(reg);
        }
    }

    bool allocate(registers_t &reg)
    {
        if (available.empty()) {
            return false;
        }
        reg = available.top();
        available.pop();
        used.insert(reg);
        return true;
    }

    std::set<registers_t> used{};
    std::stack<registers_t> available{};
};
}


#endif // VM_REGISTER_ALLOCATOR_H_
