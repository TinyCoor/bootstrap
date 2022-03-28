//
// Created by 12132 on 2022/3/26.
//

#include "terp.h"
#include "formatter.h"
#include "extern/fmt/include/fmt/format.h"
#include <sstream>
#include <iomanip>

namespace gfx {

terp::terp(size_t heap_size) : heap_size_(heap_size)
{
}

terp::~terp() {
	if (heap_) {
		delete heap_;
		heap_ = nullptr;
		heap_size_ = 0;
	}
}

bool terp::initialize(result& r) {
	heap_ = new uint8_t[heap_size_];
	reset();
	return !r.is_failed();
}

void terp::reset() {
	registers_.pc = 0;
	registers_.fr = 0;
	registers_.sr = 0;
	registers_.sp = heap_size_;
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.i[i] = 0;
		registers_.f[i] = 0.0;
	}
	exited_ = false;
}

void terp::dump_state() {
	fmt::print("cfg interpreter state: -----------------------------------------------\n");
	for (int index = 0; index < 16 ; ++index) {
		fmt::print("I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x}\n",
			index, registers_.i[index],
			index + 1, registers_.i[index + 1],
			index + 2, registers_.i[index + 2],
			index + 3, registers_.i[index + 3]);
		index += 4;
	}

	fmt::print("-----------------------------------------------------------\n");
	for (int index = 0; index < 16 ; ++index) {
		fmt::print("F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x}\n",
				   index, static_cast<uint64_t >(registers_.f[index]),
				   index + 1, static_cast<uint64_t >(registers_.f[index + 1]),
				   index + 2, static_cast<uint64_t >(registers_.f[index + 2]),
				   index + 3, static_cast<uint64_t >(registers_.f[index + 3]));
		index += 4;
	}
	fmt::print("-------------------------------------------------------------\n");
	fmt::print("PC =${:08x} | SP =${:08x} | FR =${:08x} | SR =${:08x}\n\n",
			   registers_.pc, registers_.sp, registers_.fr, registers_.sr);
}

size_t terp::encode_instruction(result& r, uint64_t address, instruction_t instruction)
{
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	uint8_t size = 5;

	auto* op_ptr = word_ptr(address + 1);
	*op_ptr = static_cast<uint16_t>(instruction.op);

	auto encoding_ptr = byte_ptr(address);
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
		switch (instruction.operands[i].type) {
			case operand_types::register_sp:
			case operand_types::register_pc:
			case operand_types::register_flags:
			case operand_types::register_status:
			case operand_types::register_integer:
			case operand_types::increment_register_pre:
			case operand_types::decrement_register_pre:
			case operand_types::register_floating_point:
			case operand_types::increment_register_post:
			case operand_types::decrement_register_post: break;
			case operand_types::increment_constant_pre:
			case operand_types::increment_constant_post:
			case operand_types::decrement_constant_pre:
			case operand_types::decrement_constant_post:
			case operand_types::constant_integer: {
				uint64_t *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
				*constant_value_ptr = instruction.operands[i].value.u64;
				offset += sizeof(uint64_t);
				size += sizeof(uint64_t);
				break;
			}
			case operand_types::constant_float: {
				double *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
				*constant_value_ptr = instruction.operands[i].value.d64;
				offset += sizeof(double);
				size += sizeof(double);
				break;
			}
		}
	}

	if (size < 8)
		size = 8;

	size = static_cast<uint8_t>(align(size, sizeof(uint64_t)));
	*encoding_ptr = size;

	return size;
}

size_t terp::decode_instruction(result& r, uint64_t address, instruction_t &inst) {
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}

	uint16_t *op_ptr = word_ptr(address + 1);
	inst.op = static_cast<op_codes>(*op_ptr);

	uint8_t* encoding_ptr = byte_ptr(address);
	uint8_t size = *encoding_ptr;
	inst.size = static_cast<op_sizes>(static_cast<uint8_t>(*(encoding_ptr + 3)));
	inst.operands_count = static_cast<uint8_t>(*(encoding_ptr + 4));

	size_t offset = 5u;
	for (size_t i = 0u; i < inst.operands_count ; ++i) {
		inst.operands[i].type = static_cast<operand_types>(*(encoding_ptr + offset));
		++offset;

		inst.operands[i].index = *(encoding_ptr + offset);
		++offset;

		switch (inst.operands[i].type) {
			case operand_types::register_sp:
			case operand_types::register_pc:
			case operand_types::register_flags:
			case operand_types::register_status:
			case operand_types::register_integer:
			case operand_types::increment_register_pre:
			case operand_types::decrement_register_pre:
			case operand_types::increment_register_post:
			case operand_types::decrement_register_post:
			case operand_types::register_floating_point: break;
			case operand_types::increment_constant_pre:
			case operand_types::decrement_constant_pre:
			case operand_types::increment_constant_post:
			case operand_types::decrement_constant_post:
			case operand_types::constant_integer: {
				uint64_t *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
				inst.operands[i].value.u64 = *constant_value_ptr;
				offset += sizeof(uint64_t);
				break;
			}
			case operand_types::constant_float: {
				double *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
				inst.operands[i].value.d64 = *constant_value_ptr;
				offset += sizeof(double);
				break;
			}
		}
	}

	return size;
}

bool terp::step(result &r) {
	instruction_t inst{};
	auto size = decode_instruction(r, registers_.pc, inst);
	if (size == 0) {
		return false;
	}
	registers_.pc += size;

	switch (inst.op) {
		case op_codes::nop:{
		}break;
		case op_codes::load: {
			uint64_t address;
			if (!get_operand_value(r, inst, 1, address))
				return false;
			if (inst.operands_count > 2) {
				uint64_t offset;
				if (!get_operand_value(r, inst, 2, offset))
					return false;
				address += offset;
			}
			uint64_t value = *qword_ptr(address);
			if (!set_target_operand_value(r, inst, 0, value))
				return false;
			break;
		}
		case op_codes::inc: {
			registers_.i[inst.operands[0].index]++;
			break;
		}
		case op_codes::dec: {
			registers_.i[inst.operands[0].index]--;
			break;
		}
		case op_codes::copy:{}break;
		case op_codes ::fill:{}break;
		case op_codes::store:{
			uint64_t value;
			if (!get_operand_value(r, inst, 0, value))
				return false;

			uint64_t address;
			if (!get_operand_value(r, inst, 1, address))
				return false;
			if (inst.operands_count > 2) {
				uint64_t offset;
				if (!get_operand_value(r, inst, 2, offset))
					return false;
				address += offset;
			}
			*qword_ptr(address) = value;
		}break;
		case op_codes::move: {
			uint64_t source_value;
			if (!get_operand_value(r, inst, 0, source_value)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 1, source_value)){
				return false;
			}
		}break;
		case op_codes::push:{
			uint64_t value;
			if (!get_operand_value(r, inst, 0, value)){
				return false;
			}
			push(value);
		}break;
		case op_codes::pop:{
			uint64_t value = pop();
			if (!set_target_operand_value(r, inst, 0, value)){
				return false;
			}
		}break;
		case op_codes::add:{
			uint64_t lhs, rhs;
			if (!get_operand_value(r, inst, 1, lhs)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 0, lhs + rhs)){
				return false;
			}
		}break;
		case op_codes::sub:{
			uint64_t lhs, rhs;
			if (!get_operand_value(r, inst, 1, lhs)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 0, lhs - rhs)){
				return false;
			}
		}break;
		case op_codes::mul:{
			uint64_t lhs, rhs;
			if (!get_operand_value(r, inst, 1, lhs)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 0, lhs * rhs)){
				return false;
			}
		}break;
		case op_codes::div:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			uint64_t result = 0;
			if (rhs_value != 0)
				result = lhs_value / rhs_value;
			if (!set_target_operand_value(r, inst, 0, result))
				return false;
		}break;
		case op_codes::mod:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value % rhs_value))
				return false;
		}break;
		case op_codes::neg:{

		}break;
		case op_codes::shl:{
			break;
		}
		case op_codes::shr:{
			break;
		}
		case op_codes::rol:{
			break;
		}
		case op_codes::ror:{
			break;
		}
		case op_codes::and_op:{
			break;
		}
		case op_codes::or_op:{
			break;
		}
		case op_codes::xor_op:{
			break;
		}
		case op_codes::not_op:{
			break;
		}
		case op_codes::bis:{
			break;
		}
		case op_codes::bic:{
			break;
		}
		case op_codes::test:{
			break;
		}
		case op_codes::cmp:{
			break;
		}
		case op_codes::bz:{
			break;
		}
		case op_codes::bnz:{
			break;
		}
		case op_codes::tbz:{
			break;
		}
		case op_codes::tbnz:{
			break;
		}
		case op_codes::bne:{
			break;
		}
		case op_codes::beq:{
			break;
		}
		case op_codes::bae:{
			break;
		}
		case op_codes::ba:{
			break;
		}
		case op_codes::ble:{
			break;
		}
		case op_codes::bl: {
			break;
		}
		case op_codes::bo: {
			break;
		}
		case op_codes::bcc: {
			break;
		}
		case op_codes::bcs: {

		}break;
		case op_codes::jsr: {

		}break;
		case op_codes::rts: {

		}break;
		case op_codes::jmp:{
		}break;
		case op_codes::meta:{
		}break;
		case op_codes::debug:{

		}break;
		case op_codes::exit: {
			exited_ = true;
			break;
		}
		default:break;
	}

	return !r.is_failed();
}

uint64_t terp::pop() {
	uint64_t value = *qword_ptr(registers_.sp);
	registers_.sp += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value) {
	registers_.sp -= sizeof(uint64_t);
	*qword_ptr(registers_.sp) = value;
}

void terp::dump_heap(uint64_t offset, size_t size) {
	auto pMemory = formatter::Hex((void*)(heap_ + offset), size);
	fmt::print("{}\n",pMemory);
}

size_t terp::align(uint64_t addr, size_t size) {
	auto offset = addr % size;
	return offset ? addr + (size - offset) : addr;
}

bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, uint64_t& value) const {
	switch (inst.operands[operand_index].type) {
		case operand_types::increment_register_pre:
		case operand_types::decrement_register_pre:
		case operand_types::increment_register_post:
		case operand_types::decrement_register_post:
		case operand_types::register_integer: {
			value = registers_.i[inst.operands[operand_index].index];
			break;
		}
		case operand_types::register_floating_point: {
			value = static_cast<uint64_t>(registers_.f[inst.operands[operand_index].index]);
			break;
		}
		case operand_types::register_sp: {value = registers_.sp;break;}
		case operand_types::register_pc: {value = registers_.pc;break;}
		case operand_types::register_flags: {value = registers_.fr;break;}
		case operand_types::register_status: {value = registers_.sr;break;}
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post:
		case operand_types::constant_integer: {value = inst.operands[operand_index].value.u64;break;}
		case operand_types::constant_float: {value = static_cast<uint64_t>(inst.operands[operand_index].value.d64);break;}
	}

	// XXX: need to implement zero extend
	switch (inst.size) {
		case op_sizes::byte:
			break;
		case op_sizes::word:
			break;
		case op_sizes::dword:
			break;
		case op_sizes::qword:
			break;
		default: {
			r.add_message(
				"B005",
				"unsupported size of 'none' for operand.",
				true);
			return false;
		}
	}

	return true;
}

bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, double& value) const {
	switch (inst.operands[operand_index].type) {
		case operand_types::increment_register_pre:
		case operand_types::increment_register_post:
		case operand_types::decrement_register_post:
		case operand_types::decrement_register_pre:
		case operand_types::register_floating_point:{
			value = registers_.f[inst.operands[operand_index].index];
		}break;

		case operand_types::register_pc:
		case operand_types::register_sp:
		case operand_types::register_flags:
		case operand_types::register_status:
		case operand_types::register_integer:{
			r.add_message(
				"B005",
				"integer registers cannot be used for floating point operands.",
				true);
		}break;

		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post:
		case operand_types::constant_integer:
			value = inst.operands[operand_index].value.u64;
		case operand_types::constant_float:
			value = inst.operands[operand_index].value.d64;
			break;
	}
	return true;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, uint64_t value) {
	switch (inst.operands[operand_index].type) {
		case operand_types::increment_register_pre:
		case operand_types::decrement_register_pre:
		case operand_types::increment_register_post:
		case operand_types::decrement_register_post:
		case operand_types::register_integer: {
			registers_.i[inst.operands[operand_index].index] = value;
			return true;
		}
		case operand_types::register_floating_point: {registers_.f[inst.operands[operand_index].index] = value;break;}
		case operand_types::register_sp: {registers_.sp = value;break;}
		case operand_types::register_pc: {registers_.pc = value;break;}
		case operand_types::register_flags: {registers_.fr = value;break;}
		case operand_types::register_status: {registers_.sr = value;break;}
		case operand_types::constant_float:
		case operand_types::constant_integer:
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post: {
			r.add_message(
				"B006",
				"constant cannot be a target operand type.",
				true);
			break;
		}
	}
	return false;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, double value) {
	switch (inst.operands[operand_index].type) {
		case operand_types::increment_register_pre:
		case operand_types::decrement_register_pre:
		case operand_types::increment_register_post:
		case operand_types::decrement_register_post:
		case operand_types::register_integer: {
			registers_.i[inst.operands[operand_index].index] = value;
			return true;
		}
		case operand_types::register_floating_point: {registers_.f[inst.operands[operand_index].index] = value;break;}
		case operand_types::register_sp: { registers_.sp = value;break;}
		case operand_types::register_pc: { registers_.pc = value;break;}
		case operand_types::register_flags: { registers_.fr = value;break;}
		case operand_types::register_status: { registers_.sr = value;break;}
		case operand_types::constant_float:
		case operand_types::constant_integer:
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post: {
			r.add_message(
				"B006",
				"constant cannot be a target operand type.",
				true);
			break;
		}
	}

	return false;
}

std::string terp::disassemble(result &r, uint64_t address) {
	std::stringstream stream;
	while (true) {
		instruction_t inst;
		auto inst_size = decode_instruction(r, address, inst);
		if (inst_size == 0)
			break;

		stream << fmt::format("${:08X}: ", address)
			   << disassemble(inst) << "\n";

		if (inst.op == op_codes::exit)
			break;

		address += inst_size;
	}
	return stream.str();
}

std::string terp::disassemble(const instruction_t &inst) const {
	std::stringstream stream;
	auto it = s_op_code_names.find(inst.op);
	if (it != s_op_code_names.end()) {
		std::stringstream mnemonic;
		mnemonic <<  it->second;
		switch (inst.size) {
			case op_sizes::byte:
				mnemonic << ".B";
				break;
			case op_sizes::word:
				mnemonic << ".W";
				break;
			case op_sizes::dword:
				mnemonic << ".DW";
				break;
			case op_sizes::qword:
				mnemonic << ".QW";
				break;
			default: {
				break;
			}
		}

		stream << std::left << std::setw(10) << mnemonic.str();

		for (size_t i = 0; i < inst.operands_count; i++) {
			if (i > 0 && i < inst.operands_count) {
				stream << ", ";
			}
			switch (inst.operands[i].type) {
				case operand_types::register_integer:
					stream << "I" << std::to_string(inst.operands[i].index);
					break;
				case operand_types::register_floating_point:
					stream << "F" << std::to_string(inst.operands[i].index);
					break;
				case operand_types::register_sp:
					stream << "SP";
					break;
				case operand_types::register_pc:
					stream << "PC";
					break;
				case operand_types::register_flags:
					stream << "FR";
					break;
				case operand_types::register_status:
					stream << "SR";
					break;
				case operand_types::constant_integer:
					stream << fmt::format("#${:08X}", inst.operands[i].value.u64);
					break;
				case operand_types::constant_float:
					stream << fmt::format("#${:08X}", static_cast<uint64_t>(inst.operands[i].value.d64));
					break;
				case operand_types::increment_constant_pre:
					stream << "++" << std::to_string(inst.operands[i].value.u64);
					break;
				case operand_types::increment_constant_post:
					stream << std::to_string(inst.operands[i].value.u64) << "++";
					break;
				case operand_types::increment_register_pre:
					stream << "++" << "I" << std::to_string(inst.operands[i].index);
					break;
				case operand_types::increment_register_post:
					stream << "I" << std::to_string(inst.operands[i].index) << "++";
					break;
				case operand_types::decrement_constant_pre:
					stream << "--" << std::to_string(inst.operands[i].value.u64);
					break;
				case operand_types::decrement_constant_post:
					stream << std::to_string(inst.operands[i].value.u64) << "--";
					break;
				case operand_types::decrement_register_pre:
					stream << "--" << "I" << std::to_string(inst.operands[i].index);
					break;
				case operand_types::decrement_register_post:
					stream << "I" << std::to_string(inst.operands[i].index) << "--";
					break;
			}
		}
	} else {
		stream << "UNKNOWN";
	}
	return stream.str();
}

}