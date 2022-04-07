//
// Created by 12132 on 2022/3/30.
//

#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_
#include "src/common/result.h"
#include "ast/ast.h"

namespace gfx {
using symbol_dict = std::unordered_map<std::string, ast_node_shared_ptr>;
class symbol_table {
public:
	symbol_table() =default;

	void put(const std::string& name, const ast_node_shared_ptr& value);

	void clear();

	void remove(const std::string& name);

	std::vector<std::string> identifiers()
	{
		std::vector<std::string> identifiers;
		for (auto& symbol : symbols_) {
			identifiers.push_back(symbol.first);
		}
		return identifiers;
	}

	ast_node_shared_ptr get(const std::string& name) const;

private:
	symbol_dict symbols_;
};
}

#endif // SYMBOL_TABLE_H_
