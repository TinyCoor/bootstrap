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
#include "instruction_block.h"
#include "assembly_listing.h"
#include <stack>

namespace gfx {
class assembler {
public:
	explicit assembler(terp* terp);

	virtual ~assembler();

    bool initialize(result& r);

    assembly_listing& listing();

    bool assemble(result& r, instruction_block* block);

    bool assemble_from_source(result& r, std::istream& source);

    void push_block(instruction_block* block);

    instruction_block* pop_block();

    instruction_block* root_block();

    segment_list_t segments() const;

    instruction_block* current_block();

    bool in_procedure_scope() const;

    gfx::segment* segment(const std::string& name);

	gfx::segment* segment(const std::string &name, segment_type_t type);

    instruction_block* make_basic_block(instruction_block* parent = nullptr);

    instruction_block* make_procedure_block(instruction_block* parent = nullptr);

private:
    void add_new_block(instruction_block* block);

private:
	terp* terp_ = nullptr;
    uint64_t location_counter_ = 0;
    assembly_listing listing_ {};
    uint32_t procedure_block_count_ = 0;
    std::vector<instruction_block*> blocks_ {};
    std::stack<instruction_block*> block_stack_{};
	std::unordered_map<std::string, gfx::segment> segments_{};
};
}
#endif // ASSEMBLER_H_
