//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#define COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#include <set>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "parser/token.h"

namespace gfx::compiler {
using id_t = uint32_t;
class cast;
class type;
class alias;
class field;
class block;
class label;
class import;
class element;
class program;
class comment;
class any_type;
class directive;
class attribute;
class identifier;
class expression;
class initializer;
class statement;
class type_info;
class if_element;
class tuple_type;
class array_type;
class string_type;
class unknown_type;
class numeric_type;
class float_literal;
class argument_list;
class return_element;
class symbol_element;
class procedure_call;
class operator_base;
class procedure_type;
class unary_operator;
class integer_literal;
class boolean_literal;
class namespace_type;
class composite_type;
class string_literal;
class binary_operator;
class namespace_element;
class procedure_instance;
class identifier_reference;

using string_set_t = std::set<std::string>;
using string_list_t = std::vector<std::string>;
using type_list_t = std::vector<type*>;
using label_list_t = std::vector<label*>;
using block_list_t = std::vector<block*>;
using field_list_t = std::vector<field*>;
using element_list_t = std::vector<element*>;
using comment_list_t = std::vector<comment*>;
using statement_list_t = std::vector<statement*>;
using identifier_list_t = std::vector<identifier*>;
using attribute_list_t = std::vector<attribute*>;
using directive_map_t = std::map<std::string, directive*>;
using string_literal_list_t = std::vector<string_literal*>;
using procedure_type_list_t = std::vector<procedure_type*>;
using procedure_instance_list_t = std::vector<procedure_instance*>;
using identifier_reference_list_t = std::vector<identifier_reference*>;

///////////////////////////////////////////////////////////////////////////

enum class composite_types_t {
	enum_type,
	union_type,
	struct_type,
};

static inline std::unordered_map<composite_types_t, std::string_view> s_composite_type_names = {
	{composite_types_t::enum_type, "enum_type"},
	{composite_types_t::union_type, "union_type"},
	{composite_types_t::struct_type, "struct_type"},
};

static inline std::string_view composite_type_name(composite_types_t type) {
	auto it = s_composite_type_names.find(type);
	if (it == s_composite_type_names.end()) {
		return "unknown_composite_type";
	}
	return it->second;
}

enum class element_type_t {
	element = 1,
	cast,
	label,
	block,
    symbol,
	field,
	if_e,
	comment,
	program,
	any_type,
    type_info,
	proc_type,
	directive,
	attribute,
	bool_type,
	proc_call,
	return_e,
	import_e,
	statement,
	alias_type,
    tuple_type,
	array_type,
	identifier,
	expression,
	string_type,
    struct_type,
	namespace_e,
	initializer,
	numeric_type,
	unknown_type,
	argument_list,
	proc_instance,
	float_literal,
	string_literal,
	namespace_type,
	composite_type,
	unary_operator,
	boolean_literal,
	integer_literal,
	binary_operator,
	proc_type_block,
	unknown_identifier,
	proc_instance_block,
    identifier_reference,
};

static inline std::unordered_map<element_type_t, std::string_view> s_element_type_names = {
	{element_type_t::if_e, 					"if"},
	{element_type_t::cast, 					"cast"},
	{element_type_t::label, 				"label"},
	{element_type_t::block, 				"block"},
	{element_type_t::field, 				"field"},
	{element_type_t::return_e, 				"return"},
	{element_type_t::import_e, 				"import"},
    {element_type_t::symbol,            "symbol_element"},
	{element_type_t::element, 				"element"},
	{element_type_t::comment, 				"comment"},
	{element_type_t::program, 				"program"},
	{element_type_t::any_type, 				"any_type"},
    {element_type_t::type_info, 			"type_info"},
	{element_type_t::proc_type, 			"proc_type"},
	{element_type_t::directive, 			"directive"},
	{element_type_t::attribute, 			"attribute"},
	{element_type_t::bool_type, 			"bool_type"},
	{element_type_t::statement, 			"statement"},
	{element_type_t::proc_call, 			"proc_call"},
    {element_type_t::namespace_e, 			"namespace"},
    {element_type_t::tuple_type, 			"tuple_type"},
	{element_type_t::alias_type, 			"alias_type"},
	{element_type_t::array_type, 			"array_type"},
	{element_type_t::identifier, 			"identifier"},
	{element_type_t::expression, 			"expression"},
	{element_type_t::string_type, 			"string_type"},
    {element_type_t::struct_type, 			"struct_type"},
	{element_type_t::initializer, 			"initializer"},
	{element_type_t::unknown_type, 			"unknown_type"},
	{element_type_t::numeric_type, 			"numeric_type"},
	{element_type_t::argument_list, 		"argument_list"},
	{element_type_t::proc_instance, 		"proc_instance"},
	{element_type_t::float_literal, 		"float_literal"},
	{element_type_t::string_literal,		"string_literal"},
	{element_type_t::composite_type, 		"composite_type"},
	{element_type_t::namespace_type, 		"namespace_type"},
	{element_type_t::unary_operator, 		"unary_operator"},
	{element_type_t::boolean_literal, 		"boolean_literal"},
	{element_type_t::integer_literal, 		"integer_literal"},
	{element_type_t::binary_operator, 		"binary_operator"},
	{element_type_t::proc_type_block, 		"proc_type_block"},
	{element_type_t::unknown_identifier, 	"unknown_identifier"},
	{element_type_t::proc_instance_block, 	"proc_instance_block"},
    {element_type_t::identifier_reference, 	"identifier_reference"},
};

static inline std::string_view element_type_name(element_type_t type) {
	auto it = s_element_type_names.find(type);
	if (it == s_element_type_names.end()) {
		return "";
	}
	return it->second;
}

enum class  comment_type_t {
	line = 1,
	block,
};

static inline std::string_view comment_type_name(comment_type_t type) {
	switch (type) {
		case comment_type_t::line:  return "line";
		case comment_type_t::block: return "block";
	}
	return "unknown";
}

enum class operator_type_t {
	unknown,
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

static inline std::unordered_map<operator_type_t, std::string_view> s_operator_type_names = {
	{operator_type_t::unknown,               "unknown"},
	{operator_type_t::negate,                "negate"},
	{operator_type_t::binary_not,            "binary_not"},
	{operator_type_t::logical_not,           "logical_not"},
	{operator_type_t::add,                   "add"},
	{operator_type_t::subtract,              "subtract"},
	{operator_type_t::multiply,              "multiply"},
	{operator_type_t::divide,                "divide"},
	{operator_type_t::modulo,                "modulo"},
	{operator_type_t::equals,                "equals"},
	{operator_type_t::not_equals,            "not_equals"},
	{operator_type_t::greater_than,          "greater_than"},
	{operator_type_t::less_than,             "less_than"},
	{operator_type_t::greater_than_or_equal, "greater_than_or_equal"},
	{operator_type_t::less_than_or_equal,    "less_than_or_equal"},
	{operator_type_t::logical_or,            "logical_or"},
	{operator_type_t::logical_and,           "logical_and"},
	{operator_type_t::binary_or,             "binary_or"},
	{operator_type_t::binary_and,            "binary_and"},
	{operator_type_t::binary_xor,            "binary_xor"},
	{operator_type_t::shift_right,           "shift_right"},
	{operator_type_t::shift_left,            "shift_left"},
	{operator_type_t::rotate_right,          "rotate_right"},
	{operator_type_t::rotate_left,           "rotate_left"},
	{operator_type_t::exponent,              "exponent"},
	{operator_type_t::assignment,            "assignment"}
};

static inline std::string_view operator_type_name(operator_type_t type) {
	auto it = s_operator_type_names.find(type);
	if (it == s_operator_type_names.end())
		return "";
	return it->second;
}

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

	attribute_list_t as_list() const;

	bool remove(const std::string& name);

	compiler::attribute* find(const std::string& name);

private:
	std::unordered_map<std::string, attribute*> attrs_ {};
};

struct field_map_t {

	void add(field* value);

	field_list_t as_list();

	size_t size() const;

	bool remove(id_t id);

	compiler::field* find(id_t id);

private:
	std::unordered_map<id_t , field*> fields_ {};
};

struct identifier_map_t {
	void add(identifier* value);

	size_t size() const;

	bool empty() const {return identifiers_.empty();}

	bool remove(const std::string& name);

	identifier* find(const std::string& name);

	identifier_list_t as_list() const;

private:
	std::unordered_multimap<std::string, identifier*> identifiers_ {};
};

struct type_map_t {
	void add(compiler::type* type);

    type_list_t as_list() const;

	size_t size() const;

	bool remove(const std::string& name);

	compiler::type* find(const std::string& name);

private:
	std::unordered_map<std::string, type*> types_ {};
};

struct qualified_symbol_t {
    bool is_qualified() const {
        return !namespaces.empty();
    }
    std::string name {};
    string_list_t namespaces {};
};

}
#endif // COMPILER_ELEMENTS_ELEMENT_TYPES_H_
