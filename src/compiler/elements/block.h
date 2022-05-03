//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_BLOCK_H_
#define COMPILER_ELEMENTS_BLOCK_H_
#include "type.h"
#include "element.h"
#include "element_types.h"
#include "identifier.h"

namespace gfx::compiler {

class block : public element {
public:
	explicit block(block* parent);

	~block() override;

	block* parent() const;

	element_list_t& children();

	type_map_t& types() {
		return types_;
	}

	identifier_map_t& identifiers() {
		return identifiers_;
	}

private:
	type_map_t types_ {};
	block* parent_ = nullptr;
	element_list_t children_ {};
	identifier_map_t identifiers_ {};
};
}

#endif // COMPILER_ELEMENTS_BLOCK_H_
