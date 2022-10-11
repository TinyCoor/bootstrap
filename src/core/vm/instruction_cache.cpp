//
// Created by 12132 on 2022/4/3.
//

#include "instruction_cache.h"
#include "terp.h"
namespace gfx {

instruction_cache::instruction_cache(terp *terp) : terp_(terp)
{
}

void instruction_cache::reset()
{
	cache_.clear();
}

size_t instruction_cache::fetch(result& r, instruction_t &inst)
{
	return fetch_at(r,  inst, terp_->register_file().r[register_pc].qw);
}

size_t instruction_cache::fetch_at(result& r, instruction_t &inst, uint64_t address)
{
	auto it = cache_.find(address);
	if (it == cache_.end()) {
		auto inst_size = inst.decode(r, terp_->heap(), address);
		if (inst_size == 0 ) {
			return inst_size;
		}
		cache_.insert(std::make_pair(address, inst_cache_entry_t{.size = inst_size, .inst = inst}));
		return inst_size;
	} else {
		inst = it->second.inst;
		return it->second.size;
	}
}
}