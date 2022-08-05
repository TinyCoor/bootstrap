//
// Created by 12132 on 2022/7/23.
//

#ifndef VM_INSTRUCTION_EXECUTOR_H_
#define VM_INSTRUCTION_EXECUTOR_H_
#include "op_code.h"
#include "instruction.h"
namespace gfx {

class TypeToOpSize {
private:
    static constexpr op_sizes TypeToOpSizeImpl(uint8_t)
    {
        return op_sizes::byte;
    }

    static constexpr op_sizes TypeToOpSizeImpl(uint16_t)
    {
        return op_sizes::word;
    }

    static constexpr op_sizes TypeToOpSizeImpl(uint32_t)
    {
        return op_sizes::dword;
    }

    static constexpr op_sizes TypeToOpSizeImpl(uint64_t)
    {
        return op_sizes::qword;
    }

    static constexpr op_sizes TypeToOpSizeImpl(float)
    {
        return op_sizes::dword;
    }

    static constexpr op_sizes TypeToOpSizeImpl(double)
    {
        return op_sizes::qword;
    }

public:
    template<typename T>
    static constexpr op_sizes ToOpSize()
    {
        return TypeToOpSizeImpl(T{});
    }
};

}
#endif // VM_INSTRUCTION_EXECUTOR_H_
