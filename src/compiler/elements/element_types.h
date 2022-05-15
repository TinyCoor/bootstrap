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
#include "parser/token.h"

namespace gfx::compiler {

class cast;
class type;
class alias;
class field;
class block;
class label;
class element;
class program;
class comment;
class any_type;
class directive;
class attribute;
class statement;
class array_type;
class string_type;
class numeric_type;
class if_element;
class return_element;
class procedure_call;
class identifier;
class expression;
class initializer;
class float_literal;
class operator_base;
class procedure_type;
class unary_operator;
class integer_literal;
class boolean_literal;
class string_literal;
class binary_operator;
class namespace_element;
class composite_type;
class procedure_instance;

using label_list_t  = std::vector<label*>;
using element_list_t = std::vector<element*>;
using comment_list_t  = std::vector<comment*>;
using statement_list_t  = std::vector<statement*>;
using directive_map_t = std::map<std::string, directive*>;
using procedure_instance_list_t = std::vector<procedure_instance*>;

enum class element_type_t {
	element = 1,
	cast,
	label,
	block,
	field,
	if_e,
	else_e,
	comment,
	program,
	any_type,
	proc_type,
	directive,
	attribute,
	bool_type,
	proc_call,
	return_e,
	statement,
	alias_type,
	else_if_e,
	array_type,
	identifier,
	expression,
	string_type,
	namespace_e,
	initializer,
	numeric_type,
	proc_instance,
	float_literal,
	string_literal,
	composite_type,
	unary_operator,
	boolean_literal,
	integer_literal,
	binary_operator,
};

enum class  comment_type_t {
	line = 1,
	block,
};

enum class operator_type_t {
	// unary
	negate,
	binary_not,
	logical_not,

	// binary
	add,
	subtract,
	multiply,
	divide,
	modulo,
	equals,
	not_equals,
	greater_than,
	less_than,
	greater_than_or_equal,
	less_than_or_equal,
	logical_or,
	logical_and,
	binary_or,
	binary_and,
	binary_xor,
	shift_right,
	shift_left,
	rotate_right,
	rotate_left,
	exponent,
	assignment
};

static inline std::unordered_map<token_types_t, operator_type_t> s_unary_operators = {
	{token_types_t::minus, operator_type_t::negate},
	{token_types_t::tilde, operator_type_t::binary_not},
	{token_types_t::bang,  operator_type_t::logical_not}
};

static inline std::unordered_map<token_types_t, operator_type_t> s_binary_operators = {
	{ token_types_t::equals,     operator_type_t::equals},
	{ token_types_t::minus,      operator_type_t::subtract},
	{ token_types_t::plus,       operator_type_t::add},
	{ token_types_t::asterisk,   operator_type_t::multiply},
	{ token_types_t::slash,      operator_type_t::divide},
	{ token_types_t::percent,    operator_type_t::modulo},
	{ token_types_t::not_equals, operator_type_t::not_equals},
	{ token_types_t::assignment, operator_type_t::assignment},
	{ token_types_t::logical_or, operator_type_t::logical_or}
};


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
