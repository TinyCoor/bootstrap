//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_BLOCK_H_
#define COMPILER_ELEMENTS_BLOCK_H_
#include "element.h"
namespace gfx::compiler {

class block : public element {
public:
	explicit block(block* parent);

	~block() override;

	block* parent() const;

	element_list_t& children();

	inline size_t type_count() const {
		return types_.size();
	}

	inline size_t identifier_count() const {
		return identifiers_.size();
	}

	bool remove_type(const std::string& name);

	bool remove_identifier(const std::string& name);

	compiler::type* find_type(const std::string& name);

	compiler::identifier* find_identifier(const std::string& name);

private:
	type_map_t types_ {};
	block* parent_ = nullptr;
	element_list_t children_ {};
	identifier_map_t identifiers_ {};
};
}

#endif // COMPILER_ELEMENTS_BLOCK_H_
