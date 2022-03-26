#include "terp.h"
#include <fmt/format.h>


int main() {
	cfg::terp terp(1024 * 1024 *32);
	cfg::result r;
	if (!terp.initialize(r)){
		fmt::print("terp initialized failed.\n");
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
	return 0;
}
