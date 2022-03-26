//
// Created by 12132 on 2022/3/26.
//

#include "terp.h"
#include "formatter.h"
#include <fmt/format.h>

namespace cfg {

terp::terp(uint32_t heap_size) : heap_size_(heap_size)
{
}

terp::~terp() {
	if (heap_) {
		delete heap_;
		heap_ = nullptr;
		heap_size_ = 0;
	}
}

void terp::dump_state() {
	fmt::print("cfg interpreter state:\n");
	fmt::print("-----------------------------------------------\n");
	fmt::print("I0={:08x} | I1= {:08x} | I2= {:08x} | I3= {:08x}\n",
			   registers_.i[0], registers_.i[1], registers_.i[2], registers_.i[3]);
	fmt::print("I4={:08x} | I5= {:08x} | I6= {:08x} | I7= {:08x}\n",
			   registers_.i[4], registers_.i[5], registers_.i[6], registers_.i[7]);
	fmt::print("\n");
	fmt::print("PC={:08x} | SP= {:08x} | FR= {:08x} | SR= {:08x}\n\n",
			   registers_.pc, registers_.sp, registers_.fr, registers_.sr);
}

size_t terp::encode_instruction(result& r, uint64_t address, instruction_t instruction)
{
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	auto qword_address = address / sizeof(uint64_t);
	uint8_t size = 4;

	auto encoding_ptr = reinterpret_cast<uint8_t*>(heap_ + qword_address);
	auto op_ptr = reinterpret_cast<uint16_t*>(encoding_ptr + 1);
	*op_ptr = static_cast<uint16_t>(instruction.op);

	*(encoding_ptr + 3) = static_cast<uint8_t>(instruction.size);
	*(encoding_ptr + 4) = instruction.operands_count;

	auto offset = 5u;
	for (size_t i = 0u; i < instruction.operands_count ; ++i) {
		*(encoding_ptr + offset) = static_cast<uint8_t>(instruction.operands[i].type);
		++offset;
		++size;

		*(encoding_ptr + offset) = instruction.operands[i].index;
		++offset;
		++size;
		if (instruction.operands[i].type == operand_types::constant) {
			auto *constant_value_ptr = reinterpret_cast<uint64_t*>(encoding_ptr + offset);
			*constant_value_ptr = instruction.operands[i].value;
			offset += sizeof(uint64_t);
			size += sizeof(uint64_t);
		}
	}
	if (instruction.operands_count > 0){
		++size;
	}

	if (size < 8)
		size = 8;

	size = static_cast<uint8_t>(align(size, sizeof(uint64_t)));
	*encoding_ptr = size;

	return size;
}

size_t terp::decode_instruction(result& r, instruction_t &inst) {
	if (registers_.pc % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	uint8_t* encoding_ptr = reinterpret_cast<uint8_t*>(&heap_[registers_.pc / sizeof(uint64_t)]);
	uint8_t size = *encoding_ptr;

	uint16_t *op_ptr = reinterpret_cast<uint16_t*>(encoding_ptr + 1);
	inst.op = static_cast<op_codes>(*op_ptr);

	inst.size = static_cast<op_sizes>(static_cast<uint8_t>(*(encoding_ptr + 3)));
	inst.operands_count = static_cast<uint8_t>(*(encoding_ptr + 4));

	size_t offset = 5u;
	for (size_t i = 0u; i < inst.operands_count ; ++i) {
		inst.operands[i].type = static_cast<operand_types>(*(encoding_ptr + offset));
		++offset;

		inst.operands[i].index = *(encoding_ptr + offset);
		++offset;

		inst.operands[i].value = 0;
		if (inst.operands[i].type == operand_types::constant) {
			auto *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
			inst.operands[i].value = *constant_value_ptr;
			offset += sizeof(uint64_t);
		}
	}

	registers_.pc += size;

	return size;
}


bool terp::step(result &r) {
	instruction_t inst{};
	auto size = decode_instruction(r, inst);
	if (size == 0) {
		return false;
	}

	switch (inst.op) {
		case op_codes::nop:{
			fmt::print("nop\n");
		}break;
		case op_codes::load: {

		}break;
		case op_codes::store:{

		}break;
		case op_codes::move: {
			fmt::print("move\n");
			uint64_t source_value;
			if (!get_operand_value(r, inst, 0, source_value)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 1, source_value)){
				return false;
			}
		}break;
		case op_codes::push:{
			fmt::print("push\n");
		}break;
		case op_codes::pop:{
			fmt::print("pop\n");
		}break;
		case op_codes::add:{
			fmt::print("add\n");
			uint64_t lhs, rhs;
			if (!get_operand_value(r, inst, 1, lhs)) {
				return false;
			}
			if (!get_operand_value(r, inst, 1, rhs)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 0, lhs + rhs)){
				return false;
			}
		}break;
		case op_codes::sub:{

		}break;
		case op_codes::mul:{

		}break;
		case op_codes::div:{

		}break;
		case op_codes::mod:break;
		case op_codes::neg:break;
		case op_codes::shl:break;
		case op_codes::shr:break;
		case op_codes::rol:break;
		case op_codes::ror:break;
		case op_codes::and_op:break;
		case op_codes::or_op:break;
		case op_codes::xor_op:break;
		case op_codes::not_op:break;
		case op_codes::bis:break;
		case op_codes::bic:break;
		case op_codes::test:break;
		case op_codes::cmp:break;
		case op_codes::bz:break;
		case op_codes::bnz:break;
		case op_codes::tbz:break;
		case op_codes::tbnz:break;
		case op_codes::bne:break;
		case op_codes::beq:break;
		case op_codes::bae:break;
		case op_codes::ba:break;
		case op_codes::ble:break;
		case op_codes::bl:break;
		case op_codes::bo:break;
		case op_codes::bcc:break;
		case op_codes::bcs:break;
		case op_codes::jsr:break;
		case op_codes::ret:break;
		case op_codes::jmp:break;
		case op_codes::meta:break;
		case op_codes::debug:break;
		default:break;
	}

	return !r.is_failed();
}


bool terp::initialize(result& r) {
	heap_ = new uint64_t[heap_size_in_qwords()];

	registers_.pc = 0;
	registers_.fr = 0;
	registers_.sr = 0;
	registers_.sp = heap_size_in_qwords();
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.i[i] = 0;
		registers_.f[i] = 0.0;
	}
	return !r.is_failed();
}

uint64_t terp::pop() {
	uint64_t value = heap_[registers_.sp];
	registers_.sp += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value) {
	registers_.sp -= sizeof(uint64_t);
	heap_[registers_.sp] = value;
}

const register_file_t& terp::register_file() const {
	return registers_;
}

size_t terp::heap_size() const {
	return heap_size_;
}

size_t terp::heap_size_in_qwords() const {
	return heap_size_ / sizeof(uint64_t);
}

void terp::dump_heap(uint64_t address, size_t size) {
	auto pMemory = cfg::formatter::dump_to_string(heap_, size);
	fmt::print("{}\n",pMemory);
}

size_t terp::align(uint64_t addr, size_t size) {
	auto offset = addr % size;
	return offset ? addr + (size - offset) : addr;
}


bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, uint64_t& value) const {
	switch (inst.operands[operand_index].type) {
		case operand_types::register_integer:
			value = registers_.i[inst.operands[operand_index].index];
			break;
		case operand_types::register_floating_point:
			break;
		case operand_types::register_pc:
			value = registers_.pc;
			break;
		case operand_types::register_sp:
			value = registers_.sp;
			break;
		case operand_types::register_flags:
			value = registers_.fr;
			break;
		case operand_types::register_status:
			value = registers_.sr;
			break;
		case operand_types::constant:
			value = inst.operands[operand_index].value;
			break;
	}
	switch (inst.size) {
		case op_sizes::byte:{

		}break;
		case op_sizes::word:{

		}break;
		case op_sizes::dword:{

		}break;
		case op_sizes::qword:{

		}break;
		default:{
			r.add_message("B005","unsupported size.", true);
			return false;
		}
	}
	return true;
}

bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, double& value) const {
	switch (inst.operands[operand_index].type) {
		case operand_types::register_floating_point:
			value = registers_.f[inst.operands[operand_index].index];
			break;
		case operand_types::register_pc:
		case operand_types::register_sp:
		case operand_types::register_flags:
		case operand_types::register_status:
		case operand_types::register_integer:
			r.add_message("B010","", true);
			break;
		case operand_types::constant:
			break;
	}
	return true;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, uint64_t value) {
	switch (inst.operands[operand_index].type) {
		case operand_types::register_integer:
			registers_.i[inst.operands[operand_index].index] = value;
			return true;
		case operand_types::register_floating_point:
			r.add_message("B007",
						  "floating point register can not be the target of integer", true);
			return true;
		case operand_types::register_sp:
			return true;
		case operand_types::register_pc:
			return true;
		case operand_types::register_flags:
			return true;
		case operand_types::register_status:
			return true;
		case operand_types::constant:
			r.add_message("B007",
						  "constant can not be a operand type", true);
			break;
	}

	return false;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, double value) {
	switch (inst.operands[operand_index].type) {
		case operand_types::register_integer:
		case operand_types::register_sp:
		case operand_types::register_pc:
		case operand_types::register_flags:
		case operand_types::register_status:
			r.add_message("B007",
						  "integer register can not be the target of float", true);
			break;
		case operand_types::register_floating_point:
			registers_.i[inst.operands[operand_index].index] = value;
			return true;
		case operand_types::constant:
			r.add_message("B007", "constant can not be a operand type", true);
			break;
	}

	return false;
}


}