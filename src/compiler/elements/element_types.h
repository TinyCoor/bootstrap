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
class any_type;
class field;
class block;
class element;
class program;
class directive;
class array_type;
class string_type;
class numeric_type;
class attribute;
class identifier;
class expression;
class initializer;
class float_literal;
class operator_base;
class procedure_type;
class line_comment;
class unary_operator;
class integer_literal;
class binary_operator;
class namespace_element;
class composite_type;

using element_list_t = std::vector<element*>;

using directive_map_t = std::map<std::string, directive*>;

struct attribute_map_t {

	size_t size() const;

	void add(attribute* value);

	bool remove(const std::string& name);

	compiler::attribute* find(const std::string& name);

private:
	std::unordered_map<std::string, attribute*> attrs_ {};
};

struct field_map_t {

	void add(field* value);

	size_t size() const;

	bool remove(const std::string& name);

	compiler::field* find(const std::string& name);

private:
	std::unordered_map<std::string, field*> fields_ {};
};


struct identifier_map_t {
	void add(identifier* value);

	size_t size() const;

	bool remove(const std::string& name);

	identifier* find(const std::string& name);

	// XXX: add ability to get range of identifiers for overloads

private:
	std::unordered_multimap<std::string, identifier*> identifiers_ {};
};

struct type_map_t {
	void add(compiler::type* type);

	size_t size() const;

	bool remove(const std::string& name);

	compiler::type* find(const std::string& name);

private:
	std::unordered_map<std::string, type*> types_ {};
};

}
#endif // COMPILER_ELEMENTS_ELEMENT_TYPES_H_
