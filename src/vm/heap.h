//
// Created by 12132 on 2022/4/14.
//

#ifndef VM_HEAP_H_
#define VM_HEAP_H_
#include <cstdint>

// 0: pointer to the top of the stack
// 1: pointer to the bottom of the stack, based on the stack_size value
//    passed into the vm::terp
// 2: pointer to the start of program space
// 3: pointer to the end of bootstrap code/beginning of free space
enum class heap_vectors_t : uint8_t {
	top_of_stack = 0,
	bottom_of_stack,
	program_start,
	free_space_start,
};

struct heap_block_t {
	enum flags_t : uint8_t {
		none       = 0b00000000,
		allocated  = 0b00000001,
	};

	inline void mark_allocated() {
		flags |= flags_t::allocated;
	}

	inline void clear_allocated() {
		flags &= ~flags_t::allocated;
	}

	inline bool is_free() const {
		return (flags & flags_t::allocated) == 0;
	}

	inline bool is_allocated() const {
		return (flags & flags_t::allocated) != 0;
	}

	uint64_t size = 0;
	uint64_t address = 0;
	heap_block_t* prev = nullptr;
	heap_block_t* next = nullptr;
	uint8_t flags = flags_t::none;
};


#endif // VM_HEAP_H_
