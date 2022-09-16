//
// Created by 12132 on 2022/4/1.
//

#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <common/source_location.h>
namespace gfx {

enum class token_types_t {
	invalid,
	plus,						/// '+'
	bang,						/// '!'
	pipe,						/// '|'
	minus,						/// '-'
	slash,						/// '/'
	caret,						/// '^'
	tilde,						/// '~'
	colon,						/// ':'
	comma,						/// ';'
	label,						///
	equals,						/// '='
	period,						/// '.'
	percent,					/// ''
	question,					/// '?'
	asterisk,					/// '*'
	ampersand,					/// ''
	attribute,					/// '@'
	directive,					/// '#'
	less_than,					/// '<'
	not_equals,					///	'!='
	left_paren,					/// ’(‘
	semi_colon,					/// ','
	in_literal,					/// 'in'
	if_literal,					/// 'if'
	identifier,					/// '[_a-zA-Z]'
	assignment,					/// '='
	logical_or,					/// '||'
	logical_and,				/// '&&'
	right_paren,				/// ')'
	for_literal,				/// 'for'
	end_of_file,				///
	xor_literal,				/// 'xor'
	shl_literal,				/// 'shl'
	shr_literal,				/// 'shr'
	rol_literal,				/// 'rol'
	ror_literal,				/// 'ror'
	with_literal,				/// 'with'
	greater_than,				/// '>'
	line_comment,				/// '//'
	true_literal,				/// 'true'
	enum_literal,				/// 'enum'
	proc_literal,				/// 'proc'
	null_literal,				/// 'null'
	else_literal,				/// 'else'
	cast_literal,				/// 'cast'
	block_comment,				/// '/* */'
	false_literal,				/// 'false'
	alias_literal,				/// 'alias'
	break_literal,				/// 'break'
	while_literal,				/// 'while'
	defer_literal,				/// 'defer'
	import_literal,				/// 'import'
    from_literal,				/// 'from'
	struct_literal,				/// 'struct'
	number_literal,				/// [0-9]*
	scope_operator,				/// '::'
	string_literal,				/// '"'
	union_literal,				/// 'union'
	return_literal,				/// 'return'
	less_than_equal,			/// '<'
	spread_operator,			/// ’...‘
	else_if_literal,			/// ’else if‘
	left_curly_brace,			/// '{'
	continue_literal,			/// 'continue'
	constant_literal,			/// 'constant'
    module_literal,			    /// 'module'
	right_curly_brace,			/// '}'
	character_literal,			/// '''
	namespace_literal,			/// 'ns'
	greater_than_equal,			/// '>='
	left_square_bracket,		/// '['
	right_square_bracket,		/// ']'
};

static inline std::unordered_map<token_types_t, std::string_view> s_type_to_name = {
	{token_types_t::invalid,                "invalid"},
	{token_types_t::comma,                  "comma"},
	{token_types_t::label,  				"label"},
	{token_types_t::equals,                 "equals"},
	{token_types_t::period, 				"period"},
	{token_types_t::percent,  				"percent"},
    {token_types_t::line_comment,           "comment"},
    {token_types_t::block_comment,          "comment"},
	{token_types_t::ampersand, 				"ampersand"},
	{token_types_t::assignment,             "assignment"},
	{token_types_t::not_equals,             "not_equals"},
	{token_types_t::directive,  			"directive"},
	{token_types_t::left_paren,             "left_paren"},
	{token_types_t::right_paren,            "right_paren"},
	{token_types_t::struct_literal,         "struct_literal"},
	{token_types_t::return_literal, 		"return_literal"},
	{token_types_t::left_curly_brace,       "left_curly_brace"},
	{token_types_t::right_curly_brace,      "right_curly_brace"},
	{token_types_t::left_square_bracket,    "left_square_bracket"},
	{token_types_t::right_square_bracket,   "right_square_bracket"},
	{token_types_t::greater_than,           "greater_than"},
	{token_types_t::greater_than_equal,     "greater_than_equal"},
	{token_types_t::less_than,              "less_than"},
	{token_types_t::less_than_equal,        "less_than_equal"},
	{token_types_t::logical_or,             "logical_or"},
	{token_types_t::logical_and,            "logical_and"},
	{token_types_t::xor_literal,            "xor_literal"},
	{token_types_t::shl_literal,            "shl_literal"},
	{token_types_t::shr_literal,            "shr_literal"},
	{token_types_t::rol_literal,            "rol_literal"},
	{token_types_t::ror_literal,            "ror_literal"},
	{token_types_t::plus,                   "plus"},
	{token_types_t::minus,                  "minus"},
	{token_types_t::asterisk,               "asterisk"},
	{token_types_t::slash,                  "slash"},
	{token_types_t::caret,                  "caret"},
	{token_types_t::tilde,                  "tilde"},
	{token_types_t::bang,                   "bang"},
	{token_types_t::question,               "question"},
	{token_types_t::colon,                  "colon"},
	{token_types_t::semi_colon,             "semi_colon"},
	{token_types_t::pipe,                   "pipe"},
	{token_types_t::scope_operator,         "scope_operator"},
	{token_types_t::spread_operator,        "spread_operator"},
	{token_types_t::identifier,             "identifier"},
	{token_types_t::attribute,              "attribute"},
	{token_types_t::string_literal,         "string_literal"},

	{token_types_t::alias_literal, 			"alias_literal"},
	{token_types_t::character_literal,      "character_literal"},
	{token_types_t::true_literal,           "true_literal"},
	{token_types_t::false_literal,          "false_literal"},
	{token_types_t::import_literal,         "import_literal"},
    {token_types_t::from_literal,           "from_literal"},
	{token_types_t::null_literal,           "null_literal"},
	{token_types_t::proc_literal,           "proc_literal"},
	{token_types_t::number_literal,         "number_literal"},
    {token_types_t::module_literal,         "module_literal"},
	{token_types_t::namespace_literal,      "namespace_literal"},
	{token_types_t::for_literal,            "for_literal"},
	{token_types_t::in_literal,             "in_literal"},
	{token_types_t::if_literal,             "if_literal"},
	{token_types_t::else_if_literal,        "else_if_literal"},
	{token_types_t::else_literal,           "else_literal"},
	{token_types_t::union_literal,          "union_literal"},
	{token_types_t::break_literal,          "break_literal"},
	{token_types_t::continue_literal,       "continue_literal"},
	{token_types_t::constant_literal, 		"constant_literal"},
	{token_types_t::while_literal,          "while_literal"},
	{token_types_t::cast_literal,           "cast_literal"},
	{token_types_t::defer_literal,          "defer_literal"},
	{token_types_t::end_of_file,            "end_of_file"},
	{token_types_t::with_literal, 			"with_literal"},
};

enum class conversion_result_t {
	success,
	overflow,
	underflow,
	inconvertible
};

enum class number_types_t {
	none,
	integer,
	floating_point,
};

struct token_t {
	[[nodiscard, maybe_unused]] bool as_bool() const;

	[[nodiscard]] bool is_label() const;

	[[nodiscard, maybe_unused]] bool is_boolean() const;

	[[nodiscard]] bool is_numeric() const;

	[[nodiscard]] std::string name() const;

	[[nodiscard]] bool is_line_comment() const;

	[[nodiscard]] bool is_block_comment() const;

	conversion_result_t parse(double& out) const;

	conversion_result_t parse(int64_t& out) const;

	conversion_result_t parse(uint64_t& out) const;

	token_types_t type = token_types_t::invalid;
	std::string value;
	uint8_t radix = 10;
	source_location location;
	number_types_t number_type = number_types_t::none;
};

}
#endif // TOKEN_H_
