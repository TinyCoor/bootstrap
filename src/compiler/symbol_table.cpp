//
// Created by 12132 on 2022/3/30.
//

#include "symbol_table.h"
namespace gfx {

void symbol_table::put(const std::string &name, const symbol_table_entry_t &value)
{
	symbols_.insert(std::make_pair(name,value));
}

void symbol_table::clear()
{
	symbols_.clear();
}

void symbol_table::remove(const std::string &name)
{
	symbols_.erase(name);
}

std::set<std::string> symbol_table::names() const
{
	std::set<std::string> names;
	for (const auto& symbol : symbols_) {
		names.insert(symbol.first);
	}
	return names;
}

bool symbol_table::is_defined(const std::string &name)
{
	return symbols_.count(name) > 0;
}

symbol_lookup_result_t symbol_table::get(const std::string &name)
{
	auto range = symbols_.equal_range(name);
	if (range.first == range.second) {
		return {};
	}
	symbol_lookup_result_t result;
	result.found = true;
	for (auto it = range.first; it != range.second; ++it) {
		result.entries.push_back(&it->second);
	}
	return result;
}

}