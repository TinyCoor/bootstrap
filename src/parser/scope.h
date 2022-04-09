//
// Created by 12132 on 2022/4/9.
//

#ifndef SCOPE_H_
#define SCOPE_H_
#include <memory>
#include "symbol_table.h"

namespace gfx {

class scope;

using scope_list = std::vector<std::unique_ptr<scope>>;

class scope {
public:
	scope(scope* parent, const ast_node_shared_ptr& node);

	void clear();

	uint64_t address() const;

	scope* parent();

	void address(uint64_t value);

	ast_node_shared_ptr ast_node();

	const scope_list& children() const;

	scope* add_child_scope(const ast_node_shared_ptr& node);

private:
	uint64_t address_;
	scope_list children_ {};
	scope* parent_ = nullptr;
	ast_node_shared_ptr node_ = nullptr;
	symbol_table symbol_table_ {};
};

}

#endif // SCOPE_H_
