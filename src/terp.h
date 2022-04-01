//
// Created by 12132 on 2022/3/26.
//

#ifndef TERP_H_
#define TERP_H_
#include <cstdint>
#include <string>
#include <functional>
#include "result.h"
#include "instruction.h"
#include <map>

namespace gfx {
	class terp {
	public:
		/// using to call cpp code
		using trap_callable = std::function<void(terp*)>;
		/// 中断向量表
		static constexpr size_t interrupt_vector_table_start = 0u;
		static constexpr size_t interrupt_vector_table_size = 16u;
		static constexpr size_t interrupt_vector_table_end = sizeof(uint64_t) * interrupt_vector_table_size;
		static constexpr size_t program_start = interrupt_vector_table_end;

		explicit terp(size_t heap_size);

		virtual ~terp();

		void reset();

		void swi(uint8_t index, uint64_t address);

		bool initialize(result& r);

		const register_file_t& register_file() const
		{
			return registers_;
		}

		uint8_t* heap() const
		{
			return heap_;
		}

		uint64_t pop();

		uint64_t peek() const;

		void push(uint64_t value);

		bool step(result& r);

		inline bool has_exited() const
		{
			return exited_;
		}

		void dump_heap(uint64_t offset, size_t size = 256);

		void dump_state(uint8_t count = 16);

		std::string disassemble(result& r, uint64_t address);

		std::string disassemble(const instruction_t& inst) const;

		void register_trap(uint8_t index, const trap_callable& callable);

		void remove_trap(uint8_t index);
	protected:

		bool set_target_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, uint64_t value);

		bool get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, uint64_t& value) const;

		bool set_target_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, double value);

		bool get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, double& value) const;

		inline uint8_t op_size_in_bytes(op_sizes size) const
		{
			switch (size) {
				case op_sizes::none:  return 0;
				case op_sizes::byte:  return 1;
				case op_sizes::dword: return 2;
				case op_sizes::word:  return 4;
				case op_sizes::qword: return 8;
				default:
					return 0;
			}
		}

	private:
		inline uint64_t* qword_ptr(uint64_t address) const
		{
			return reinterpret_cast<uint64_t*>(heap_ + address);
		}
	private:
		inline static std::map<op_codes, std::string> s_op_code_names = {
			{op_codes::nop,    "NOP"},  {op_codes::load,   "LOAD"}, {op_codes::store,  "STORE"},
			{op_codes::copy,   "COPY"}, {op_codes::fill,   "FILL"}, {op_codes::move,   "MOVE"},
			{op_codes::push,   "PUSH"}, {op_codes::pop,    "POP"}, {op_codes::inc,    "INC"},
			{op_codes::dec,    "DEC"},  {op_codes::add,    "ADD"}, {op_codes::sub,    "SUB"},
			{op_codes::mul,    "MUL"},  {op_codes::div,    "DIV"}, {op_codes::mod,    "MOD"},
			{op_codes::neg,    "NEG"},  {op_codes::shr,    "SHR"}, {op_codes::shl,    "SHL"},
			{op_codes::ror,    "ROR"},  {op_codes::rol,    "ROL"}, {op_codes::and_op, "AND"},
			{op_codes::or_op,  "OR"},   {op_codes::xor_op, "XOR"}, {op_codes::not_op, "NOT"},
			{op_codes::bis,    "BIS"},  {op_codes::bic,    "BIC"}, {op_codes::test,   "TEST"},
			{op_codes::cmp,    "CMP"},  {op_codes::bz,     "BZ"},  {op_codes::bnz,    "BNZ"},
			{op_codes::tbz,    "TBZ"},  {op_codes::tbnz,   "TBNZ"},{op_codes::bne,    "BNE"},
			{op_codes::beq,    "BEQ"},  {op_codes::bge,    "BGE"}, {op_codes::bg,     "BG"},
			{op_codes::ble,    "BLE"},  {op_codes::bl,     "BL"},  {op_codes::jsr,    "JSR"},
			{op_codes::dup,    "dup"},  {op_codes::rts,    "RTS"}, {op_codes::jmp,    "JMP"},
			{op_codes::meta,   "META"}, {op_codes::swi,   "SWI"},  {op_codes::trap,  "TRAP"},
			{op_codes::exit,   "EXIT"},
		};
		bool exited_ = false;
		uint32_t heap_size_ = 0;
		uint8_t * heap_ = nullptr;
		register_file_t registers_{};
		std::map<uint8_t, trap_callable> traps_{};

	};
}

#endif // TERP_H_
