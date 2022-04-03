//
// Created by 12132 on 2022/3/26.
//

#include "terp.h"
#include "formatter.h"
#include <fmt/format.h>
#include <sstream>
#include <iomanip>

namespace gfx {

static inline uint64_t rotl(uint64_t n, uint8_t c)
{
	const unsigned int mask = (CHAR_BIT *sizeof(n) -1);
	c &= mask;
	return (n << c) | (n >>( (-c) & mask) );
}

static inline uint64_t rotr(uint64_t n, uint8_t c)
{
	const unsigned int mask = (CHAR_BIT *sizeof(n) -1);
	c &= mask;
	return (n >> c) | (n << ( (-c) & mask) );
}

terp::terp(size_t heap_size) : heap_size_(heap_size) {}

terp::~terp() {
	if (heap_) {
		delete heap_;
		heap_ = nullptr;
		heap_size_ = 0;
	}
}

bool terp::initialize(result& r)
{
	heap_ = new uint8_t[heap_size_];
	reset();
	return !r.is_failed();
}

void terp::reset()
{
	registers_.pc = program_start;
	registers_.fr = 0;
	registers_.sr = 0;
	registers_.sp = heap_size_;
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.i[i] = 0;
		registers_.f[i] = 0.0;
	}
	exited_ = false;
}

void terp::dump_state(uint8_t count) {
	fmt::print("-------------------------------------------------------------\n");
	fmt::print("PC =${:08x} | SP =${:08x} | FR =${:08x} | SR =${:08x}\n",
			   registers_.pc, registers_.sp, registers_.fr, registers_.sr);
	fmt::print("-------------------------------------------------------------\n");
	for (int index = 0; index < count ; ++index) {
		fmt::print("I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x}\n",
			index, registers_.i[index],
			index + 1, registers_.i[index + 1],
			index + 2, registers_.i[index + 2],
			index + 3, registers_.i[index + 3]);
		index += 4;
	}
	fmt::print("-------------------------------------------------------------\n");
	for (int index = 0; index < count ; ++index) {
		fmt::print("F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x}\n",
				   index, static_cast<uint64_t >(registers_.f[index]),
				   index + 1, static_cast<uint64_t >(registers_.f[index + 1]),
				   index + 2, static_cast<uint64_t >(registers_.f[index + 2]),
				   index + 3, static_cast<uint64_t >(registers_.f[index + 3]));
		index += 4;
	}

}

/// todo move to a function table
bool terp::step(result &r)
{
	instruction_t inst{};
	auto size = inst.decode(r, heap_, registers_.pc);
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
		}break;
		case op_codes::store: {
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
		case op_codes::inc: {
			registers_.i[inst.operands[0].index]++;
		}break;
		case op_codes::dec: {
			registers_.i[inst.operands[0].index]--;

		}break;
		case op_codes::copy:{
			uint64_t source_address, target_address;
			if (!get_operand_value(r, inst, 0, source_address))
				return false;
			if (!get_operand_value(r, inst, 0, target_address))
				return false;
			uint64_t  length ;
			if (!get_operand_value(r, inst, 2, length))
				return false;
			memcpy(heap_ + target_address, heap_ + source_address, length * op_size_in_bytes(inst.size));

		}break;
		case op_codes ::fill:{
			uint64_t value;
			if (!get_operand_value(r, inst, 0, value))
				return false;

			uint64_t address;
			if (!get_operand_value(r, inst, 1, address))
				return false;

			uint64_t  length ;
			if (!get_operand_value(r, inst, 2, length))
				return false;

			length *= op_size_in_bytes(inst.size);

			switch (inst.size) {
				case op_sizes::byte:
					memset(heap_ + address, static_cast<uint8_t>(value), length);
					break;
				case op_sizes::word:
					memset(heap_ + address, static_cast<uint16_t>(value), length);
					break;
				case op_sizes::dword:
					memset(heap_ + address, static_cast<uint32_t>(value), length);
					break;
				case op_sizes::qword:length *= sizeof(uint64_t);
					memset(heap_ + address, value, length);
					break;
				default:
					//// error
					break;
			}
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
		case op_codes::dup: {
			push(peek());

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
			uint64_t value;
			if (!get_operand_value(r, inst, 1, value))
				return false;
			int64_t negate_res = -static_cast<int64_t>(value);
			if (!set_target_operand_value(r, inst, 0, static_cast<uint64_t>(negate_res)))
				return false;
		}break;
		case op_codes::shl:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value << rhs_value))
				return false;
		}break;
		case op_codes::shr: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value >> rhs_value))
				return false;
		}break;
		case op_codes::rol:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			uint64_t left_rotate_value =rotl(lhs_value, static_cast<uint8_t>(rhs_value));
			if (!set_target_operand_value(r, inst, 0, left_rotate_value))
				return false;
		}break;
		case op_codes::ror:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			uint64_t right_rotate_value = rotr(lhs_value, static_cast<uint8_t>(rhs_value));
			if (!set_target_operand_value(r, inst, 0, right_rotate_value))
				return false;
		}break;
		case op_codes::and_op:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value & rhs_value))
				return false;
		} break;
		case op_codes::or_op:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value | rhs_value))
				return false;
		} break;
		case op_codes::xor_op: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			if (!set_target_operand_value(r, inst, 0, lhs_value ^ rhs_value))
				return false;
		} break;
		case op_codes::not_op:{
			uint64_t value;
			if (!get_operand_value(r, inst, 1, value))
				return false;
			uint64_t not_res = ~value;
			if (!set_target_operand_value(r, inst, 0, not_res))
				return false;
		}break;
		case op_codes::bis:{
			uint64_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value))
				return false;
			if (!get_operand_value(r, inst, 2, bit_number))
				return false;
			uint64_t masked_value =static_cast<uint64_t > (1 << bit_number);
			if (!set_target_operand_value(r, inst, 0, value | masked_value))
				return false;
		}break;
		case op_codes::bic:{
			uint64_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value))
				return false;
			if (!get_operand_value(r, inst, 2, bit_number))
				return false;
			uint64_t masked_value = static_cast<uint64_t>(~(1 << bit_number));
			if (!set_target_operand_value(r, inst, 0, value & masked_value))
				return false;
		}break;
		case op_codes::test:{
			uint64_t value, mask;
			if (!get_operand_value(r, inst, 1, value))
				return false;
			if (!get_operand_value(r, inst, 2, mask))
				return false;
			registers_.flags(register_file_t::zero, (value & mask) != 0);
		}break;
		case op_codes::cmp: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 0, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 1, rhs_value))
				return false;
			uint64_t result = lhs_value - rhs_value;
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::overflow, rhs_value > lhs_value);

		}break;
		case op_codes::bz:{
			uint64_t value, address;
			if (!get_operand_value(r, inst, 0, value))
				return false;
			if (!get_operand_value(r, inst, 1, address))
				return false;
			if (value == 0){
				registers_.pc = address;
			}

		}break;
		case op_codes::bnz: {
			uint64_t value, address;
			if (!get_operand_value(r, inst, 0, value))
				return false;
			if (!get_operand_value(r, inst, 1, address))
				return false;
			if (value != 0) {
				registers_.pc = address;
			}
		}break;
		case op_codes::tbz: {
			uint64_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value))
				return false;
			if (!get_operand_value(r, inst, 1, mask))
				return false;
			if (!get_operand_value(r, inst, 2, address))
				return false;
			if ((value & mask) == 0) {
				registers_.pc = address;
			}

		} break;
		case op_codes::tbnz: {
			uint64_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value))
				return false;
			if (!get_operand_value(r, inst, 1, mask))
				return false;
			if (!get_operand_value(r, inst, 2, address))
				return false;
			if ((value & mask) != 0) {
				registers_.pc = address;
			}
		}break;
		case op_codes::bne: {
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address))
				return false;
			if (registers_.flags(register_file_t::flags_t::zero) == 0){
				// registers_.flags(register_file_t::zero, false);
				registers_.pc = address;
			}
		}break;
		case op_codes::beq: {
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address))
				return false;
			if (registers_.flags(register_file_t::flags_t::zero) != 0){
				registers_.flags(register_file_t::zero, false);
				registers_.pc = address;
			}
		} break;
		case op_codes::bg: {

			break;
		}
		case op_codes::bge: {
			break;
		}
		case op_codes::ble:{
			break;
		}
		case op_codes::bl: {
			break;
		}

		case op_codes::jsr: {
			push(registers_.pc);
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address))
				return false;
			registers_.pc = address;
		}break;
		case op_codes::rts: {
			uint64_t address = pop();
			registers_.pc = address;
		}break;
		case op_codes::jmp: {
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address))
				return false;
			registers_.pc = address;
		}break;
		case op_codes::swi:{
			uint64_t index;
			if (!get_operand_value(r, inst, 0, index))
				return false;
			size_t swi_offset = sizeof(uint64_t) * index;
			uint64_t swi_address = *qword_ptr(swi_offset);
			if (swi_address != 0) {
				/// TODO 恢复现场
				push(registers_.pc);
				registers_.pc = swi_address;
			}
		}break;
		case op_codes::trap:{
			uint64_t index;
			if (!get_operand_value(r, inst, 0, index))
				return false;
			auto it = traps_.find(static_cast<uint8_t>(index));
			if (it== traps_.end()){
				break;
			}
			it->second(this);
		}break;
		case op_codes::meta: {

		}break;

		case op_codes::exit: {
			exited_ = true;
			break;
		}
		default:break;
	}

	return !r.is_failed();
}

uint64_t terp::pop()
{
	uint64_t value = *qword_ptr(registers_.sp);
	registers_.sp += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value)
{
	registers_.sp -= sizeof(uint64_t);
	*qword_ptr(registers_.sp) = value;
}

void terp::dump_heap(uint64_t offset, size_t size)
{
	auto pMemory = formatter::Hex((void*)(heap_ + offset), size);
	fmt::print("{}\n",pMemory);
}

bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, uint64_t& value) const
{
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
		case operand_types::register_pc:  {value = registers_.pc;break;}
		case operand_types::register_flags: {value = registers_.fr;break;}
		case operand_types::register_status: {value = registers_.sr;break;}
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post:
		case operand_types::constant_integer: {value = inst.operands[operand_index].value.u64;break;}
		case operand_types::constant_float: {value = static_cast<uint64_t>(inst.operands[operand_index].value.d64);break;}
	}

	return true;
}

bool terp::get_operand_value(result& r, const instruction_t& inst, uint8_t operand_index, double& value) const
{
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

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, uint64_t value)
{
	switch (inst.operands[operand_index].type) {
		case operand_types::increment_register_pre:
		case operand_types::decrement_register_pre:
		case operand_types::increment_register_post:
		case operand_types::decrement_register_post:
		case operand_types::register_integer: {
			registers_.i[inst.operands[operand_index].index] = value;
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			return true;
		}
		case operand_types::register_floating_point: {
			registers_.f[inst.operands[operand_index].index] = value;
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			break;
		}
		case operand_types::register_sp: {
			registers_.sp = value;break;
		}
		case operand_types::register_pc: {
			registers_.pc = value;break;
		}
		case operand_types::register_flags: {
			registers_.fr = value;break;
		}
		case operand_types::register_status: {
			registers_.sr = value;break;
		}
		case operand_types::constant_float:
		case operand_types::constant_integer:
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post: {
			r.add_message( "B006","constant cannot be a target operand type.", true);
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
			uint64_t integer_value = static_cast<uint64_t>(value);
			registers_.i[inst.operands[operand_index].index] = integer_value;
			registers_.flags(register_file_t::flags_t::zero, integer_value == 0);
			break;
		}
		case operand_types::register_floating_point: {
			registers_.f[inst.operands[operand_index].index] = value;
			break;
		}
		case operand_types::register_sp: {
			registers_.sp = value;
			break;
		}
		case operand_types::register_pc: {
			registers_.pc = value;
			break;
		}
		case operand_types::register_flags: {
			registers_.fr = value;break;
		}
		case operand_types::register_status: {
			registers_.sr = value;break;
		}
		case operand_types::constant_float:
		case operand_types::constant_integer:
		case operand_types::increment_constant_pre:
		case operand_types::decrement_constant_pre:
		case operand_types::increment_constant_post:
		case operand_types::decrement_constant_post: {
			r.add_message("B006", "constant cannot be a target operand type.", true);
			break;
		}
	}

	return false;
}

std::string terp::disassemble(result &r, uint64_t address)
{
	std::stringstream stream;
	while (true) {
		instruction_t inst;
		auto inst_size = inst.decode(r, heap_, address);
		if (inst_size == 0)
			break;

		stream << fmt::format("${:08X}: ", address)
			   << disassemble(inst) << "\n";

		if (inst.op == op_codes::exit)
			break;

		address += inst_size;
	}
	return std::move(stream.str());
}

std::string terp::disassemble(const instruction_t &inst) const
{
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
	return std::move(stream.str());
}

void terp::register_trap(uint8_t index, const terp::trap_callable &callable)
{
	traps_.insert(std::make_pair(index, callable));
}

void terp::remove_trap(uint8_t index)
{
	traps_.erase(index);
}

uint64_t terp::peek() const
{
	uint64_t value = *qword_ptr(registers_.sp);
	return value;
}

void terp::swi(uint8_t index, uint64_t address)
{
	size_t swi_address = sizeof(uint64_t) * index;
	*qword_ptr(swi_address) = address;
}

}