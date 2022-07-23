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
    static constexpr op_sizes ToOpSize(T val)
    {
        return TypeToOpSizeImpl(val);
    }
};

template<enum op_codes>
struct Instruction;

template<>
struct Instruction<op_codes::neg>
{
   static constexpr op_codes op = op_codes::neg;
};

template<>
struct Instruction<op_codes::push>
{
    static constexpr op_codes op = op_codes::push;
};

template<>
struct Instruction<op_codes::pop>
{
    static constexpr op_codes op = op_codes::pop;
};

template<>
struct Instruction<op_codes::jmp>
{
    static constexpr op_codes op = op_codes::jmp;
};

template<>
struct Instruction<op_codes::load>
{
    static constexpr op_codes op = op_codes::load;
};

template<>
struct Instruction<op_codes::store>
{
    static constexpr op_codes op = op_codes::store;
};

template<>
struct Instruction<op_codes::move>
{
    static constexpr op_codes op = op_codes::move;
};

template<>
struct Instruction<op_codes::add>
{
    static constexpr op_codes op = op_codes::add;
    static constexpr int operand_count = 3;
};

template<>
struct Instruction<op_codes::sub>
{
    static constexpr op_codes op = op_codes::sub;
    static constexpr int operand_count = 3;

};

template<>
struct Instruction<op_codes::inc>
{
    static constexpr op_codes op = op_codes::inc;
    static constexpr int operand_count = 1;
    static constexpr uint8_t type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;
};

template<>
struct Instruction<op_codes::dec>
{
    static constexpr op_codes op = op_codes::dec;
    static constexpr int operand_count = 1;
    static constexpr uint8_t type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;

};

class InstructionBuild {
private:

public:

};
}
#endif // VM_INSTRUCTION_EXECUTOR_H_
