//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#define COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace gfx::compiler {

class type;
class field;
class block;
class element;
class program;
class directive;
class array_type;
class string_type;
class attribute;
class identifier;
class expression;
class initializer;
class float_literal;
class operator_base;
class procedure_type;
class unary_operator;
class integer_literal;
class binary_operator;
class namespace_element;

using element_list_t = std::vector<element*>;

using directive_map_t = std::map<std::string, directive*>;

struct attribute_map_t {
	attribute_map_t(element* parent);

	~attribute_map_t();

	inline size_t size() const
	{
		return attrs_.size();
	}

	bool remove(const std::string& name);

	compiler::attribute* find(const std::string& name);

	element * add(const std::string& name, element* expr);

private:
	element* parent_ = nullptr;
	std::unordered_map<std::string, attribute*> attrs_ {};
};

}
#endif // COMPILER_ELEMENTS_ELEMENT_TYPES_H_
