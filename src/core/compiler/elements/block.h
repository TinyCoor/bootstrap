//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_BLOCK_H_
#define COMPILER_ELEMENTS_BLOCK_H_
#include "type.h"
#include "element.h"
#include "element_types.h"
#include "identifier.h"
#include "vm/assembler.h"

namespace gfx::compiler {
class block : public element {
public:
	explicit block(block* parent, element_type_t type = element_type_t::block);

	~block() override;

	type_map_t& types();

	block_list_t& blocks();

	bool emit(result& r, assembler& ass);

	bool define_data(result& r, assembler& assembler);

	comment_list_t& comments();

	statement_list_t& statements();

	identifier_map_t& identifiers();
private:
	void add_symbols(result& r, segment_t* segment, const identifier_list_t &list);

private:
	type_map_t types_ {};
	comment_list_t comments_{};
	block_list_t blocks_ {};
	statement_list_t statements_{};
	identifier_map_t identifiers_ {};
};
}

#endif // COMPILER_ELEMENTS_BLOCK_H_
