#include "terp.h"
#include <fmt/format.h>

void print_result(cfg::result& r){
	fmt::print("result message :{}", !r.is_failed());
	for (const auto &msg : r.messages()) {
		fmt::print("{} code: {}| message:{}\n",
				   msg.is_error() ? "ERROR": "",
				   msg.code(),
				   msg.message());
	}
}

int main() {
	cfg::terp terp(1024 * 1024 *32);
	cfg::result r;
	if (!terp.initialize(r)) {
		fmt::print("terp initialized failed.\n");
		print_result(r);
		return 1;
	}

	fmt::print("heap size: {}, {} qwords\n", terp.heap_size(), terp.heap_size_in_qwords());
	auto& regs = terp.register_file();
	auto stack_top = regs.sp;
	fmt::print("regs.sp = ${:08x}\n", regs.sp);
	terp.push(0x08);
	terp.push(0x04);
	terp.push(0x02);
	fmt::print("regs.sp = ${:08x}, number of entries: {}\n", regs.sp, (stack_top - regs.sp) / sizeof(uint64_t));

	auto t1 = terp.pop();
	auto t2 = terp.pop();
	auto t3 = terp.pop();
	fmt::print("t1 = ${:08x}\n", t1);
	fmt::print("t2 = ${:08x}\n", t2);
	fmt::print("t3 = ${:08x}\n", t3);

	uint64_t location_count = 0;
	uint8_t inst_size = 0u;
	cfg::instruction_t nop_inst;
	nop_inst.op = cfg::op_codes::nop;
	nop_inst.operands_count = 0;
	inst_size = terp.encode_instruction(r, location_count, nop_inst);
	if(inst_size == 0){
		print_result(r);
		return 1;
	}
	location_count += inst_size;

	cfg::instruction_t move_constant_to_reg3;
	move_constant_to_reg3.op = cfg::op_codes::move;
	move_constant_to_reg3.size = cfg::op_sizes::byte;
	move_constant_to_reg3.operands_count = 2;
	move_constant_to_reg3.operands[0].value = 0x7f;
	move_constant_to_reg3.operands[0].type = cfg::operand_types::constant;
	move_constant_to_reg3.operands[1].type = cfg::operand_types::register_integer;
	move_constant_to_reg3.operands[1].index = 3u;
	inst_size = terp.encode_instruction(r, location_count, move_constant_to_reg3);
	location_count += inst_size;

	cfg::instruction_t move_constant_to_reg4;
	move_constant_to_reg4.op = cfg::op_codes::move;
	move_constant_to_reg4.size = cfg::op_sizes::byte;
	move_constant_to_reg4.operands_count = 2;
	move_constant_to_reg4.operands[0].value = 0x7f;
	move_constant_to_reg4.operands[0].type = cfg::operand_types::constant;
	move_constant_to_reg4.operands[1].type = cfg::operand_types::register_integer;
	move_constant_to_reg4.operands[1].index = 4u;
	inst_size = terp.encode_instruction(r, location_count, move_constant_to_reg4);

	location_count += inst_size;

	cfg::instruction_t add_i3_to_i4;
	add_i3_to_i4.op = cfg::op_codes::add;
	add_i3_to_i4.size = cfg::op_sizes::byte;
	add_i3_to_i4.operands_count = 3;
	add_i3_to_i4.operands[0].index = 2u;
	add_i3_to_i4.operands[0].type = cfg::operand_types::register_integer;
	add_i3_to_i4.operands[1].index = 3u;
	add_i3_to_i4.operands[1].type = cfg::operand_types::register_integer;
	add_i3_to_i4.operands[2].index = 4u;
	add_i3_to_i4.operands[2].type = cfg::operand_types::register_integer;
	inst_size = terp.encode_instruction(r, location_count, add_i3_to_i4);


	terp.dump_heap(0);
	location_count += inst_size;
	for (int i = 0; i < 4 ; ++i) {
		if (!terp.step(r))
			print_result(r);
		terp.dump_state();
	}


	return 0;
}
