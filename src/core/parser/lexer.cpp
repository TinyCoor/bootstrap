//
// Created by 12132 on 2022/4/3.
//

#include "lexer.h"
#include <sstream>

namespace gfx {

static inline token_t s_invalid = {
    .type = token_types_t::invalid,
    .value = ""
};

static inline token_t s_proc_literal = {
	.type = token_types_t::proc_literal,
	.value = "proc"
};

static inline token_t s_from_literal = {
    .type = token_types_t::from_literal,
    .value = "from"
};

static inline token_t s_if_literal = {
	.type = token_types_t::if_literal,
	.value = "if"
};

static inline token_t s_module_literal = {
    .type = token_types_t::module_literal,
    .value = "module"
};

static inline token_t s_in_literal = {
	.type = token_types_t::in_literal,
	.value = "in"
};

static inline token_t s_for_literal = {
	.type = token_types_t::for_literal,
	.value = "for"
};

static inline token_t s_else_literal = {
	.type = token_types_t::else_literal,
	.value = "else"
};

static inline token_t s_block_comment = {
	.type = token_types_t::block_comment,
	.value = "/*"
};

static inline token_t s_bang_literal = {
	.type = token_types_t::bang,
	.value = "!"
};

static inline token_t s_plus_literal = {
	.type = token_types_t::plus,
	.value = "+"
};

static inline token_t s_import_literal = {
	.type = token_types_t::import_literal,
	.value = "import"
};

static inline token_t s_with_literal = {
	.type = token_types_t::with_literal,
	.value = "with"
};

static inline token_t s_pipe_literal = {
	.type = token_types_t::pipe,
	.value = "|"
};

static inline token_t s_colon_literal = {
	.type = token_types_t::colon,
	.value = ":"
};

static inline token_t s_end_of_file = {
	.type = token_types_t::end_of_file,
	.value = ""
};

static inline token_t s_enum_literal = {
	.type = token_types_t::enum_literal,
	.value = "enum"
};

static inline token_t s_cast_literal = {
	.type = token_types_t::cast_literal,
	.value = "cast"
};

static inline token_t s_null_literal = {
	.type = token_types_t::null_literal,
	.value = "null"
};


static inline token_t s_true_literal = {
	.type = token_types_t::true_literal,
	.value = "true"
};

static inline token_t s_minus_literal = {
	.type = token_types_t::minus,
	.value = "-"
};

static inline token_t s_slash_literal = {
	.type = token_types_t::slash,
	.value = "/"
};

static inline token_t s_comma_literal = {
	.type = token_types_t::comma,
	.value = ","
};

static inline token_t s_defer_literal = {
	.type = token_types_t::defer_literal,
	.value = "defer"
};

static inline token_t s_break_literal = {
	.type = token_types_t::break_literal,
	.value = "break"
};

static inline token_t s_false_literal = {
	.type = token_types_t::false_literal,
	.value = "false"
};

static inline token_t s_while_literal = {
	.type = token_types_t::while_literal,
	.value = "while"
};

static inline token_t s_alias_literal = {
	.type = token_types_t::alias_literal,
	.value = "alias"
};

static inline token_t s_union_literal = {
	.type = token_types_t::union_literal,
	.value = "union"
};

static inline token_t s_caret_literal = {
	.type = token_types_t::caret,
	.value = "^"
};

static inline token_t s_tilde_literal = {
	.type = token_types_t::tilde,
	.value = "~"
};

static inline token_t s_period_literal = {
	.type = token_types_t::period,
	.value = "."
};

static inline token_t s_struct_literal = {
	.type = token_types_t::struct_literal,
	.value = "struct"
};

static inline token_t s_return_literal = {
	.type = token_types_t::return_literal,
	.value = "return"
};

static inline token_t s_else_if_literal = {
	.type = token_types_t::else_if_literal,
	.value = "else if"
};

static inline token_t s_percent_literal = {
	.type = token_types_t::percent,
	.value = "%"
};

static inline token_t s_constant_literal = {
	.type = token_types_t::constant_literal,
	.value = "constant"
};

static inline token_t s_continue_literal = {
	.type = token_types_t::continue_literal,
	.value = "continue"
};

static inline token_t s_asterisk_literal = {
	.type = token_types_t::asterisk,
	.value = "*"
};

static inline token_t s_question_literal = {
	.type = token_types_t::question,
	.value = "?"
};

static inline token_t s_namespace_literal = {
	.type = token_types_t::namespace_literal,
	.value = "ns"
};

static inline token_t s_assignment_literal = {
	.type = token_types_t::assignment,
	.value = ":="
};

static inline token_t s_ampersand_literal = {
	.type = token_types_t::ampersand,
	.value = "&"
};

static inline token_t s_left_paren_literal = {
	.type = token_types_t::left_paren,
	.value = "("
};

static inline token_t s_logical_or_literal = {
	.type = token_types_t::logical_or,
	.value = "||"
};

static inline token_t s_semi_colon_literal = {
	.type = token_types_t::semi_colon,
	.value = ";"
};

static inline token_t s_right_paren_literal = {
	.type = token_types_t::right_paren,
	.value = ")"
};

static inline token_t s_logical_and_literal = {
	.type = token_types_t::logical_and,
	.value = "&&"
};

static inline token_t s_equals_literal = {
	.type = token_types_t::equals,
	.value = "=="
};

static inline token_t s_not_equals_literal = {
	.type = token_types_t::not_equals,
	.value = "!="
};

static inline token_t s_greater_than_literal = {
	.type = token_types_t::greater_than,
	.value = ">"
};

static inline token_t s_less_than_literal = {
	.type = token_types_t::less_than,
	.value = "<"
};

static inline token_t s_greater_than_equal_literal = {
	.type = token_types_t::greater_than_equal,
	.value = ">="
};

static inline token_t s_less_than_equal_literal = {
	.type = token_types_t::less_than,
	.value = "<="
};

static inline token_t s_scope_operator_literal = {
	.type = token_types_t::scope_operator,
	.value = "::"
};

static inline token_t s_spread_operator_literal = {
	.type = token_types_t::spread_operator,
	.value = "..."
};

static inline token_t s_left_curly_brace_literal = {
	.type = token_types_t::left_curly_brace,
	.value = "{"
};

static inline token_t s_right_curly_brace_literal = {
	.type = token_types_t::right_curly_brace,
	.value = "}"
};

static inline token_t s_left_square_bracket_literal = {
	.type = token_types_t::left_square_bracket,
	.value = "["
};

static inline token_t s_right_square_bracket_literal = {
	.type = token_types_t::right_square_bracket,
	.value = "]"
};

static inline token_t s_xor_literal = {
	.type = token_types_t::xor_literal,
	.value = "xor"
};

static inline token_t s_shl_literal = {
	.type = token_types_t::shl_literal,
	.value = "shl"
};

static inline token_t s_shr_literal = {
	.type = token_types_t::shr_literal,
	.value = "shr"
};

static inline token_t s_rol_literal = {
	.type = token_types_t::rol_literal,
	.value = "rol"
};

static inline token_t s_ror_literal = {
	.type = token_types_t::ror_literal,
	.value = "ror"
};

/// todo use lambda and unorder_map
std::multimap<char, lexer::lexer_case_callable> lexer::s_cases = {
	// attribute
	{'@', std::bind(&lexer::attribute, std::placeholders::_1, std::placeholders::_2)},

	// add
	{'+', std::bind(&lexer::plus, std::placeholders::_1, std::placeholders::_2)},

	// directive
	{'#', std::bind(&lexer::directive, std::placeholders::_1, std::placeholders::_2)},

	// minus, negate
	{'-', std::bind(&lexer::minus, std::placeholders::_1, std::placeholders::_2)},

	// block comment line comment, slash
	{'/', std::bind(&lexer::block_comment, std::placeholders::_1, std::placeholders::_2)},
	{'/', std::bind(&lexer::line_comment, std::placeholders::_1, std::placeholders::_2)},
	{'/', std::bind(&lexer::slash, std::placeholders::_1, std::placeholders::_2)},

	// comma
	{',', std::bind(&lexer::comma, std::placeholders::_1, std::placeholders::_2)},

	// caret
	{'^', std::bind(&lexer::caret, std::placeholders::_1, std::placeholders::_2)},

	// bang
	{'!', std::bind(&lexer::not_equals_operator, std::placeholders::_1, std::placeholders::_2)},
	{'!', std::bind(&lexer::bang, std::placeholders::_1, std::placeholders::_2)},

	// question
	{'?', std::bind(&lexer::question, std::placeholders::_1, std::placeholders::_2)},

	// spread
	// period/spread
	{'.', std::bind(&lexer::period, std::placeholders::_1, std::placeholders::_2)},
	{'.', std::bind(&lexer::spread, std::placeholders::_1, std::placeholders::_2)},

	// tilde
	{'~', std::bind(&lexer::tilde, std::placeholders::_1, std::placeholders::_2)},

	// assignment, scope operator, colon
	{':', std::bind(&lexer::assignment, std::placeholders::_1, std::placeholders::_2)},
	{':', std::bind(&lexer::scope_operator, std::placeholders::_1, std::placeholders::_2)},
	{':', std::bind(&lexer::colon, std::placeholders::_1, std::placeholders::_2)},

	// percent/number literal
	{'%', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'%', std::bind(&lexer::percent, std::placeholders::_1, std::placeholders::_2)},

	// asterisk
	{'*', std::bind(&lexer::asterisk, std::placeholders::_1, std::placeholders::_2)},

	// equals, not equals
	{'=', std::bind(&lexer::equals_operator, std::placeholders::_1, std::placeholders::_2)},

	// less than equal, less than
	{'<', std::bind(&lexer::less_than_equal_operator, std::placeholders::_1, std::placeholders::_2)},
	{'<', std::bind(&lexer::less_than_operator, std::placeholders::_1, std::placeholders::_2)},

	// greater than equal, greater than
	{'>', std::bind(&lexer::greater_than_equal_operator, std::placeholders::_1, std::placeholders::_2)},
	{'>', std::bind(&lexer::greater_than_operator, std::placeholders::_1, std::placeholders::_2)},

	// logical and, bitwise and, ampersand
	{'&', std::bind(&lexer::logical_and_operator, std::placeholders::_1, std::placeholders::_2)},
	{'&', std::bind(&lexer::ampersand_literal, std::placeholders::_1, std::placeholders::_2)},

	// logical or, bitwise or, pipe
	{'|', std::bind(&lexer::logical_or_operator, std::placeholders::_1, std::placeholders::_2)},
	{'|', std::bind(&lexer::pipe_literal, std::placeholders::_1, std::placeholders::_2)},

	// braces
	{'{', std::bind(&lexer::left_curly_brace, std::placeholders::_1, std::placeholders::_2)},
	{'}', std::bind(&lexer::right_curly_brace, std::placeholders::_1, std::placeholders::_2)},

	// return literal
	{'r', std::bind(&lexer::return_literal, std::placeholders::_1, std::placeholders::_2)},

	// parens
	{'(', std::bind(&lexer::left_paren, std::placeholders::_1, std::placeholders::_2)},
	{')', std::bind(&lexer::right_paren, std::placeholders::_1, std::placeholders::_2)},

	// square brackets
	{'[', std::bind(&lexer::left_square_bracket, std::placeholders::_1, std::placeholders::_2)},
	{']', std::bind(&lexer::right_square_bracket, std::placeholders::_1, std::placeholders::_2)},

	// line terminator
	{';', std::bind(&lexer::line_terminator, std::placeholders::_1, std::placeholders::_2)},

	// character literal
	{'\'', std::bind(&lexer::label, std::placeholders::_1, std::placeholders::_2)},
	{'\'', std::bind(&lexer::character_literal, std::placeholders::_1, std::placeholders::_2)},

	// string literal
	{'"', std::bind(&lexer::string_literal, std::placeholders::_1, std::placeholders::_2)},

	// true/false literals
	{'t', std::bind(&lexer::true_literal, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::false_literal, std::placeholders::_1, std::placeholders::_2)},

	// null/none/ns/empty literals
	{'n', std::bind(&lexer::null_literal, std::placeholders::_1, std::placeholders::_2)},
	{'n', std::bind(&lexer::ns_literal, std::placeholders::_1, std::placeholders::_2)},


	// enum literal
	// else if/else literals
	{'e', std::bind(&lexer::enum_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::else_if_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::else_literal, std::placeholders::_1, std::placeholders::_2)},
	// union literal
	{'u', std::bind(&lexer::union_literal, std::placeholders::_1, std::placeholders::_2)},

	// if
	// in literal
	{'i', std::bind(&lexer::import_literal, std::placeholders::_1, std::placeholders::_2)},
	{'i', std::bind(&lexer::if_literal, std::placeholders::_1, std::placeholders::_2)},
	{'i', std::bind(&lexer::in_literal, std::placeholders::_1, std::placeholders::_2)},

    // module literals
    {'m', std::bind(&lexer::module_literal, std::placeholders::_1, std::placeholders::_2)},

	// fn literal
	// for literal
	{'f', std::bind(&lexer::from_literal, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::for_literal, std::placeholders::_1, std::placeholders::_2)},

    // proc literal
    {'p', std::bind(&lexer::proc_literal, std::placeholders::_1, std::placeholders::_2)},

	// break literal
	{'b', std::bind(&lexer::break_literal, std::placeholders::_1, std::placeholders::_2)},

	// defer literal
	{'d', std::bind(&lexer::defer_literal, std::placeholders::_1, std::placeholders::_2)},

	// continue literal
	// read_only literal
	// cast literal
	{'c', std::bind(&lexer::continue_literal, std::placeholders::_1, std::placeholders::_2)},
	{'c', std::bind(&lexer::constant_literal, std::placeholders::_1, std::placeholders::_2)},
	{'c', std::bind(&lexer::cast_literal, std::placeholders::_1, std::placeholders::_2)},

	// alias literal
	{'a', std::bind(&lexer::alias_literal, std::placeholders::_1, std::placeholders::_2)},

	// proc literal
	{'p', std::bind(&lexer::proc_literal, std::placeholders::_1, std::placeholders::_2)},

	// while literal
	{'w', std::bind(&lexer::while_literal, std::placeholders::_1, std::placeholders::_2)},
	{'w', std::bind(&lexer::with_literal, std::placeholders::_1, std::placeholders::_2)},

	// rol/ror literal
	{'r', std::bind(&lexer::rol_literal, std::placeholders::_1, std::placeholders::_2)},
	{'r', std::bind(&lexer::ror_literal, std::placeholders::_1, std::placeholders::_2)},

	// struct/shl/shr literal
	{'s', std::bind(&lexer::struct_literal, std::placeholders::_1, std::placeholders::_2)},
	{'s', std::bind(&lexer::shl_literal, std::placeholders::_1, std::placeholders::_2)},
	{'s', std::bind(&lexer::shr_literal, std::placeholders::_1, std::placeholders::_2)},

	// xor literal
	{'x', std::bind(&lexer::xor_literal, std::placeholders::_1, std::placeholders::_2)},

	// identifier
	{'_', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'a', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'b', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'c', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'d', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'g', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'h', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'i', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'j', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'k', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'l', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'m', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'n', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'o', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'p', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'q', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'r', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'s', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'t', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'u', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'v', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'w', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'x', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'y', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},
	{'z', std::bind(&lexer::identifier, std::placeholders::_1, std::placeholders::_2)},

	// number literal
	{'_', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'$', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'%', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'@', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'0', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'1', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'2', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'3', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'4', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'5', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'6', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'7', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'8', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'9', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'a', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'b', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'c', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'d', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::number_literal, std::placeholders::_1, std::placeholders::_2)},

};

lexer::lexer(source_file* source)
	: source_(source)
{
}

rune_t lexer::peek()
{
	while (!source_->eof()) {
		auto ch = static_cast<char>(source_->next());
		if (!isspace(ch)) {
			return ch;
		}
	}
	return 0;
}

bool lexer::has_next() const
{
	return has_next_;
}

void lexer::rewind_one_char()
{
    auto pos = source_->pos();
    if (pos == 0) {
        return;
    }
    source_->seek(pos - 1);
}

bool lexer::next(token_t& token)
{	if (source_->eof()) {
        has_next_ = false;
        token = s_end_of_file;
        set_token_location(token);
        return true;
    }

    const auto ch = tolower(read());
    rewind_one_char();
    source_->push_mark();

	auto case_range = s_cases.equal_range(ch);
	for (auto it = case_range.first; it != case_range.second; ++it) {
		token.radix = 10;
		token.number_type = number_types_t::none;
        auto start_column = source_->column_by_index(source_->pos());
        auto start_line = source_->line_by_index(source_->pos());
		if (it->second(this, token)) {
            auto end_column = source_->column_by_index(source_->pos());
            auto end_line = source_->line_by_index(source_->pos());
            token.location.start(start_line->line, start_column);
            token.location.end(end_line->line, end_column);
//            fmt::print("token.type = {}, start = {}@{}, end = {}@{}\n",
//                token.name(),
//                token.location.start().line,
//                token.location.start().column,
//                token.location.end().line,
//                token.location.end().column);
            return true;
		}
        source_->restore_top_mark();
	}
    source_->pop_mark();
	token = s_invalid;
    token.value = ch;
    set_token_location(token);
	has_next_ = false;

	return true;
}

rune_t lexer::read(bool skip_whitespace)
{
	while (true) {
		auto ch = source_->next();
		if (skip_whitespace && isspace(ch)) {
            continue;
        }
		return ch;
	}
}

std::string lexer::read_identifier()
{
	auto ch = read(false);
	if (ch != '_' && !isalpha(ch)) {
		return "";
	}
	std::stringstream stream;
	stream << static_cast<char>(ch);
	while (true) {
		ch = read(false);
		if (ch == ';') {
			return stream.str();
		}
		if (ch == '_' || isalnum(ch)) {
			stream << static_cast<char>(ch);
			continue;
		}
		return stream.str();
	}
}

bool lexer::alias_literal(token_t& token)
{
	if (match_literal("alias")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_alias_literal;
			return true;
		}
	}
	return false;
}

bool lexer::break_literal(token_t& token)
{
	if (match_literal("break")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_break_literal;
			return true;
		}
	}
	return false;
}

bool lexer::while_literal(token_t& token)
{
	if (match_literal("while")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_while_literal;
			return true;
		}
	}
	return false;
}

bool lexer::union_literal(token_t &token)
{
	if (match_literal("union")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_union_literal;
			return true;
		}
	}
	return false;
}

bool lexer::continue_literal(token_t& token)
{
	if (match_literal("continue")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_continue_literal;
			return true;
		}
	}
	return false;
}

bool lexer::left_curly_brace(token_t& token)
{
	auto ch = read();
	if (ch == '{') {
		token = s_left_curly_brace_literal;
		return true;
	}
	return false;
}

bool lexer::right_curly_brace(token_t& token)
{
	auto ch = read();
	if (ch == '}') {
		token = s_right_curly_brace_literal;
		return true;
	}
	return false;
}

std::string lexer::read_until(char target_ch)
{
	std::stringstream stream;
	while (true) {
		auto ch = read(false);
		if (ch == target_ch) {
			break;
		}
		stream << static_cast<char>(ch);
	}
	return stream.str();
}

bool lexer::plus(token_t& token)
{
	auto ch = read();
	if (ch == '+') {
		token =s_plus_literal;
		return true;
	}
	return false;
}

bool lexer::bang(token_t& token)
{
	auto ch = read();
	if (ch == '!') {
		token = s_bang_literal;
		return true;
	}
	return false;
}

bool lexer::caret(token_t& token)
{
	auto ch = read();
	if (ch == '^') {
		token = s_caret_literal;
		return true;
	}
	return false;
}

bool lexer::tilde(token_t& token)
{
	auto ch = read();
	if (ch == '~') {
		token = s_tilde_literal;
		return true;
	}
	return false;
}

bool lexer::colon(token_t& token)
{
	auto ch = read();
	if (ch == ':') {
		token = s_colon_literal;
		return true;
	}
	return false;
}

bool lexer::minus(token_t& token)
{
	auto ch = read();
	if (ch == '-') {
		token = s_minus_literal;
		return true;
	}
	return false;
}

bool lexer::comma(token_t& token)
{
	auto ch = read();
	if (ch == ',') {
		token = s_comma_literal;
		return true;
	}
	return false;
}

bool lexer::slash(token_t& token)
{
	auto ch = read();
	if (ch == '/') {
		token = s_slash_literal;
		return true;
	}
	return false;
}

bool lexer::spread(token_t& token)
{
	if (match_literal("...")) {
		token = s_spread_operator_literal;
		return true;
	}
	return false;
}

bool lexer::percent(token_t& token)
{
	auto ch = read();
	if (ch == '%') {
		token =s_percent_literal;
		return true;
	}
	return false;
}

bool lexer::question(token_t& token)
{
	auto ch = read();
	if (ch == '?') {
		token = s_question_literal;
		return true;
	}
	return false;
}

bool lexer::asterisk(token_t& token)
{
	auto ch = read();
	if (ch == '*') {
		token = s_asterisk_literal;
		return true;
	}
	return false;
}

bool lexer::attribute(token_t& token)
{
	auto ch = read();
	if (ch == '@') {
		token.value = read_identifier();
		rewind_one_char();
		if (token.value.empty()) {
			return false;
		}
		token.type = token_types_t::attribute;
		return true;
	}
	return false;
}

bool lexer::identifier(token_t& token)
{
	auto name = read_identifier();
	if (name.empty()) {
		return false;
	}

	rewind_one_char();

	token.type = token_types_t::identifier;
	token.value = name;
	return true;
}

bool lexer::assignment(token_t& token)
{
	auto ch = read();
	if (ch == ':') {
		ch = read();
		if (ch == '=') {
			token =s_assignment_literal;
			return true;
		}
	}
	return false;
}

bool lexer::left_paren(token_t& token)
{
	auto ch = read();
	if (ch == '(') {
		token =s_left_paren_literal;
		return true;
	}
	return false;
}

bool lexer::in_literal(token_t& token)
{
	auto ch = read();
	if (ch == 'i') {
		ch = read();
		if (ch == 'n') {
			token = s_in_literal;
			return true;
		}
	}
	return false;
}

bool lexer::right_paren(token_t& token)
{
	auto ch = read();
	if (ch == ')') {
		token = s_right_paren_literal;
		return true;
	}
	return false;
}

bool lexer::proc_literal(token_t &token)
{
	if (match_literal("proc")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_proc_literal;
			return true;
		}
	}
	return false;
}

bool lexer::ns_literal(token_t& token)
{
	if (match_literal("ns")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_namespace_literal;
			return true;
		}
	}
	return false;
}

bool lexer::if_literal(token_t& token)
{
	if (match_literal("if")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_if_literal;
			return true;
		}
	}
	return false;
}

bool lexer::else_literal(token_t& token)
{
	if (match_literal("else")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_else_literal;
			return true;
		}
	}
	return false;
}

bool lexer::line_comment(token_t& token)
{
	auto ch = read();
	if (ch == '/') {
		ch = read();
		if (ch == '/') {
			token.type = token_types_t::line_comment;
			token.value = read_until('\n');
			rewind_one_char();
			return true;
		}
	}
	return false;
}

bool lexer::for_literal(token_t& token)
{
	if (match_literal("for")) {
		auto ch = read(false);
		if (isspace(ch)) {
			rewind_one_char();
			token= s_for_literal;
			return true;
		}
	}
	return false;
}

bool lexer::null_literal(token_t& token)
{
	if (match_literal("null")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token =s_null_literal;
			return true;
		}
	}
	return false;
}

bool lexer::cast_literal(token_t& token)
{
	if (match_literal("cast")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token =s_cast_literal;
			return true;
		}
	}
	return false;
}

bool lexer::true_literal(token_t& token)
{
	if (match_literal("true")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_true_literal;
			return true;
		}
	}
	return false;
}

bool lexer::pipe_literal(token_t& token)
{
	auto ch = read();
	if (ch == '|') {
		token = s_pipe_literal;
		return true;
	}
	return false;
}

bool lexer::string_literal(token_t& token)
{
	auto ch = read();
	if (ch == '\"') {
		token.type = token_types_t::string_literal;
		token.value = read_until('"');
		return true;
	}
	return false;
}

bool lexer::false_literal(token_t& token)
{
	if (match_literal("false")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_false_literal;
			return true;
		}
	}
	return false;
}

bool lexer::defer_literal(token_t& token)
{
	if (match_literal("defer")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token =s_defer_literal;
			return true;
		}
	}
	return false;
}

bool lexer::number_literal(token_t& token)
{
	std::stringstream stream;
	token.type = token_types_t::number_literal;
	token.number_type= number_types_t::integer;
	auto ch = read();
	if (ch == '$') {
		token.radix = 16;
		while (true) {
			ch = read();
			if (ch == '_') {
				continue;
			}

			if (!std::isxdigit(ch)) {
				break;
			}

			stream << static_cast<char> (ch);
		}
	} else if (ch == '@') {
		const std::string valid = "012345678";
		token.radix = 8;
		while (true) {
			ch = read();
			if (ch == '_') {
				continue;
			}

			if (valid.find_first_of(static_cast<char>(ch)) == std::string::npos) {
				break;
			}
            stream << static_cast<char> (ch);
		}
	} else if (ch == '%') {
		token.radix = 2;
		while (true) {
			ch = read();
			if (ch == '_') {
				continue;
			}
			if (ch != '0' && ch != '1') {
				break;
			}
            stream << static_cast<char> (ch);
		}
	} else {
		const std::string valid = "0123456789_.";
		while (valid.find_first_of(static_cast<char>(ch)) != std::string::npos) {
			if (ch!='_') {
				if (ch=='.') {
					token.number_type = number_types_t::floating_point;
				}
                stream << static_cast<char> (ch);
			}
			ch = read();
		}
	}

	token.value = stream.str();
	if (token.value.empty()) {
		return false;
	}

	rewind_one_char();

	return true;
}

bool lexer::scope_operator(token_t& token) {
	auto ch = read();
	if (ch == ':') {
		ch = read();
		if (ch == ':') {
			token  = s_scope_operator_literal;
			return true;
		}
	}
	return false;
}

bool lexer::line_terminator(token_t& token)
{
	auto ch = read();
	if (ch == ';') {
		token = s_semi_colon_literal;
		return true;
	}
	return false;
}

bool lexer::equals_operator(token_t& token)
{
	auto ch = read();
	if (ch == '=') {
		ch = read();
		if (ch == '=') {
			token = s_equals_literal;
			return true;
		}
	}
	return false;
}

bool lexer::character_literal(token_t& token)
{
	auto ch = read();
	if (ch == '\'') {
		auto value = read();
		ch = read();
		if (ch == '\'') {
			token.type = token_types_t::character_literal;
			token.value = value;
			return true;
		}
	}
	return false;
}


bool lexer::ampersand_literal(token_t& token)
{
	auto ch = read();
	if (ch == '&') {
		token = s_ampersand_literal;
		return true;
	}
	return false;
}

bool lexer::less_than_operator(token_t& token)
{
	auto ch = read();
	if (ch == '<') {
		token = s_less_than_literal;
		return true;
	}
	return false;
}

bool lexer::logical_or_operator(token_t& token)
{
	auto ch = read();
	if (ch == '|') {
		ch = read();
		if (ch == '|') {
			token = s_logical_or_literal;
			return true;
		}
	}
	return false;
}

bool lexer::logical_and_operator(token_t& token)
{
	auto ch = read();
	if (ch == '&') {
		ch = read();
		if (ch == '&') {
			token = s_logical_and_literal;
			return true;
		}
	}
	return false;
}

bool lexer::not_equals_operator(token_t& token)
{
	auto ch = read();
	if (ch == '!') {
		ch = read();
		if (ch == '=') {
			token = s_not_equals_literal;
			return true;
		}
	}
	return false;
}

bool lexer::left_square_bracket(token_t& token)
{
	auto ch = read();
	if (ch == '[') {
		token = s_left_square_bracket_literal;
		return true;
	}
	return false;
}

bool lexer::right_square_bracket(token_t& token)
{
	auto ch = read();
	if (ch == ']') {
		token= s_right_square_bracket_literal;
		return true;
	}
	return false;
}

bool lexer::greater_than_operator(token_t& token)
{
	auto ch = read();
	if (ch == '>') {
		token = s_greater_than_literal;
		return true;
	}
	return false;
}

bool lexer::less_than_equal_operator(token_t& token)
{
	auto ch = read();
	if (ch == '<') {
		ch = read();
		if (ch == '=') {
			token = s_less_than_equal_literal;
			return true;
		}
	}
	return false;
}

bool lexer::match_literal(const std::string& literal)
{
	auto ch = read();
	for (size_t i = 0; i < literal.length(); ++i) {
		const auto& target_ch = literal[i];
		if (target_ch != ch) {
			return false;
		}
		ch = read(false);
	}
	rewind_one_char();
	return true;
}

bool lexer::greater_than_equal_operator(token_t& token)
{
	auto ch = read();
	if (ch == '>') {
		ch = read();
		if (ch == '=') {
			token = s_greater_than_equal_literal;
			return true;
		}
	}
	return false;
}

bool lexer::directive(token_t &token)
{
	auto ch = read();
	if (ch == '#') {
		token.value = read_identifier();
		if (token.value.empty()) {
			return false;
		}
		token.type = token_types_t::directive;
		return true;
	}
	return false;
}

bool lexer::enum_literal(token_t &token)
{
	if (match_literal("enum")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_enum_literal;
			return true;
		}
	}
	return false;
}
bool lexer::struct_literal(token_t &token)
{
	if (match_literal("struct")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_struct_literal;
			return true;
		}
	}
	return false;
}
bool lexer::return_literal(token_t &token)
{
	if (match_literal("return")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_return_literal;
			return true;
		}
	}
	return false;
}

bool lexer::constant_literal(token_t &token)
{
	if (match_literal("constant")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_constant_literal;
			return true;
		}
	}
	return false;
}

bool lexer::else_if_literal(token_t& token)
{
	if (match_literal("else if")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_else_if_literal;
			return true;
		}
	}
	return false;
}

bool lexer::period(token_t &token)
{
	auto ch = read();
	if (ch == '.') {
		ch = read();
		if (ch != '.') {
			rewind_one_char();
			token =s_period_literal;
			return true;
		}
	}
	return false;
}

bool lexer::with_literal(token_t& token)
{
	if (match_literal("with")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_with_literal;
			return true;
		}
	}
	return false;
}

bool lexer::block_comment(token_t &token)
{
	if (match_literal("/*")) {
		auto block_count = 1;
		token = s_block_comment;
		std::stringstream stream;
		while (true) {
			if (source_->eof()) {
				token = s_end_of_file;
                set_token_location(token);
				return true;
			}

			auto ch = read(false);
			if (ch == '/') {
				ch = read(false);
				if (ch == '*') {
					block_count++;
					continue;
				} else {
					rewind_one_char();
					ch = read(false);
				}
			} else if (ch == '*') {
				ch = read(false);
				if (ch == '/') {
					block_count--;
					if (block_count == 0) {
                        break;
                    }
					continue;
				} else {
					rewind_one_char();
					ch = read(false);
				}
			}
			stream << ch;
		}

		token.value = stream.str();
		return true;
	}
	return false;
}

bool lexer::label(token_t &token)
{
	auto ch = read();
	if (ch == '\'') {
		auto identifier = read_identifier();
		if (identifier.empty()) {
			return false;
		}
		rewind_one_char();
		ch = read(false);
		if (ch == ':') {
			token.type = token_types_t::label;
			token.value = identifier;
			return true;
		}
	}
	return false;
}

bool lexer::xor_literal(token_t &token)
{
	if (match_literal("xor")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_xor_literal;
			return true;
		}
	}
	return false;
}

bool lexer::shl_literal(token_t &token)
{
	if (match_literal("shl")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_shl_literal;
			return true;
		}
	}
	return false;
}

bool lexer::shr_literal(token_t &token)
{
	if (match_literal("shr")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_shr_literal;
			return true;
		}
	}
	return false;
}

bool lexer::rol_literal(token_t &token)
{
	if (match_literal("rol")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_rol_literal;
			return true;
		}
	}
	return false;
}

bool lexer::ror_literal(token_t &token)
{
	if (match_literal("ror")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_ror_literal;
			return true;
		}
	}
	return false;
}
bool lexer::import_literal(token_t &token)
{
	if (match_literal("import")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_import_literal;
			return true;
		}
	}
	return false;
}

bool lexer::from_literal(token_t &token)
{
    if (match_literal("from")) {
        auto ch = read(false);
        if (!isalnum(ch)) {
            rewind_one_char();
            token = s_from_literal;
            return true;
        }
    }
    return false;
}

void lexer::set_token_location(token_t& token)
{
    auto column = source_->column_by_index(source_->pos());
    auto source_line = source_->line_by_index(source_->pos());
    token.location.end(source_line->line, column);
    token.location.start(source_line->line, column);
}

bool lexer::module_literal(token_t& token) {
    if (match_literal("module")) {
        auto ch = read(false);
        if (!isalnum(ch)) {
            rewind_one_char();
            token = s_module_literal;
            return true;
        }
    }
    return false;
}


}