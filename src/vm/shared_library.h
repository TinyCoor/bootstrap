//
// Created by 12132 on 2022/4/13.
//

#ifndef SHARED_LIBRARY_H_
#define SHARED_LIBRARY_H_
#include <filesystem>
#include <common/result.h>
#include <dyncall/dyncall.h>
#include <dynload/dynload.h>

namespace gfx {
using symbol_address_map = std::unordered_map<std::string, void *>;

class shared_library {
public:
	shared_library() =default;
	~shared_library() {
		if (library_) {
			dlFreeLibrary(library_);
			library_ = nullptr;
		}
	}

	bool initialize(result& r, const std::filesystem::path& path);

//	bool initialize(result& r);

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

private:
	DLLib *library_ = nullptr;
	std::filesystem::path path_{};
	symbol_address_map symbols_{};
};
}
#endif //BOOTSTRAP_SRC_VM_SHARED_LIBRARY_H_
