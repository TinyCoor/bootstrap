//
// Created by 12132 on 2022/8/5.
//

#ifndef VM_REGISTER_ALLOCATOR_H_
#define VM_REGISTER_ALLOCATOR_H_
#include <set>
#include <stack>
#include <cstdint>

namespace gfx {
enum class target_register_type_t {
    none,
    integer,
    floating_point,
};

struct target_register_t {
    op_sizes size;
    target_register_type_t type;
    union {
        i_registers_t i;
        f_registers_t f;
    } reg;
};

template<typename T>
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
            available.push(static_cast<T>(r));
        }
    }

    void free(T reg)
    {
        if (used.erase(reg) > 0) {
            available.push(reg);
        }
    }

    bool allocate(T &reg)
    {
        if (available.empty()) {
            return false;
        }
        reg = available.top();
        available.pop();
        used.insert(reg);
        return true;
    }

    std::set<T> used{};
    std::stack<T> available{};
};
}


#endif // VM_REGISTER_ALLOCATOR_H_
