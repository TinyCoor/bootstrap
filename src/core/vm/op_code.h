//
// Created by 12132 on 2022/4/3.
//

#ifndef OP_CODE_H_
#define OP_CODE_H_
#include <cstdint>
#include <string>

#include <map>
#include "instruction.h"
namespace gfx {
enum class op_codes : uint8_t {
	nop = 1,
    alloc,
    free,
    size,
	load,
    store,
    swap,
    move,
    moves,
    movez,
	copy,
    convert,
    fill,
    clr,
    push,
    pop,
	dup,
    add,
    sub,
    inc,
	dec,
    mul,
    div,
    mod,
	neg,
    shl,
    shr,
    rol,
	ror,
    and_op,
    or_op,
	xor_op,
    not_op,
    bis,
	bic,
    test,
    cmp,
    bz,
	bnz,
    tbz,
    tbnz,
    bne,
	beq,
    bg,
    bge,
    bl,
    ble,
	jsr,
    rts,
    jmp,
    swi,
    trap,
	ffi,
    meta,
    exit,
    setz,
    setnz,
};

inline static std::map<op_codes, std::string_view> s_op_code_names  = {
	{op_codes::nop,    "NOP"},
    {op_codes::alloc,  "ALLOC"},
    {op_codes::free, "FREE"},
	{op_codes::load,   "LOAD"},
    {op_codes::store,  "STORE"},
    {op_codes::size, "SIZE"},
	{op_codes::copy,   "COPY"},
    {op_codes::fill,   "FILL"},
    {op_codes::clr,   "CLR"},
    {op_codes::move,  "MOVE"},
    {op_codes::moves,  "MOVES"},
    {op_codes::movez,  "MOVEZ"},
    {op_codes::convert,  "CONVERT"},
	{op_codes::push,   "PUSH"},
    {op_codes::pop,    "POP"},
    {op_codes::inc,    "INC"},
	{op_codes::dec,    "DEC"},
    {op_codes::add,    "ADD"},
    {op_codes::sub,    "SUB"},
    {op_codes::mul,    "MUL"},
    {op_codes::div,    "DIV"},
    {op_codes::mod,    "MOD"},
	{op_codes::neg,    "NEG"},
    {op_codes::shr,    "SHR"},
    {op_codes::shl,    "SHL"},
	{op_codes::ror,    "ROR"},
    {op_codes::rol,    "ROL"},
    {op_codes::and_op, "AND"},
	{op_codes::or_op,  "OR"},
    {op_codes::xor_op, "XOR"},
    {op_codes::not_op, "NOT"},
	{op_codes::bis,    "BIS"},
    {op_codes::bic,    "BIC"},
    {op_codes::test,   "TEST"},
	{op_codes::cmp,    "CMP"},
    {op_codes::bz,     "BZ"},
    {op_codes::bnz,    "BNZ"},
	{op_codes::tbz,    "TBZ"},
    {op_codes::tbnz,   "TBNZ"},
    {op_codes::bne,    "BNE"},
	{op_codes::beq,    "BEQ"},
    {op_codes::bge,    "BGE"},
    {op_codes::bg,     "BG"},
	{op_codes::ble,    "BLE"},
    {op_codes::bl,     "BL"},
    {op_codes::jsr,    "JSR"},
	{op_codes::dup,    "DUP"},
    {op_codes::rts,    "RTS"},
    {op_codes::jmp,    "JMP"},
	{op_codes::meta,   "META"},
    {op_codes::swi,    "SWI"},
    {op_codes::trap,  "TRAP"},
	{op_codes::exit,   "EXIT"},
    {op_codes::swap,   "SWAP"},
    {op_codes::ffi,   "FFI"},
    {op_codes::setnz,  "SETNZ"},
    {op_codes::setz,   "SETZ"},
};

inline static std::string_view op_code_name(op_codes type) {
    auto it = s_op_code_names.find(type);
    if (it != s_op_code_names.end()) {
        return it->second;
    }
    return "";
}

struct mnemonic_operand_t {
    enum flags : uint8_t {
        none             = 0b00000000,
        integer_register = 0b00000001,
        float_register   = 0b00000010,
        immediate        = 0b00000100,
        pc_register      = 0b00001000,
        sp_register      = 0b00010000,
        fp_register      = 0b00100000,
    };

    uint8_t types;
    bool required = false;
};

struct mnemonic_t {
    op_codes code;
    std::vector<mnemonic_operand_t> operands {};
    size_t required_operand_count() const {
        size_t count = 0;
        for (const auto& op : operands) {
            count += op.required ? 1 : 0;
        }
        return count;
    }
};

inline static std::map<std::string, mnemonic_t> s_mnemonics = {
    {
        "NOP",
        mnemonic_t{
            op_codes::nop,
            {}
        }
    },
    {   "ALLOC",
        mnemonic_t{
            op_codes::alloc,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {   "FREE",
        mnemonic_t{
            op_codes::free,
            {
                {mnemonic_operand_t::flags::integer_register, true},
            }
        }
    },
    {   "SIZE",
        mnemonic_t{
            op_codes::size,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register, true},
            }
        }
    },
    {
        "LOAD",
        mnemonic_t{
            op_codes::load,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "STORE",
        mnemonic_t{
            op_codes::store,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "COPY",
        mnemonic_t{
            op_codes::copy,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "CVRT",
        mnemonic_t{
            op_codes::convert,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
            }
        }
    },
    {
        "FILL",
        mnemonic_t{
            op_codes::fill,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "CLR",
        mnemonic_t{
            op_codes::clr,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
            }
        }
    },
    {
        "MOVE",
        mnemonic_t{
            op_codes::move,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "MOVES",
        mnemonic_t{
            op_codes::moves,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "MOVEZ",
        mnemonic_t{
            op_codes::movez,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "PUSH",
        mnemonic_t{
            op_codes::push,
            {
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "POP",
        mnemonic_t{
            op_codes::pop,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true}
            }
        }
    },
    {
        "DUP",
        mnemonic_t{
            op_codes::dup,
            {

            }
        }
    },
    {
        "INC",
        mnemonic_t{
            op_codes::inc,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true}
            }
        }
    },
    {
        "DEC",
        mnemonic_t{
            op_codes::dec,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true}
            }
        }
    },
    {
        "ADD",
        mnemonic_t{
            op_codes::add,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "SUB",
        mnemonic_t{
            op_codes::sub,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "MUL",
        mnemonic_t{
            op_codes::mul,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "DIV",
        mnemonic_t{
            op_codes::div,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register
                     | mnemonic_operand_t::flags::float_register
                     | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "MOD",
        mnemonic_t{
            op_codes::mod,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "NEG",
        mnemonic_t{
            op_codes::neg,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::float_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "SHR",
        mnemonic_t{
            op_codes::shr,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "SHL",
        mnemonic_t{
            op_codes::shl,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "ROR",
        mnemonic_t{
            op_codes::ror,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "ROL",
        mnemonic_t{
            op_codes::rol,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "AND",
        mnemonic_t{
            op_codes::and_op,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "OR",
        mnemonic_t{
            op_codes::or_op,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "XOR",
        mnemonic_t{
            op_codes::xor_op,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "NOT",
        mnemonic_t{
            op_codes::not_op,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "BIS",
        mnemonic_t{
            op_codes::bis,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "BIC",
        mnemonic_t{
            op_codes::bic,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "TEST",
        mnemonic_t{
            op_codes::test,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "CMP",
        mnemonic_t{
            op_codes::cmp,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "BZ",
        mnemonic_t{
            op_codes::bz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "BNZ",
        mnemonic_t{
            op_codes::bnz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "TBZ",
        mnemonic_t{
            op_codes::tbz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "TBNZ",
        mnemonic_t{
            op_codes::tbnz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "BNE",
        mnemonic_t{
            op_codes::bne,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "BEQ",
        mnemonic_t{
            op_codes::beq,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "BG",
        mnemonic_t{
            op_codes::bg,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "BGE",
        mnemonic_t{
            op_codes::bge,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "BL",
        mnemonic_t{
            op_codes::bl,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "BLE",
        mnemonic_t{
            op_codes::ble,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "SETZ",
        mnemonic_t{
            op_codes::setz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
            }
        }
    },
    {
        "SETNZ",
        mnemonic_t{
            op_codes::setnz,
            {
                {mnemonic_operand_t::flags::integer_register, true},
            }
        }
    },
    {
        "JSR",
        mnemonic_t{
            op_codes::jsr,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "RTS",
        mnemonic_t{
            op_codes::rts,
            {}
        }
    },
    {
        "JMP",
        mnemonic_t{
            op_codes::jmp,
            {
                {mnemonic_operand_t::flags::pc_register | mnemonic_operand_t::flags::immediate, true},
                {mnemonic_operand_t::flags::immediate, false},
            }
        }
    },
    {
        "SWI",
        mnemonic_t{
            op_codes::swi,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "SWAP",
        mnemonic_t{
            op_codes::swap,
            {
                {mnemonic_operand_t::flags::integer_register, true},
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "TRAP",
        mnemonic_t{
            op_codes::trap,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "FFI",
        mnemonic_t{
            op_codes::ffi,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },

    {
        "META",
        mnemonic_t{
            op_codes::meta,
            {
                {mnemonic_operand_t::flags::integer_register | mnemonic_operand_t::flags::immediate, true},
            }
        }
    },
    {
        "EXIT",
        mnemonic_t{
            op_codes::exit,
            {}
        }
    },
};

inline static mnemonic_t* mnemonic(const std::string& code) {
    const auto it = s_mnemonics.find(code);
    if (it != s_mnemonics.end()) {
        return &it->second;
    }
    return nullptr;
}

}
#endif // OP_CODE_H_
