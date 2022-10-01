
#include "fmt/format.h"
#include <chrono>
#include <filesystem>
#include "common/ya_getopt.h"
#include "common/result.h"
#include "common/string_extension.h"
#include "core/compiler/session.h"
using namespace gfx;

static constexpr size_t heap_size = (1024 * 1024) * 32;
static constexpr size_t stack_size = (1024 * 1024) * 8;

static void print_results(result& r)
{
	auto has_messages = !r.messages().empty();

	if (has_messages) {
		fmt::print("\n");
	}

	for (const auto& msg : r.messages()) {
		fmt::print("[{}] {}{}\n", msg.code(), msg.is_error() ? "ERROR: " : "WARNING: ", msg.message());
		if (!msg.details().empty()) {
			fmt::print("{}\n", msg.details());
		}
	}

	if (has_messages) {
		fmt::print("\n");
	}
}

static void usage()
{
	constexpr std::string_view fmt_args = R"(usage: bootstrap
			   	[-?|--help]
				[-v|--verbose]
   				[-G{{filename}}|--ast={{filename}}]
			   	[-H{{filename}}|--code_dom={{filename}}]
				file\n)";
	fmt::print(fmt::runtime(fmt_args));
}

int main(int argc, char** argv) {
	using namespace std::chrono;
	int opt = -1;
	bool help_flag = false;
	bool verbose_flag = false;
	std::filesystem::path ast_graph_file_name;
	std::filesystem::path code_dom_graph_file_name;
    std::unordered_map<std::string, std::string> definitions {};

    /// todo Clara
	static struct option long_options[] = {
		{"help",    ya_no_argument,       nullptr, 0  },
		{"verbose", ya_no_argument,       nullptr, 0  },
		{"ast",     ya_required_argument, 0,       'G'},
		{"code_dom",ya_required_argument, 0,       'H'},
		{0,         0,                    0,       0  },
	};

	while (true) {
		int option_index = -1;
		opt = ya_getopt_long(argc, argv, "?vGH:D:", long_options, &option_index);
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
					case 3:
						code_dom_graph_file_name = ya_optarg;
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
				ast_graph_file_name = std::filesystem::path(ya_optarg);
				break;
			case 'H':
				code_dom_graph_file_name = std::filesystem::path(ya_optarg);
				break;
            case 'D': {
                auto parts = string_to_list(ya_optarg, '=');
                std::string value;
                if (parts.size() == 2) {
                    value = parts[1];
                }
                trim(parts[0]);
                definitions.insert(std::make_pair(parts[0], value));
                break;
            }
			default:
				break;
		}
	}

	if (help_flag) {
		usage();
		return 1;
	}
	verbose_flag = true;

	high_resolution_clock::time_point start = high_resolution_clock::now();
    std::vector<std::filesystem::path> source_files {};
    while (ya_optind < argc) {
        std::filesystem::path source_file_path(argv[ya_optind++]);
        source_files.push_back(source_file_path);
    }
    if (source_files.empty()) {
        usage();
        return 1;
    }
	result r;
    compiler::session_options_t session_options {
        .verbose = verbose_flag,
        .heap_size = heap_size,
        .stack_size = stack_size,
        .full_path = std::filesystem::absolute(argv[0]).remove_filename(),
        .ast_graph_file = ast_graph_file_name,
        .dom_graph_file = code_dom_graph_file_name,
        .compile_callback = [&](gfx::compiler::session_compile_phase_t phase,
            const std::filesystem::path &source_file) {
            switch (phase) {
              case gfx::compiler::session_compile_phase_t::start:
                  fmt::print("{}\n", source_file.filename().string());
                  break;
              case gfx::compiler::session_compile_phase_t::success:
                  break;
              case gfx::compiler::session_compile_phase_t::failed:
                  break;
            }
        },
        .definitions = definitions
    };
    compiler::session session(session_options, source_files);
    if (!session.initialize(r)) {
        print_results(r);
        return 1;
    } else {
        if (!session.compile(r)) {
            print_results(r);
            return 1;
        } else {
            high_resolution_clock::time_point end = high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            fmt::print("Total compilation time (in μs): {}\n", duration);
            fmt::print("\n");
            session.finalize();
            return 0;
        }
    }
}
