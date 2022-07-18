//
// Created by 12132 on 2022/4/20.
//

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include "terp.h"
#include "symbol.h"
#include "segment.h"
#include "common/result.h"
#include "common/id_pool.h"
#include "instruction_emitter.h"
#include "instruction_block.h"

namespace gfx {
class assembler {
public:
	explicit assembler(terp* terp);

	virtual ~assembler();

    bool assemble(result& r, instruction_block* block);

    bool assemble_from_source(result& r, std::istream& source);

    segment_list_t segments() const;

    gfx::segment* segment(const std::string& name);

	gfx::segment* segment(const std::string &name, segment_type_t type);

    instruction_block* current_block();

    instruction_block* make_new_block();

private:
	terp* terp_ = nullptr;
    uint64_t location_counter_ = 0;
    std::vector<instruction_block*> blocks_ {};
    instruction_block* current_block_ = nullptr;
	std::unordered_map<std::string, gfx::segment> segments_{};
};
}
#endif // ASSEMBLER_H_
