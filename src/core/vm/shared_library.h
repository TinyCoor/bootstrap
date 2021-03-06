//
// Created by 12132 on 2022/4/13.
//

#ifndef SHARED_LIBRARY_H_
#define SHARED_LIBRARY_H_
#include "ffi.h"
#include <filesystem>
#include "common/result.h"

namespace gfx {
using symbol_address_map = std::unordered_map<std::string, void*>;

class shared_library {
public:
	shared_library() = default;
	~shared_library() {
		if (library_) {
			dlFreeLibrary(library_);
			library_ = nullptr;
		}
	}

	bool initialize(result& r, const std::filesystem::path& path);

	bool initialize(result& r);

	inline const std::filesystem::path& path() const
	{
		return path_;
	}

	inline const symbol_address_map& symbols() const
	{
		return symbols_;
	}

	bool exports_symbol(const std::string& symbol_name);

	void* symbol_address(const std::string& symbol_name);

private:
	void load_symbols(const char* path);

	void get_library_path();

private:
	DLLib *library_ = nullptr;
	std::filesystem::path path_{};
	symbol_address_map symbols_{};
};
}
#endif // SHARED_LIBRARY_H_
