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

size_t terp::encode_instruction(result& r, uint64_t address, instruction_t instruction)
{
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	address /= sizeof(uint64_t);
	uint8_t size = 8;

	auto encoding_ptr = reinterpret_cast<uint8_t*>(heap_ + address);
	*encoding_ptr = size;

	auto op_ptr = reinterpret_cast<uint16_t*>(encoding_ptr + 1);
	*op_ptr = static_cast<uint16_t>(instruction.op);

	*(encoding_ptr + 3) = static_cast<uint8_t>(instruction.size);
	*(encoding_ptr + 4) = instruction.operands_count;

	///


	return size;
}

size_t terp::decode_instruction(result& r, instruction_t &inst) {
	if (registers_.pc % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	uint8_t* encoding_ptr = reinterpret_cast<uint8_t*>(&heap_[registers_.pc]);

	uint8_t size = *encoding_ptr;
	inst.op = static_cast<op_codes>(static_cast<uint16_t>(*(encoding_ptr + 1)));
	inst.size = static_cast<op_sizes>(static_cast<uint8_t>(*(encoding_ptr + 3)));
	inst.operands_count = static_cast<uint8_t>(*(encoding_ptr + 4));

	///
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
		case op_codes::nop: break;
		case op_codes::load:break;
		case op_codes::store:break;
		case op_codes::move:break;
		case op_codes::push:break;
		case op_codes::pop:break;
		case op_codes::add:break;
		case op_codes::sub:break;
		case op_codes::mul:break;
		case op_codes::div:break;
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

}