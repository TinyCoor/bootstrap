//
// Created by 12132 on 2022/3/30.
//
#ifndef PARSER_H_
#define PARSER_H_
#include "src/parser/ast/ast.h"
#include "src/common/result.h"
#include "src/lexer/lexer.h"
#include <map>
#include <stack>
#include <string>

namespace gfx {

enum class precedence_t : uint8_t {
	assignment = 1,
	conditional,
	sum,
	product,
	logical,
	relational,
	exponent,
	bitwise,
	prefix,
	postfix,
	type,
	variable,
	call
};

class parser;

////////////////////////////////////////////////////////////////////////////////////////////////
/// 中缀
class infix_parser {
public:
	virtual ~infix_parser() = default;

	virtual ast_node_shared_ptr parse(result& r,parser* parser, const ast_node_shared_ptr& lhs, token_t& token) = 0;

	virtual precedence_t precedence() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class type_decl_infix_parser : public infix_parser {
public:
	type_decl_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

class variable_reference_infix_parser : public infix_parser {
public:
	variable_reference_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

class fn_call_infix_parser : public infix_parser {
public:
	fn_call_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
		token_t& token) override;

	precedence_t precedence() const override;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class binary_operator_infix_parser : public infix_parser {
public:
	binary_operator_infix_parser( precedence_t precedence, bool is_right_associative) noexcept;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;

private:
	precedence_t _precedence;
	bool _is_right_associative = false;
};


/////////////////////////////////////////////////////////////////////////////////////////////

class assignment_infix_parser : public infix_parser {
public:
	assignment_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

/////////////////////////////////////////////////////////////////////////////////////////////
/// 前缀
class prefix_parser {
public:
	virtual ~prefix_parser() = default;

	virtual ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////

class if_prefix_parser : public prefix_parser {
public:
	if_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

///////////////////////////////////////////////////////////////////////////

class type_decl_prefix_parser : public prefix_parser {
public:
	type_decl_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

///////////////////////////////////////////////////////////////////////////

class basic_block_prefix_parser : public prefix_parser {
public:
	basic_block_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class group_prefix_parser : public prefix_parser {
public:
	group_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class fn_decl_prefix_parser : public prefix_parser {
public:
	fn_decl_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};


class unary_operator_prefix_parser : public prefix_parser {
public:
	explicit unary_operator_prefix_parser(precedence_t precedence) noexcept;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;

private:
	precedence_t _precedence;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class symbol_literal_prefix_parser : public prefix_parser {
public:
	symbol_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

class number_literal_prefix_parser : public prefix_parser {
public:
	number_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r,parser* parser, token_t& token) override;
};

class string_literal_prefix_parser : public prefix_parser {
public:
	string_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

class char_literal_prefix_parser : public prefix_parser {
public:
	char_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};


class line_comment_prefix_parser : public prefix_parser {
public:
	line_comment_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

class block_comment_prefix_parser : public prefix_parser {
public:
	block_comment_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r,parser* parser, token_t& token) override;
};

class variable_decl_prefix_parser : public prefix_parser {
public:
	variable_decl_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

class attribute_prefix_parser : public prefix_parser  {
public:
	attribute_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};


class array_subscript_prefix_parser : public prefix_parser {
public:
	array_subscript_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class parser {
public:
	explicit parser(std::istream& source);

	virtual ~parser();

	bool peek(token_types_t type);

	bool consume();

	bool consume(token_t& token);

	bool look_ahead(size_t count);

	ast_builder* ast_builder();

	ast_node_shared_ptr parse(result& r);

	bool expect(result& r, token_t& token);

	ast_node_shared_ptr parse_expression(result& r, uint8_t precedence);

	void error(result& r, const std::string& code, const std::string& message, uint32_t line, uint32_t column);

	ast_node_shared_ptr parse_scope(result& r);
protected:

	ast_node_shared_ptr parse_statement(result& r);

private:
	uint8_t current_infix_precedence();

	static infix_parser* infix_parser_for(token_types_t type);

	static prefix_parser* prefix_parser_for(token_types_t type);

private:
	static inline if_prefix_parser s_if_prefix_parser {};
	static inline group_prefix_parser s_group_prefix_parser {};
	static inline fn_decl_prefix_parser s_fn_decl_prefix_parser {};
	static inline attribute_prefix_parser s_attribute_prefix_parser {};
	static inline type_decl_prefix_parser s_type_decl_prefix_parser {};
	static inline basic_block_prefix_parser s_basic_block_prefix_parser {};
	static inline char_literal_prefix_parser s_char_literal_prefix_parser {};
	static inline line_comment_prefix_parser s_line_comment_prefix_parser {};
	static inline variable_decl_prefix_parser s_variable_decl_prefix_parser {};
	static inline block_comment_prefix_parser s_block_comment_prefix_parser {};
	static inline string_literal_prefix_parser s_string_literal_prefix_parser {};
	static inline number_literal_prefix_parser s_number_literal_prefix_parser {};
	static inline symbol_literal_prefix_parser s_symbol_literal_prefix_parser {};
	static inline array_subscript_prefix_parser s_array_subscript_prefix_parser {};
	static inline unary_operator_prefix_parser s_negate_prefix_parser {precedence_t::sum};
	static inline unary_operator_prefix_parser s_not_prefix_parser {precedence_t::prefix};
	static inline unary_operator_prefix_parser s_binary_not_prefix_parser {precedence_t::prefix};

	static inline std::unordered_map<token_types_t, prefix_parser*> s_prefix_parsers = {
		{token_types_t::if_literal,          &s_if_prefix_parser},
		{token_types_t::bang,                &s_not_prefix_parser},
		{token_types_t::left_paren,          &s_group_prefix_parser},
		{token_types_t::minus,               &s_negate_prefix_parser},
		{token_types_t::fn_literal,          &s_fn_decl_prefix_parser},
		{token_types_t::colon,               &s_type_decl_prefix_parser},
		{token_types_t::attribute,           &s_attribute_prefix_parser},
		{token_types_t::tilde,               &s_binary_not_prefix_parser},
		{token_types_t::left_curly_brace,    &s_basic_block_prefix_parser},
		{token_types_t::character_literal,   &s_char_literal_prefix_parser},
		{token_types_t::line_comment,        &s_line_comment_prefix_parser},
		{token_types_t::block_comment,       &s_block_comment_prefix_parser},
		{token_types_t::identifier,          &s_variable_decl_prefix_parser},
		{token_types_t::none_literal,        &s_symbol_literal_prefix_parser},
		{token_types_t::null_literal,        &s_symbol_literal_prefix_parser},
		{token_types_t::break_literal,       &s_symbol_literal_prefix_parser},
		{token_types_t::continue_literal,    &s_symbol_literal_prefix_parser},
		{token_types_t::empty_literal,       &s_symbol_literal_prefix_parser},
		{token_types_t::string_literal,      &s_string_literal_prefix_parser},
		{token_types_t::number_literal,      &s_number_literal_prefix_parser},
		{token_types_t::true_literal,        &s_symbol_literal_prefix_parser},
		{token_types_t::false_literal,       &s_symbol_literal_prefix_parser},
		{token_types_t::left_square_bracket, &s_array_subscript_prefix_parser},
	};

	static inline fn_call_infix_parser s_fn_call_infix_parser {};
	static inline type_decl_infix_parser s_type_decl_infix_parser {};
	static inline assignment_infix_parser s_assignment_infix_parser {};
	static inline variable_reference_infix_parser s_variable_reference_infix_parser {};
	static inline binary_operator_infix_parser s_sum_binary_op_parser {precedence_t::sum, false};
	static inline binary_operator_infix_parser s_product_binary_op_parser {precedence_t::product, false};
	static inline binary_operator_infix_parser s_bitwise_binary_op_parser {precedence_t::bitwise, false};
	static inline binary_operator_infix_parser s_logical_binary_op_parser {precedence_t::logical, false};
	static inline binary_operator_infix_parser s_exponent_binary_op_parser {precedence_t::exponent, true};
	static inline binary_operator_infix_parser s_relational_binary_op_parser {precedence_t::relational, false};

	static inline std::unordered_map<token_types_t, infix_parser*> s_infix_parsers = {
		{token_types_t::left_paren,         &s_fn_call_infix_parser},
		{token_types_t::minus,              &s_sum_binary_op_parser},
		{token_types_t::plus,               &s_sum_binary_op_parser},
		{token_types_t::colon,              &s_type_decl_infix_parser},
		{token_types_t::assignment,         &s_assignment_infix_parser},
		{token_types_t::slash,              &s_product_binary_op_parser},
		{token_types_t::asterisk,           &s_product_binary_op_parser},
		{token_types_t::percent,            &s_product_binary_op_parser},
		{token_types_t::ampersand,          &s_bitwise_binary_op_parser},
		{token_types_t::pipe,               &s_bitwise_binary_op_parser},
		{token_types_t::logical_and,        &s_logical_binary_op_parser},
		{token_types_t::logical_or,         &s_logical_binary_op_parser},
		{token_types_t::caret,              &s_exponent_binary_op_parser},
		{token_types_t::equals,             &s_relational_binary_op_parser},
		{token_types_t::not_equals,         &s_relational_binary_op_parser},
		{token_types_t::less_than,          &s_relational_binary_op_parser},
		{token_types_t::less_than_equal,    &s_relational_binary_op_parser},
		{token_types_t::greater_than,       &s_relational_binary_op_parser},
		{token_types_t::greater_than_equal, &s_relational_binary_op_parser},
		{token_types_t::identifier,         &s_variable_reference_infix_parser},
	};

private:
	std::istream& source_;
	lexer lexer_;
	class ast_builder ast_builder_;
	std::vector<token_t> tokens_{};
};

}
#endif // PARSER_H_
