//
// Created by 12132 on 2022/3/30.
//

#include "symbol_table.h"
namespace gfx {
void symbol_table::put(const std::string &name, const symbol_table_entry_t &value) {
	symbols_.insert(std::make_pair(name,value));
}

void symbol_table::clear() {
	symbols_.clear();
}

void symbol_table::remove(const std::string &name) {
	symbols_.erase(name);
}

}