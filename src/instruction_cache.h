//
// Created by 12132 on 2022/4/3.
//

#ifndef INSTRUCTION_CACHE_H_
#define INSTRUCTION_CACHE_H_

#include <unordered_map>
#include <cstdint>
#include "instruction.h"
namespace gfx {
class terp;
struct inst_cache_entry_t{
	size_t size;
	instruction_t inst;
};

class instruction_cache {
public:
	explicit instruction_cache(terp* terp);

	void reset();

	size_t fetch(result& r, instruction_t& inst);

	size_t fetch_at(result& r, instruction_t& inst,uint64_t address);
private:
	terp* terp_ = nullptr;
	std::unordered_map<uint64_t, inst_cache_entry_t > cache_;
};
}

#endif // INSTRUCTION_CACHE_H_
