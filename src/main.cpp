
#include <fmt/format.h>
#include <chrono>
#include "common/ya_getopt.h"
#include "compiler/bytecode_emitter.h"
using namespace gfx;

static constexpr size_t heap_size = (1024 * 1024) * 32;
static constexpr size_t stack_size = (1024 * 1024) * 8;

static void print_results(result& r) {
	auto has_messages = !r.messages().empty();

	if (has_messages)
		fmt::print("\n");

	for (const auto& msg : r.messages()) {
		fmt::print("[{}] {}{}\n", msg.code(), msg.is_error() ? "ERROR: " : " ", msg.message());
		if (!msg.details().empty()) {
			fmt::print("{}\n", msg.details());
		}
	}

	if (has_messages)
		fmt::print("\n");
}

static void usage() {
	fmt::print("usage: bootstrap [-?|--help] [-v|--verbose] file\n");
}

int main(int argc, char** argv) {
	using namespace std::chrono;

	int opt = -1;
	bool help_flag = false;
	bool verbose_flag = false;
	std::filesystem::path ast_graph_file_name;

	static struct option long_options[] = {
		{"help",    ya_no_argument,       nullptr, 0  },
		{"verbose", ya_no_argument,       nullptr, 0  },
		{"ast",     ya_required_argument, 0,       'G'},
		{0,         0,                    0,       0  },
	};

	while (true) {
		int option_index = -1;
		opt = ya_getopt_long(
			argc,
			argv,
			"?:v:G:",
			long_options,
			&option_index);
		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 0: {
				switch (option_index) {
					case 0:
						help_flag = true;
						break;
					case 1:
						verbose_flag = true;
						break;
					case 2:
						ast_graph_file_name = ya_optarg;
						break;
					default:
						abort();
				}
				break;
			}
			case '?':
				help_flag = true;
				break;
			case 'v':
				verbose_flag = true;
				break;
			case 'G':
				ast_graph_file_name = ya_optarg;
				break;
			default:
				break;
		}
	}

	if (help_flag) {
		usage();
		return 1;
	}

	high_resolution_clock::time_point start = high_resolution_clock::now();
	bytecode_emitter_options_t compiler_options {
		.verbose = verbose_flag,
		.heap_size = heap_size,
		.stack_size = stack_size,
		.ast_graph_file_name = ast_graph_file_name,
	};
	bytecode_emitter compiler(compiler_options);
	result r;
	int rc = 0;

	if (!compiler.initialize(r)) {
		rc = 1;
	} else {
		std::vector<std::filesystem::path> source_files {};
		while (ya_optind < argc) {
			std::filesystem::path source_file_path(argv[ya_optind++]);
			source_files.push_back(source_file_path);
		}

		if (source_files.empty()) {
			usage();
			rc = 1;
		} else {
			if (!compiler.compile_files(r, source_files)) {
				rc = 1;
			} else {
				high_resolution_clock::time_point end = high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				fmt::print("Total compilation time (in μs): {}\n", duration);
			}
		}
	}

	print_results(r);

	high_resolution_clock::time_point end = high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	fmt::print("Total compilation time (in μs): {}\n", duration);

	return rc;
}