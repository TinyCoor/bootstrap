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

	bool define_data(result& r, string_set_t& interned_strings, assembler& assembler);

	comment_list_t& comments();

	statement_list_t& statements();

	identifier_map_t& identifiers();
private:
	static void add_symbols(result& r, segment* segment, const identifier_list_t &list);

    bool on_emit(result& r, assembler& assembler, emit_context_t& context) override;
private:
	type_map_t types_ {};
	comment_list_t comments_{};
	block_list_t blocks_ {};
	statement_list_t statements_{};
	identifier_map_t identifiers_ {};
};
}

#endif // COMPILER_ELEMENTS_BLOCK_H_
