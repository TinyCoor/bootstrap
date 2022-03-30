//
// Created by 12132 on 2022/3/30.
//

#ifndef BOOTSTRAP__SYMBOL_TABLE_H_
#define BOOTSTRAP__SYMBOL_TABLE_H_
#include "result.h"
#include "parser_types.h"

namespace gfx {
class symbol_table {
public:
	symbol_table() =default;

	void put(const std::string& name, const ast_node_shared_ptr& value);

	void clear();

	bool missing_is_error() const
	{
		return missing_is_error_;
	}

	void missing_is_error(bool flag)
	{
		missing_is_error_ = flag;
	}

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
	bool missing_is_error_ = true;

};
}

#endif //BOOTSTRAP__SYMBOL_TABLE_H_
