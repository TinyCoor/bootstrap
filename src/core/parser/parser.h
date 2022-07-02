//
// Created by 12132 on 2022/3/30.
//
#ifndef PARSER_H_
#define PARSER_H_
#include "prefix_parser.h"
#include <filesystem>
#include "lexer.h"
#include <map>
#include <stack>
#include <string>

namespace gfx {
class parser {
public:
	explicit parser(std::istream& source);

	virtual ~parser();

	bool peek(token_types_t type);

	bool consume();

	bool consume(token_t& token);

	void write_ast_graph(const std::filesystem::path& path,
						 const ast_node_shared_ptr& program_node);

	bool look_ahead(size_t count);

	ast_builder* ast_builder();

	ast_node_shared_ptr parse(result& r);

	bool expect(result& r, token_t& token);

	ast_node_shared_ptr parse_expression(result& r, uint8_t precedence);

	void error(result& r, const std::string& code, const std::string& message, uint32_t line, uint32_t column);

	ast_node_shared_ptr expect_expression(result& r,ast_node_types_t expected_type, uint8_t precedence);

	ast_node_shared_ptr parse_scope(result& r);
protected:
	ast_node_shared_ptr parse_statement(result& r);

private:
	uint8_t current_infix_precedence();

	static infix_parser* infix_parser_for(token_types_t type);

	static prefix_parser* prefix_parser_for(token_types_t type);

private:
	static inline if_prefix_parser s_if_prefix_parser {};
	static inline with_prefix_parser s_with_prefix_parser{};
	static inline cast_prefix_parser s_cast_prefix_parser {};
	static inline enum_prefix_parser s_enum_prefix_parser {};
	static inline union_prefix_parser s_union_prefix_parser {};
	static inline group_prefix_parser s_group_prefix_parser {};
	static inline label_prefix_parser s_label_prefix_parser {};
	static inline struct_prefix_parser s_struct_prefix_parser {};
	static inline for_in_prefix_parser s_for_in_prefix_parser {};
	static inline return_prefix_parser s_return_prefix_parser {};
	static inline symbol_prefix_parser s_symbol_prefix_parser {};
	static inline constant_prefix_parser s_constant_prefix_parser {};
	static inline defer_prefix_parser s_defer_literal_prefix_parser {};
	static inline namespace_prefix_parser s_namespace_prefix_parser {};
	static inline attribute_prefix_parser s_attribute_prefix_parser {};
	static inline directive_prefix_parser s_directive_prefix_parser {};
	static inline basic_block_prefix_parser s_basic_block_prefix_parser {};
	static inline char_literal_prefix_parser s_char_literal_prefix_parser {};
	static inline line_comment_prefix_parser s_line_comment_prefix_parser {};
	static inline block_comment_prefix_parser s_block_comment_prefix_parser {};
	static inline string_literal_prefix_parser s_string_literal_prefix_parser {};
	static inline number_literal_prefix_parser s_number_literal_prefix_parser {};
	static inline keyword_literal_prefix_parser s_keyword_literal_prefix_parser {};
	static inline type_identifier_prefix_parser s_type_identifier_prefix_parser {};
	static inline array_subscript_prefix_parser s_array_subscript_prefix_parser {};
	static inline proc_expression_prefix_parser s_proc_expression_prefix_parser {};
	static inline unary_operator_prefix_parser s_negate_prefix_parser {precedence_t::sum};
	static inline unary_operator_prefix_parser s_not_prefix_parser {precedence_t::prefix};
	static inline unary_operator_prefix_parser s_binary_not_prefix_parser {precedence_t::prefix};

	static inline std::unordered_map<token_types_t, prefix_parser*> s_prefix_parsers = {
		{token_types_t::with_literal,   	 &s_with_prefix_parser},
		{token_types_t::if_literal,          &s_if_prefix_parser},
		{token_types_t::bang,                &s_not_prefix_parser},
		{token_types_t::label,               &s_label_prefix_parser},
		{token_types_t::cast_literal,        &s_cast_prefix_parser},
		{token_types_t::enum_literal,        &s_enum_prefix_parser},
		{token_types_t::left_paren,          &s_group_prefix_parser},
		{token_types_t::union_literal,       &s_union_prefix_parser},
		{token_types_t::struct_literal,      &s_struct_prefix_parser},
		{token_types_t::constant_literal,    &s_constant_prefix_parser},
		{token_types_t::for_literal,         &s_for_in_prefix_parser},
		{token_types_t::minus,               &s_negate_prefix_parser},
		{token_types_t::return_literal,      &s_return_prefix_parser},
		{token_types_t::identifier,          &s_symbol_prefix_parser},
		{token_types_t::namespace_literal,   &s_namespace_prefix_parser},
		{token_types_t::attribute,           &s_attribute_prefix_parser},
		{token_types_t::directive,           &s_directive_prefix_parser},
		{token_types_t::tilde,               &s_binary_not_prefix_parser},
		{token_types_t::left_curly_brace,    &s_basic_block_prefix_parser},
		{token_types_t::character_literal,   &s_char_literal_prefix_parser},
		{token_types_t::line_comment,        &s_line_comment_prefix_parser},
		{token_types_t::defer_literal, 		 &s_defer_literal_prefix_parser},
		{token_types_t::block_comment,       &s_block_comment_prefix_parser},
		{token_types_t::string_literal,      &s_string_literal_prefix_parser},
		{token_types_t::number_literal,      &s_number_literal_prefix_parser},
		{token_types_t::alias_literal,       &s_keyword_literal_prefix_parser},
		{token_types_t::null_literal,        &s_keyword_literal_prefix_parser},
		{token_types_t::break_literal,       &s_keyword_literal_prefix_parser},
		{token_types_t::continue_literal,    &s_keyword_literal_prefix_parser},
		{token_types_t::import_literal,      &s_keyword_literal_prefix_parser},
		{token_types_t::true_literal,        &s_keyword_literal_prefix_parser},
		{token_types_t::false_literal,       &s_keyword_literal_prefix_parser},
		{token_types_t::left_square_bracket, &s_array_subscript_prefix_parser},
		{token_types_t::colon,               &s_type_identifier_prefix_parser},
		{token_types_t::proc_literal,        &s_proc_expression_prefix_parser},
	};

	static inline cast_infix_parser s_cast_infix_parser {};
	static inline comma_infix_parser s_comma_infix_parser {};
	static inline symbol_infix_parser s_symbol_infix_parser {};
	static inline proc_call_infix_parser s_proc_call_infix_parser {};
	static inline assignment_infix_parser s_assignment_infix_parser {};
	static inline block_comment_infix_parser s_block_comment_infix_parser {};
	static inline type_identifier_infix_parser s_type_identifier_infix_parser {};
	static inline binary_operator_infix_parser s_sum_binary_op_parser {precedence_t::sum, false};
	static inline binary_operator_infix_parser s_product_binary_op_parser {precedence_t::product, false};
	static inline binary_operator_infix_parser s_bitwise_binary_op_parser {precedence_t::bitwise, false};
	static inline binary_operator_infix_parser s_logical_binary_op_parser {precedence_t::logical, false};
	static inline binary_operator_infix_parser s_exponent_binary_op_parser {precedence_t::exponent, true};
	static inline binary_operator_infix_parser s_relational_binary_op_parser {precedence_t::relational, false};

	static inline std::unordered_map<token_types_t, infix_parser*> s_infix_parsers = {
		{token_types_t::cast_literal,       &s_cast_infix_parser},
		{token_types_t::comma,              &s_comma_infix_parser},
		{token_types_t::identifier,         &s_symbol_infix_parser},
		{token_types_t::left_paren,         &s_proc_call_infix_parser},
		{token_types_t::minus,              &s_sum_binary_op_parser},
		{token_types_t::plus,               &s_sum_binary_op_parser},
		{token_types_t::assignment,         &s_assignment_infix_parser},
		{token_types_t::slash,              &s_product_binary_op_parser},
		{token_types_t::block_comment,      &s_block_comment_infix_parser},
		{token_types_t::asterisk,           &s_product_binary_op_parser},
		{token_types_t::percent,            &s_product_binary_op_parser},
		{token_types_t::ampersand,          &s_bitwise_binary_op_parser},
		{token_types_t::pipe,               &s_bitwise_binary_op_parser},
		{token_types_t::xor_literal,        &s_bitwise_binary_op_parser},
		{token_types_t::shl_literal,        &s_bitwise_binary_op_parser},
		{token_types_t::shr_literal,        &s_bitwise_binary_op_parser},
		{token_types_t::rol_literal,        &s_bitwise_binary_op_parser},
		{token_types_t::ror_literal,        &s_bitwise_binary_op_parser},
		{token_types_t::logical_and,        &s_logical_binary_op_parser},
		{token_types_t::logical_or,         &s_logical_binary_op_parser},
		{token_types_t::caret,              &s_exponent_binary_op_parser},
		{token_types_t::equals,             &s_relational_binary_op_parser},
		{token_types_t::not_equals,         &s_relational_binary_op_parser},
		{token_types_t::less_than,          &s_relational_binary_op_parser},
		{token_types_t::less_than_equal,    &s_relational_binary_op_parser},
		{token_types_t::greater_than,       &s_relational_binary_op_parser},
		{token_types_t::greater_than_equal, &s_relational_binary_op_parser},
		{token_types_t::colon,              &s_type_identifier_infix_parser},
	};

private:
	std::istream& source_;
	lexer lexer_;
	class ast_builder ast_builder_;
	std::vector<token_t> tokens_{};
};

}
#endif // PARSER_H_
