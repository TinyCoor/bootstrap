#include "terp.h"
#include "extern/fmt/include/fmt/format.h"
#include "instruction_emitter.h"
#include <chrono>
#include <functional>

using test_function_callable = std::function<int(gfx::result&, gfx::terp&)>;

void print_results(gfx::result& r) {
	fmt::print("result message :{}\n", !r.is_failed());
	for (const auto &msg : r.messages()) {
		fmt::print("\t{} code: {} | message:{}\n",
				   msg.is_error() ? "ERROR": "",
				   msg.code(),
				   msg.message());
	}
}

static int test_fibonacci(gfx::result& r, gfx::terp& terp) {
	gfx::instruction_emitter bootstrap_emitter(0);
	bootstrap_emitter.jump_direct(0);

	gfx::instruction_emitter fn_fibonacci(bootstrap_emitter.end_address());
	fn_fibonacci.load_stack_offset_to_register(0, 8);
	fn_fibonacci.compare_int_register_to_constant(gfx::op_sizes::dword, 0, 0);
	fn_fibonacci.branch_if_equal(0);
	fn_fibonacci.compare_int_register_to_constant(gfx::op_sizes::dword, 0, 1);
	fn_fibonacci.branch_if_equal(0);
	fn_fibonacci.jump_direct(0);
	auto label_exit_fib = fn_fibonacci.end_address();
	fn_fibonacci[2].patch_branch_address(label_exit_fib);
	fn_fibonacci[4].patch_branch_address(label_exit_fib);

	fn_fibonacci.rts();

	auto label_next_fib = fn_fibonacci.end_address();
	fn_fibonacci[5].patch_branch_address(label_next_fib);

	fn_fibonacci.subtract_int_constant_from_register(gfx::op_sizes::dword, 1, 0, 1);
	fn_fibonacci.subtract_int_constant_from_register(gfx::op_sizes::dword, 2, 0, 2);
	fn_fibonacci.push_int_register(gfx::op_sizes::dword, 2);
	fn_fibonacci.jump_subroutine_direct(fn_fibonacci.start_address());
	fn_fibonacci.pop_int_register(gfx::op_sizes::dword, 2);
	fn_fibonacci.add_int_register_to_register(gfx::op_sizes::dword, 1, 1, 2);
	fn_fibonacci.push_int_register(gfx::op_sizes::dword, 1);
	fn_fibonacci.jump_subroutine_direct(fn_fibonacci.start_address());
	fn_fibonacci.pop_int_register(gfx::op_sizes::dword, 1);
	fn_fibonacci.store_register_to_stack_offset(1, 8);
	fn_fibonacci.rts();

	gfx::instruction_emitter main_emitter(fn_fibonacci.end_address());
	main_emitter.push_int_constant(gfx::op_sizes::dword, 100);
	main_emitter.jump_subroutine_direct(fn_fibonacci.start_address());
	main_emitter.pop_int_register(gfx::op_sizes::dword, 0);

	main_emitter.exit();

	bootstrap_emitter[0].patch_branch_address(main_emitter.start_address());
	bootstrap_emitter.encode(r, terp);
	fn_fibonacci.encode(r, terp);
	main_emitter.encode(r, terp);

	fmt::print("Disassembly:\n{}\n", terp.disassemble(r, 0));
	while (!terp.has_exited()) {
		if (!terp.step(r)) {
			print_results(r);
			return 1;
		}
	}
//	terp.dump_state(4);
	return 0;
}

static int test_square(gfx::result& r, gfx::terp& terp)
{
	gfx::instruction_emitter bootstrap_emitter(0);
	bootstrap_emitter.jump_direct(0);

	gfx::instruction_emitter fn_square_emitter(bootstrap_emitter.end_address());
	fn_square_emitter.load_stack_offset_to_register(0, 8);
	fn_square_emitter.multiply_int_register_to_register(gfx::op_sizes::dword, 0, 0, 0);
	fn_square_emitter.store_register_to_stack_offset(0, 8);
	fn_square_emitter.rts();

	gfx::instruction_emitter main_emitter(fn_square_emitter.end_address());
	main_emitter.push_int_constant(gfx::op_sizes::dword, 9);
	main_emitter.jump_subroutine_direct(fn_square_emitter.start_address());
	main_emitter.pop_int_register(gfx::op_sizes::dword, 5);
	main_emitter.push_int_constant(gfx::op_sizes::dword, 5);
	main_emitter.jump_subroutine_direct(fn_square_emitter.start_address());
	main_emitter.pop_int_register(gfx::op_sizes::dword, 6);
	main_emitter.exit();

	bootstrap_emitter[0].patch_branch_address(main_emitter.start_address());
	bootstrap_emitter.encode(r, terp);
	fn_square_emitter.encode(r, terp);
	main_emitter.encode(r, terp);

	if (r.is_failed()) {
		print_results(r);
		return 1;
	}

//	 terp.dump_heap(0);
//	 fmt::print("Disassembly:\n{}\n", terp.disassemble(r, 0));

	while (!terp.has_exited()) {
		if (!terp.step(r)) {
			print_results(r);
			return 1;
		}
	}
//	terp.dump_state();
	return 0;
}


static int time_test_function(gfx::result& r, gfx::terp& terp ,
							  const std::string& title,
							  const test_function_callable& test_function)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	terp.reset();

	auto rc = test_function(r, terp);

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	fmt::print("function: {} {}\n", title, !rc ? "SUCCESS" : "FAILED" );
	fmt::print("execution time: {}\n\n",duration);

	return rc;
}


int main() {
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	gfx::terp terp((1024 * 1024) * 32);
	gfx::result r;
	if (!terp.initialize(r)) {
		fmt::print("terp initialized failed.\n");
		print_results(r);
		return 1;
	}

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	fmt::print("execution time (in us): {}\n\n", duration);

	time_test_function(r, terp, "test_square" ,test_square);
	time_test_function(r, terp, "test_fibonacci", test_fibonacci);
	return 0;
}
