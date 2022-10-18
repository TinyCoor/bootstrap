//
// Created by 12132 on 2022/3/26.
//

#ifndef TERP_H_
#define TERP_H_
#include "instruction.h"
#include "heap.h"
#include "common/result.h"
#include "shared_library.h"
#include "instruction_cache.h"
#include <cstdint>
#include <string>
#include <functional>
#include <map>

namespace gfx {
	class terp {
	public:
		/// using to call cpp code
		using trap_callable = std::function<void(terp*)>;

		static constexpr uint64_t mask_byte        = 0xff;
		static constexpr uint64_t mask_byte_clear  = ~mask_byte;

		static constexpr uint64_t mask_word        = 0xffff;
		static constexpr uint64_t mask_word_clear  = ~mask_word;

		static constexpr uint64_t mask_dword       = 0xffffffff;
		static constexpr uint64_t mask_dword_clear = ~mask_dword;

		static constexpr uint64_t mask_byte_negative  = 0x80;
		static constexpr uint64_t mask_word_negative  = 0x8000;
		static constexpr uint64_t mask_dword_negative = 0x80000000;
		static constexpr uint64_t mask_qword_negative = 0x8000000000000000;

		/// 中断向量表
		static constexpr size_t interrupt_vector_table_start = 0u;
		static constexpr size_t interrupt_vector_table_size = 16u;
		static constexpr size_t interrupt_vector_table_end = sizeof(uint64_t) * interrupt_vector_table_size;

		/// Heap Start
		static constexpr size_t heap_vector_table_start = interrupt_vector_table_end;
		static constexpr size_t heap_vector_table_size = 16u;
		static constexpr size_t heap_vector_table_end = heap_vector_table_start
			+ (sizeof(uint16_t) * heap_vector_table_size);

		static constexpr size_t program_start = heap_vector_table_end;

		static constexpr uint8_t trap_out_of_memory = 0xff;
		static constexpr uint8_t trap_invalid_ffi_call = 0xfe;

		explicit terp(size_t heap_size, size_t stack_size);

        terp(const terp& other) = delete;

		virtual ~terp();

		void reset();

        bool run(result& r);

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

		size_t stack_size() const
		{
			return stack_size_;
		}

		const meta_information_t& meta_information() const
        {
			return meta_information_;
		}

        uint64_t read(op_sizes size, uint64_t address) const;

        void write(op_sizes size, uint64_t address, uint64_t value);

        uint64_t alloc(uint64_t size);

		uint64_t free(uint64_t address);

		uint64_t size(uint64_t address);

		void heap_free_space_begin(uint64_t address);

		uint64_t heap_vector(heap_vectors_t vector) const;

		void heap_vector(heap_vectors_t vector, uint64_t address);

		shared_library* shared_library(const std::filesystem::path& path);

		class shared_library* load_shared_library(result& r, const std::filesystem::path& path);

		std::vector<uint64_t> jump_to_subroutine(result& r, uint64_t address);

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

		void dump_shared_libraries();

		void register_trap(uint8_t index, const trap_callable& callable);

		void remove_trap(uint8_t index);

		bool register_foreign_function(result& r, function_signature_t& signature);
	protected:
		void free_heap_block_list();

		void execute_trap(uint8_t index);

		bool set_target_operand_value(result& r, const instruction_t& inst, uint8_t operand_index,
            const operand_value_t& value);

		bool get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index,
            operand_value_t& value) const;

		bool get_constant_address_or_pc_with_offset(result& r, const instruction_t& inst, uint8_t operand_index,
			uint64_t inst_size, operand_value_t& address);

	private:
        void set_pc_address(uint64_t address);

		void set_zoned_value(register_value_alias_t& reg, uint64_t value, op_sizes size);

		bool has_overflow(uint64_t lhs, uint64_t rhs, uint64_t result, op_sizes size);

		bool has_carry(operand_value_t value, op_sizes size);

		bool is_negative(operand_value_t value, op_sizes size);
	private:
		bool exited_ = false;
		size_t heap_size_ = 0;
		size_t stack_size_ = 0;
		uint8_t *heap_ = nullptr;
		register_file_t registers_{};
		instruction_cache inst_cache_;
		meta_information_t meta_information_ {};
		std::map<uint8_t, trap_callable> traps_{};
		DCCallVM* call_vm_ = nullptr;
		heap_block_t* head_heap_block_ = nullptr;
		std::unordered_map<uint64_t, heap_block_t*> address_blocks_ {};
		std::unordered_map<void*, function_signature_t> foreign_functions_{};
	    std::unordered_map<std::string, class shared_library> shared_libraries_{};
	};
}

#endif // TERP_H_
