//
// Created by 12132 on 2022/3/28.
//

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
#include "common/result.h"
#include "op_code.h"
#include <functional>
#include <type_traits>

namespace gfx {
enum class register_type_t : uint8_t {
    none,
    pc,
    sp,
    fp,
    fr,
    sr,
    integer,
    floating_point,
};

enum registers_t : uint8_t {
	r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
	r8,  r9,  r10, r11, r12, r13, r14, r15,
	r16, r17, r18, r19, r20, r21, r22, r23,
	r24, r25, r26, r27, r28, r29, r30, r31,
	r32, r33, r34, r35, r36, r37, r38, r39,
	r40, r41, r42, r43, r44, r45, r46, r47,
	r48, r49, r50, r51, r52, r53, r54, r55,
	r56, r57, r58, r59, r60, r61, r62, r63,

	pc, sp, fr, sr, fp,
};

union register_value_alias_t {
    double   d;
    uint64_t u;
};

struct register_t {
    static register_t pc()
    {
        return register_t {
            .number = registers_t::pc,
            .type = register_type_t::pc,
        };
    }

    static register_t sp()
    {
        return register_t {
            .number = registers_t::sp,
            .type = register_type_t::sp,
        };
    }

    static register_t fp()
    {
        return register_t {
            .number = registers_t::fp,
            .type = register_type_t::fp,
        };
    }
    registers_t number = registers_t::r0;
    register_type_t type = register_type_t::none;
    register_value_alias_t value{.u = 0};
};

static constexpr const uint32_t register_integer_start   = 0;
static constexpr const uint32_t number_integer_registers = 64;
static constexpr const uint32_t register_float_start     = number_integer_registers;
static constexpr const uint32_t number_float_registers   = 64;
static constexpr const uint32_t number_special_registers = 5;
static constexpr const uint32_t number_general_purpose_registers = number_integer_registers + number_float_registers;
static constexpr const uint32_t number_total_registers   = number_integer_registers
    + number_float_registers
    + number_special_registers;
static constexpr const uint32_t register_special_start   = number_integer_registers
    + number_float_registers;
static constexpr const uint32_t register_pc = register_special_start;
static constexpr const uint32_t register_sp = register_special_start + 1;
static constexpr const uint32_t register_fr = register_special_start + 2;
static constexpr const uint32_t register_sr = register_special_start + 3;
static constexpr const uint32_t register_fp = register_special_start + 4;

static inline size_t register_index(registers_t r, register_type_t type)
{
    switch (r) {
        case pc: return register_pc;
        case sp: return register_sp;
        case fr: return register_fr;
        case sr: return register_sr;
        case fp: return register_fp;
        default:
            if (type == register_type_t::floating_point) {
                return register_float_start + static_cast<uint8_t>(r);
            } else {
                return static_cast<uint8_t>(r);
            }
    }
}

struct register_comparator {
    bool operator()(
        const register_t& lhs,
        const register_t& rhs) const {
        auto lhs_index = register_index(lhs.number, lhs.type);
        auto rhs_index = register_index(rhs.number, rhs.type);
        return lhs_index < rhs_index;
    }
};

struct register_file_t {
	enum flags_t : uint64_t {
		zero     = 0x1,
		carry    = 0x2,
		overflow = 0x4,
		negative = 0x8,
		extended = 0x16,
		subtract = 0x32,
	};

	[[nodiscard]] bool flags(flags_t flag) const
    {
		return (r[register_fr].u & flag) != 0;
	}

	void flags(flags_t flag, bool value) {
		if (value) {
            r[register_fr].u |= flag;
		} else {
            r[register_fr].u &= ~flag;
		}
	}
    register_value_alias_t r[number_total_registers];
};

enum class op_sizes : uint8_t {
	none,
	byte,
	word,
	dword,
	qword,
};

static inline uint8_t op_size_in_bytes(op_sizes size)
{
	switch (size) {
		case op_sizes::byte:  return 1u;
        case op_sizes::word:  return 2u;
		case op_sizes::dword: return 4u;
		case op_sizes::qword: return 8u;
		default: return 0u;
	}
}
static inline op_sizes op_size_for_byte_size(size_t size) {
    switch (size) {
        case 1u:     return op_sizes::byte;
        case 2u:     return op_sizes::word;
        case 4u:     return op_sizes::dword;
        case 8u:     return op_sizes::qword;
        default:    return op_sizes::none;
    }
}

union operand_value_alias_t {
    uint8_t r;
    uint64_t u;
    float f;
    double d;
};

struct operand_value_t {
    register_type_t type = register_type_t::none;
    operand_value_alias_t alias {.u = 0};
};

struct operand_encoding_t {
	using flags_t = uint8_t;

	enum flags : uint8_t {
		none     = 0b00000000,
		constant = 0b00000000,
		reg      = 0b00000001,
		integer  = 0b00000010,
		negative = 0b00000100,
		prefix   = 0b00001000,
		postfix  = 0b00010000,
        unresolved  = 0b00100000,
	};

    void clear_unresolved();

    bool is_reg() const;

	bool is_prefix() const;

	bool is_postfix() const;

	bool is_integer() const;

	bool is_negative() const;

    bool is_unresolved() const;

    flags_t type = flags::reg | flags::integer;
    operand_value_alias_t value{.u = 0};
};

struct meta_information_t {
	uint32_t line_number;
	uint16_t column_number;
	std::string symbol;
	std::string source_file;
};

using id_resolve_callable = std::function<std::string (uint64_t)>;

struct instruction_t {
	static constexpr size_t base_size = 3;
	static constexpr size_t alignment = 4;

	size_t encode(result& r, uint8_t* heap, uint64_t address);

	size_t decode(result& r, uint8_t* heap, uint64_t address);

	[[nodiscard]] size_t align(uint64_t value, size_t size) const;

	[[nodiscard]] size_t encoding_size() const;

    [[nodiscard]] std::string disassemble(const id_resolve_callable& id_resolver = nullptr) const;

	op_codes op = op_codes::nop;
	op_sizes size = op_sizes::none;
	uint8_t operands_count = 0;
	operand_encoding_t operands[4];
};

}

#endif // INSTRUCTION_H_
