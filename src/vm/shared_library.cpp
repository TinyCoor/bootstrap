//
// Created by 12132 on 2022/4/13.
//

#include "shared_library.h"
#include <fmt/format.h>
using namespace gfx;
bool shared_library::initialize(gfx::result &r, const std::filesystem::path &path)
{
	if (!std::filesystem::exists(path)) {
		r.add_message("B061", fmt::format("shared library image file not found: {}.", path.string()), true);
		return false;
	}
	auto path_c_str = path.string().c_str();
	library_ = dlLoadLibrary(path_c_str);
	if (library_ == nullptr) {
		r.add_message("B062", fmt::format("unable to load library image file: {}.", path.string()), true);
		return false;
	}
	load_symbols(path_c_str);
	return true;
}

//bool shared_library::initialize(gfx::result &r) {
//	library_ = dlLoadLibrary(nullptr);
//	if (library_ == nullptr) {
//		r.add_message("B062", fmt::format("unable to load library image for self."), true);
//		return false;
//	}
//	char library_path[PATH_MAX];
//	dlGetLibraryPath(library_, library_path, PATH_MAX);
//	load_symbols(library_path);
//	path_ = library_path;
//	return true;
//}
bool shared_library::exports_symbol(const std::string &symbol_name) {
	return symbols_.count(symbol_name) > 0;
}

void *shared_library::get_symbol_address(const std::string &symbol_name) {
	auto it = symbols_.find(symbol_name);
	if (it == symbols_.end())
		return nullptr;
	if (it->second == nullptr) {
		it->second = dlFindSymbol(library_, symbol_name.c_str());
	}
	return it->second;
}

void shared_library::load_symbols(const char *path) {
	symbols_.clear();
	auto symbol_ptr = dlSymsInit(path);
	if (symbol_ptr != nullptr) {
		int count = dlSymsCount(symbol_ptr);
		for (int i = 0; i < count; i++) {
			const char* symbol_name = dlSymsName(symbol_ptr, i);
			if (symbol_name != nullptr)
				symbols_.insert(std::make_pair(symbol_name, nullptr));
		}
		dlSymsCleanup(symbol_ptr);
	}
}
