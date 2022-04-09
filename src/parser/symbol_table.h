//
// Created by 12132 on 2022/3/30.
//

#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_
#include "src/common/result.h"
#include "ast/ast.h"
#include <set>

namespace gfx {

struct symbol_table_entry_t {
	uint64_t address = 0;
	ast_node_shared_ptr node = nullptr;
};

struct symbol_lookup_result_t {
	bool found = false;
	std::vector<symbol_table_entry_t*> entries {};
};

using symbol_dict = std::unordered_multimap<std::string, symbol_table_entry_t>;
class symbol_table {
public:
	symbol_table() =default;

	void put(const std::string& name, const symbol_table_entry_t& value);

	void clear();

	void remove(const std::string& name);

	std::set<std::string> names() const;

	bool is_defined(const std::string& name);

	symbol_lookup_result_t get(const std::string& name);

private:
	symbol_dict symbols_;
};
}

#endif // SYMBOL_TABLE_H_
