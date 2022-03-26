//
// Created by 12132 on 2022/3/26.
//

#ifndef BOOTSTRAP__TERP_H_
#define BOOTSTRAP__TERP_H_
#include <cstdint>
#include <string>
#include "result.h"


namespace cfg {

	/// cfg interpreter, which consume cfg IR
	///
	/// register-based machine
	///
	/// register file
	/// general purpose data or address
	/// I0- I63: integer
	///
	/// data only
	/// f0-f63: float point registers(double precision)
	///
	/// stack pointer: sp (like register)
	/// program counter: pc (read only)
	/// flags:  fr (definite read)
	/// status: sr ()
	///
	///
	/// instructions
	/// ------------------------------------------------------
	/// load(.b|.w|.dw|.qw)
	///                  ^ default
	/// .b = 8 bit
	/// .w = 16 bit
	/// .dw = 32 bit
	/// .qw = 64 bit
	///
	/// non-used bit are zero extended
	///
	/// store(.b|.w|.dw|.qw)
	///
	/// non-used bit are zero extended
	///
	/// address mode:
	/// 	{target-register}, [{source-register}]
	///		"				", [{source-register}, offset-constant]
	///		"				", [{source-register}, {offset-register}]
	///		"				", {source-register}, post increment constant++
	///		"				", {source-register}, post increment register++
	///		"				", {source-register}, ++pre increment constant
	///		"				", {source-register}, ++pre increment register
	///		"				", {source-register}, post decrement constant--
	///		"				", {source-register}, post decrement register--
	///		"				", {source-register}, --pre decrement constant
	///		"				", {source-register}, --pre decrement register
	///
	/// copy {source-register}, {target-register}, {length-constant}
	/// copy {source-register}, {target-register}, {length-register}
	///
	/// fill constant, {target-register}, {length-constant}
	/// fill constant, {target-register}, {length-register}
	///
	///	fill {source-register}, {target-register}, {length-constant}
	/// fill {source-register}, {target-register}, {length-register}
	///
	///
	/// register/constant
	/// ---------------------------------------------------------------
	/// move(.b|.w|.dw|.qw)	{source-constant}, {target-register}
	///						{source-register}, {target-register}
	/// move.b #$06 r3
	/// move r3 r6
	///
	/// stack
	/// ---------------------------------------------------------------
	/// push(.b|.w|.dw|.qw)
	/// pop(.b|.w|.dw|.qw)
	///
	/// sp register behaves line rxx register
	///
	/// ALU
	/// ---------------------------------------------------------------
	/// size applicable to all {.b|.w|.dw|.qw}
	/// add
	/// addc
	///
	/// sub
	/// subc
	///
	/// mul
	/// div
	/// mod
	///
	/// shr
	/// shl
	/// ror
	/// rol
	///
	/// not
	/// and
	/// or
	/// xor
	///
	/// bis(bit set)
	/// bic(bit clear)
	/// test
	///
	/// cmp (compare register to register or register to constant
	///
	/// branch/conditional execution
	/// -----------------------------------------------
	/// bz  (branch if  zero)
	/// bnz	(branch if  zero)
	///
	/// tbz (test & branch if not set)
	/// tbnz(test & branch if set)
	///
	///
	/// bne
	/// beq
	/// bae
	/// ba
	/// ble
	/// bl
	/// bo
	/// bcc
	/// bcs
	///
	/// jsr	- equivalent to call (encode tail call
	///			push current pc + 1
	///         jmp to address
	///
	/// ret - jump to address on stack
	/// jmp
	///		- absolute constant jmp $0xffffff
	///     - indirect register jmp [r14]
	///     - direct jmp r14
	///
	/// nop
	///
	///

	struct register_file_t {
		uint64_t i[64];
		double f[64];
		uint64_t pc;
		uint64_t sp;
		uint64_t fr;
		uint64_t sr;
	};

	enum class opcodes : uint16_t {
		nop =0,
		load,
		store,
		move,
		push,
		pop,
		add,
		sub,
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
		bae,
		ba,
		ble,
		bl,
		bo,
		bcc,
		bcs,
		jsr,
		ret,
		jmp,
		meta,
		debug,
	};

	enum class register_types : uint8_t {
		register_integer,
		register_floating_point,
		register_pc,
		register_sp,
		register_flags,
		register_status,
	};

	struct operand_encoding_t {
		register_types source_type;
		uint8_t source_index;
		uint64_t value;
	};

	struct instruction_t {
		opcodes op;
		uint8_t operands_count;
		operand_encoding_t operand[4];
	};

	struct debug_information_t {
		uint32_t line_number;
		int16_t column_number;
		std::string symbol;
		std::string source_file;
	};

	class terp {
	public:
		explicit terp(uint32_t heap_size);
		virtual ~terp();

		bool initialize(result& r);
		uint64_t pop();
		void push(uint64_t value);

		size_t heap_size() const;
		size_t heap_size_in_qwords() const;

		const register_file_t& register_file() const;
	protected:

	private:
		uint32_t heap_size_ = 0;
		uint64_t* heap_ = nullptr;
		register_file_t registers_{};

	};
}

#endif //BOOTSTRAP__TERP_H_
