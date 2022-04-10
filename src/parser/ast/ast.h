//
// Created by 12132 on 2022/4/1.
//

#ifndef AST_H_
#define AST_H_

#include "src/lexer/token.h"
#include <memory>
#include <vector>
#include <stack>

namespace gfx {

struct ast_node_t;
using ast_node_shared_ptr = std::shared_ptr<ast_node_t>;
using ast_node_list = std::vector<ast_node_shared_ptr>;

enum class ast_node_types_t {
	program,
	fn_call,
	statement,
	attribute,
	assignment,
	expression,
	basic_block,
	line_comment,
	none_literal,
	null_literal,
	empty_literal,
	for_statement,
	block_comment,
	argument_list,
	fn_expression,
	if_expression,
	number_literal,
	string_literal,
	unary_operator,
	binary_operator,
	boolean_literal,
	map_constructor,
	else_expression,
	while_statement,
	alias_statement,
	break_statement,
	with_expression,
	type_identifier,
	extend_statement,
	character_literal,
	array_constructor,
	elseif_expression,
	switch_expression,
	continue_statement,
	variable_reference,
	namespace_statement,
	variable_declaration,
};

// foo := 5 + 5;
//
//  program_node (root)
//          |
//          |
//          | statement_node
//          +---> lhs := variable (token = "foo")
//                         |
//                rhs := expression (token is unknown)
//                         |
//                         +--children
//                              |
//                           (0)+--> binary_operator (token = "+")
//                                      lhs := number_literal (token = "5")
//
//                                      rhs := number_literal (token = "5")
//

static inline std::unordered_map<ast_node_types_t, std::string> s_node_type_names = {
	{ast_node_types_t::program, "program"},
	{ast_node_types_t::fn_call, "fn_call"},
	{ast_node_types_t::statement, "statement"},
	{ast_node_types_t::attribute, "attribute"},
	{ast_node_types_t::assignment, "assignment"},
	{ast_node_types_t::expression, "expression"},
	{ast_node_types_t::basic_block, "basic_block"},
	{ast_node_types_t::line_comment, "line_comment"},
	{ast_node_types_t::none_literal, "none_literal"},
	{ast_node_types_t::null_literal, "null_literal"},
	{ast_node_types_t::empty_literal, "empty_literal"},
	{ast_node_types_t::for_statement, "for_statement"},
	{ast_node_types_t::block_comment, "block_comment"},
	{ast_node_types_t::argument_list, "argument_list"},
	{ast_node_types_t::fn_expression, "fn_expression"},
	{ast_node_types_t::if_expression, "if_expression"},
	{ast_node_types_t::number_literal, "number_literal"},
	{ast_node_types_t::string_literal, "string_literal"},
	{ast_node_types_t::unary_operator, "unary_operator"},
	{ast_node_types_t::binary_operator, "binary_operator"},
	{ast_node_types_t::boolean_literal, "boolean_literal"},
	{ast_node_types_t::map_constructor, "map_constructor"},
	{ast_node_types_t::else_expression, "else_expression"},
	{ast_node_types_t::while_statement, "while_statement"},
	{ast_node_types_t::alias_statement, "alias_statement"},
	{ast_node_types_t::break_statement, "break_statement"},
	{ast_node_types_t::with_expression, "with_expression"},
	{ast_node_types_t::type_identifier, "type_identifier"},
	{ast_node_types_t::extend_statement, "extend_statement"},
	{ast_node_types_t::character_literal, "character_literal"},
	{ast_node_types_t::array_constructor, "array_constructor"},
	{ast_node_types_t::elseif_expression, "elseif_expression"},
	{ast_node_types_t::switch_expression, "switch_statement"},
	{ast_node_types_t::continue_statement, "continue_statement"},
	{ast_node_types_t::variable_reference, "variable_reference"},
	{ast_node_types_t::namespace_statement, "namespace_statement"},
	{ast_node_types_t::variable_declaration, "variable_declaration"},
};

struct ast_node_t {
	using flags_value_t = uint8_t;
	enum flags_t : uint8_t {
		none    = 0b00000000,
		pointer = 0b00000001,
		array   = 0b00000010,
	};

	inline bool is_array() const
	{
		return ((flags & flags_t::array) != 0);
	}

	inline bool is_pointer() const
	{
		return ((flags & flags_t::pointer) != 0);
	}

	inline std::string name() const
	{
		auto it = s_node_type_names.find(type);
		if (it == s_node_type_names.end())
			return "unknown";
		return it->second;
	}

	token_t token;
	ast_node_types_t type;
	ast_node_list children {};
	ast_node_shared_ptr lhs = nullptr;
	ast_node_shared_ptr rhs = nullptr;
	flags_value_t flags = flags_t::none;
	ast_node_shared_ptr parent = nullptr;
};

class ast_builder {
public:
	ast_builder();

	virtual ~ast_builder();

	ast_node_shared_ptr if_node();

	ast_node_shared_ptr else_node();

	ast_node_shared_ptr end_scope();

	ast_node_shared_ptr pop_scope();

	ast_node_shared_ptr begin_scope();

	ast_node_t* current_scope() const;

	ast_node_shared_ptr program_node();

	ast_node_shared_ptr fn_call_node();

	ast_node_shared_ptr fn_decl_node();

	ast_node_shared_ptr else_if_node();

	ast_node_shared_ptr statement_node();

	ast_node_shared_ptr assignment_node();

	ast_node_shared_ptr expression_node();

	ast_node_shared_ptr basic_block_node();

	void push_scope(const ast_node_shared_ptr& node);

	ast_node_shared_ptr argument_list_node();

	ast_node_shared_ptr break_node(const token_t& token);

	ast_node_shared_ptr continue_node(const token_t& token);

	ast_node_shared_ptr attribute_node(const token_t& token);

	ast_node_shared_ptr none_literal_node(const token_t& token);

	ast_node_shared_ptr empty_literal_node(const token_t& token);

	ast_node_shared_ptr null_literal_node(const token_t& token);

	ast_node_shared_ptr line_comment_node(const token_t& token);

	ast_node_shared_ptr block_comment_node(const token_t& token);

	ast_node_shared_ptr number_literal_node(const token_t& token);

	ast_node_shared_ptr string_literal_node(const token_t& token);

	ast_node_shared_ptr unary_operator_node(const token_t& token);

	ast_node_shared_ptr type_identifier_node(const token_t& token);

	ast_node_shared_ptr boolean_literal_node(const token_t& token);

	ast_node_shared_ptr character_literal_node(const token_t& token);

	ast_node_shared_ptr variable_reference_node(const token_t& token);

	ast_node_shared_ptr variable_declaration_node(const token_t& token);

	ast_node_shared_ptr binary_operator_node(const ast_node_shared_ptr& lhs, const token_t& token,
											 const ast_node_shared_ptr& rhs);

private:
	std::stack<ast_node_shared_ptr> scope_stack_ {};
};
}

#endif // AST_H_
