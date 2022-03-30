//
// Created by 12132 on 2022/3/30.
//

#include "symbol_table.h"
namespace gfx {
void symbol_table::put(const std::string &name, const gfx::ast_node_shared_ptr &value) {
	symbols_[name] = value;
}

void symbol_table::clear() {
	symbols_.clear();
}

void symbol_table::remove(const std::string &name) {
	symbols_.erase(name);
}

gfx::ast_node_shared_ptr symbol_table::get(const std::string &name) const {
	auto it = symbols_.find(name);
	if (it == symbols_.end())
		return nullptr;
	return it->second;
}

}