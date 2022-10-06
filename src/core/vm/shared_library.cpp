//
// Created by 12132 on 2022/4/13.
//

#include "shared_library.h"
#include "fmt/format.h"
using namespace gfx;
bool shared_library::initialize(gfx::result &r, const std::filesystem::path &path)
{
	library_ = dlLoadLibrary(path.string().c_str());
	if (library_ == nullptr) {
		r.add_message("B062", fmt::format("unable to load library image file: {}.", path.string()), true);
		return false;
	}
	get_library_path();
	return true;
}

 bool shared_library::initialize(gfx::result &r)
 {
	library_ = dlLoadLibrary(nullptr);
	if (library_ == nullptr) {
		r.add_message("B062", fmt::format("unable to load library image for self."), true);
		return false;
	}
	get_library_path();
	return true;
}

bool shared_library::exports_symbol(const std::string &symbol_name)
{
	return symbols_.count(symbol_name) > 0;
}

void* shared_library::symbol_address(const std::string &symbol_name)
{
	auto it = symbols_.find(symbol_name);
	if (it == symbols_.end()) {
		auto func_ptr = dlFindSymbol(library_, symbol_name.c_str());
		symbols_.insert(std::make_pair(symbol_name, func_ptr));
		return func_ptr;
	}
	if (it->second == nullptr) {
		it->second = dlFindSymbol(library_, symbol_name.c_str());
	}
	return it->second;
}

void shared_library::load_symbols(const char *path)
{
	symbols_.clear();
	auto symbol_ptr = dlSymsInit(path);
	if (symbol_ptr != nullptr) {
		int count = dlSymsCount(symbol_ptr);
		for (int i = 0; i < count; i++) {
			const char* symbol_name = dlSymsName(symbol_ptr, i);
			if (symbol_name != nullptr) {
				symbols_.insert(std::make_pair(symbol_name, nullptr));
			}
		}
		dlSymsCleanup(symbol_ptr);
	}
}

void shared_library::get_library_path()
{
	if (!library_) {
		return;
	}
	//todo
}
