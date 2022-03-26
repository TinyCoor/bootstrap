#include "terp.h"
#include <fmt/format.h>


void print_result(cfg::result& r){
	fmt::print("result message :{}", !r.is_failed());
	for (const auto &msg : r.messages()) {
		fmt::print("{} code: {}| message:{}",
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
	uint8_t inst_size = 0;
	cfg::instruction_t nop_inst;
	nop_inst.op = cfg::op_codes::nop;
	nop_inst.operands_count = 0;
	inst_size = terp.encode_instruction(r, location_count, nop_inst);
	if(inst_size == 0){
		print_result(r);
		return 1;
	}
	location_count += inst_size;

	cfg::instruction_t move_constant_to_reg;
	move_constant_to_reg.op = cfg::op_codes::move;
	move_constant_to_reg.size = cfg::op_sizes::byte;
	move_constant_to_reg.operands_count = 2;
	move_constant_to_reg.operand[0].value = 0xff;
	move_constant_to_reg.operand[0].type = cfg::operand_types::constant;
	move_constant_to_reg.operand[1].type = cfg::operand_types::register_integer;
	move_constant_to_reg.operand[1].index = 3u;
	inst_size = terp.encode_instruction(r, location_count, move_constant_to_reg);
	if(inst_size == 0){
		print_result(r);
		return 1;
	}
	location_count += inst_size;

	terp.dump_heap(0);
	return 0;
}
