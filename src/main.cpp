#include "terp.h"
#include "instruction_emitter.h"
#include <fmt/format.h>
#include <chrono>
#include <functional>
#include <sstream>
#include "alpha_compiler.h"

static constexpr size_t heap_size = (1024 * 1024) * 32;

using test_function_callable = std::function<bool(gfx::result&, gfx::terp&)>;

void print_results(gfx::result& r) {
	for (const auto &msg : r.messages()) {
		fmt::print("\t|{}|{} {}\n", msg.code(), msg.is_error() ? "ERROR": "", msg.message());
	}
}

static bool run_terp(gfx::result& r, gfx::terp& terp)
{
	while (!terp.has_exited()) {
		if (!terp.step(r)) {
			return false;
		}
	}
	return true;
}

static bool test_fibonacci(gfx::result& r, gfx::terp& terp) {
	gfx::instruction_emitter bootstrap_emitter(terp.program_start);
	bootstrap_emitter.jump_direct(0);

	gfx::instruction_emitter fn_fibonacci(bootstrap_emitter.end_address());
	fn_fibonacci.load_stack_offset_to_register(gfx::op_sizes::dword, 0, 8);
	fn_fibonacci.push_int_register(gfx::op_sizes::dword, 0);
	fn_fibonacci.trap(1);

	fn_fibonacci.compare_int_register_to_constant(gfx::op_sizes::dword, 0, 0);
	fn_fibonacci.branch_if_equal(0);
	fn_fibonacci.compare_int_register_to_constant(gfx::op_sizes::dword, 0, 1);
	fn_fibonacci.branch_if_equal(0);
	fn_fibonacci.jump_direct(0);
	auto label_exit_fib = fn_fibonacci.end_address();
	fn_fibonacci[4].patch_branch_address(label_exit_fib);
	fn_fibonacci[6].patch_branch_address(label_exit_fib);

	fn_fibonacci.rts();

	auto label_next_fib = fn_fibonacci.end_address();
	fn_fibonacci[7].patch_branch_address(label_next_fib);

	fn_fibonacci.subtract_int_constant_from_register(gfx::op_sizes::dword, 1, 0, 1);
	fn_fibonacci.subtract_int_constant_from_register(gfx::op_sizes::dword, 2, 0, 2);
	fn_fibonacci.push_int_register(gfx::op_sizes::dword, 2);
	fn_fibonacci.jump_subroutine_direct(fn_fibonacci.start_address());
	fn_fibonacci.pop_int_register(gfx::op_sizes::dword, 2);
	fn_fibonacci.add_int_register_to_register(gfx::op_sizes::dword, 1, 1, 2);
	fn_fibonacci.push_int_register(gfx::op_sizes::dword, 1);
	fn_fibonacci.jump_subroutine_direct(fn_fibonacci.start_address());
	fn_fibonacci.pop_int_register(gfx::op_sizes::dword, 1);
	fn_fibonacci.store_register_to_stack_offset(gfx::op_sizes::dword, 1, 8);
	fn_fibonacci.rts();

	gfx::instruction_emitter main_emitter(fn_fibonacci.end_address());
	main_emitter.push_int_constant(gfx::op_sizes::dword, 100);
	main_emitter.jump_subroutine_direct(fn_fibonacci.start_address());
	main_emitter.dup();
	main_emitter.pop_int_register(gfx::op_sizes::dword, 0);
	main_emitter.trap(1);
	main_emitter.exit();

	bootstrap_emitter[0].patch_branch_address(main_emitter.start_address());
	bootstrap_emitter.encode(r, terp);
	fn_fibonacci.encode(r, terp);
	main_emitter.encode(r, terp);
	auto result =run_terp(r, terp);
	if (terp.register_file().i[0] != 1){
		r.add_message("T001", "fn_fibonacci should end with 0 or 1.", true);
	}
	return result;
}

static bool test_square(gfx::result& r, gfx::terp& terp)
{
	gfx::instruction_emitter bootstrap_emitter(terp.program_start);
	bootstrap_emitter.jump_direct(0);

	gfx::instruction_emitter fn_square_emitter(bootstrap_emitter.end_address());
	fn_square_emitter.load_stack_offset_to_register(gfx::op_sizes::dword, 0, 8);
	fn_square_emitter.multiply_int_register_to_register(gfx::op_sizes::dword, 0, 0, 0);
	fn_square_emitter.store_register_to_stack_offset(gfx::op_sizes::dword, 0, 8);
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
	if(r.is_failed()){
		return false;
	}
	auto res = run_terp(r, terp);
	if (terp.register_file().i[5] != 81) {
		r.add_message("T001", "test_square register 5 should 81", true);
	}
	if (terp.register_file().i[6] != 25) {
		r.add_message("T001", "test_square register 6 should 25", true);
	}
	return res;
}

static int time_test_function(gfx::result& r, gfx::terp& terp, const std::string& title,
							  const test_function_callable& test_function)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	terp.reset();
	auto rc = test_function(r, terp);
	// fmt::print("\nASSEMBLY LISTING:\n{}\n", terp.disassemble(r, terp.program_start));
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	fmt::print("function: {} {}\n", title, rc ? "SUCCESS" : "FAILED" );

	if(!rc || r.is_failed()){
		print_results(r);
	}

	fmt::print("execution time: {}\n\n",duration);

	return rc;
}

static int terp_test() {
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	gfx::terp terp((1024 * 1024) * 32);
	terp.register_trap(1, [](gfx::terp* t){
		auto value = t->pop();
		fmt::print("terp 1 value = {}\n", value);
	  return 0;
	});
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

static int compiler_tests() {
	gfx::alpha_compiler compiler(heap_size);
	gfx::result r;
	if (!compiler.initialize(r)) {
		print_results(r);
		return 1;
	}

	std::stringstream source(R"(// this is a test comment
					   // fibonacci sequence in basecode-alpha
					  @entry_point main;
						truth:bool := true;
						lies:bool := false;
						char:u8 := 'A';
						name:string := "this is a test string literal";
						name_ptr:*u8 := address_of(name);
						name_ptr := null;

					   fib := fn(n:u64):u64 {
					      if n == 0 || n == 1
					           n;
					       else"
					          fib((n - 1) + fib(n - 2));
					   }

					   main := fn():u64 {
					       fib(100);
					   })");
	if (!compiler.compile(r, source)) {
		print_results(r);
		return 1;
	}

	return 0;
}

int main() {
	int result =0;
	result = compiler_tests();
	if (result != 0) return result;

	result = terp_test();
	return result;
}
