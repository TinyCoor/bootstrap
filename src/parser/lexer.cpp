//
// Created by 12132 on 2022/4/3.
//

#include "lexer.h"

#include <sstream>
namespace gfx {
/// todo use lambda
std::multimap<char, lexer::lexer_case_callable> lexer::s_cases = {
	// attribute
	{'@', std::bind(&lexer::attribute, std::placeholders::_1, std::placeholders::_2)},

	// add
	{'+', std::bind(&lexer::plus, std::placeholders::_1, std::placeholders::_2)},

	// directive
	{'#', std::bind(&lexer::directive, std::placeholders::_1, std::placeholders::_2)},

	// minus, negate
	{'-', std::bind(&lexer::minus, std::placeholders::_1, std::placeholders::_2)},

	// line comment, slash
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
	{'\'', std::bind(&lexer::character_literal, std::placeholders::_1, std::placeholders::_2)},

	// string literal
	{'"', std::bind(&lexer::string_literal, std::placeholders::_1, std::placeholders::_2)},

	// true/false literals
	{'t', std::bind(&lexer::true_literal, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::false_literal, std::placeholders::_1, std::placeholders::_2)},

	// null/none/ns/empty literals
	{'n', std::bind(&lexer::null_literal, std::placeholders::_1, std::placeholders::_2)},
	{'n', std::bind(&lexer::none_literal, std::placeholders::_1, std::placeholders::_2)},
	{'n', std::bind(&lexer::ns_literal, std::placeholders::_1, std::placeholders::_2)},


	// enum literal
	// else if/else literals
	{'e', std::bind(&lexer::enum_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::empty_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::else_if_literal, std::placeholders::_1, std::placeholders::_2)},
	{'e', std::bind(&lexer::else_literal, std::placeholders::_1, std::placeholders::_2)},
	// union literal
	{'u', std::bind(&lexer::union_literal, std::placeholders::_1, std::placeholders::_2)},

	// if
	// in literal
	{'i', std::bind(&lexer::if_literal, std::placeholders::_1, std::placeholders::_2)},
	{'i', std::bind(&lexer::in_literal, std::placeholders::_1, std::placeholders::_2)},


	// fn literal
	// for literal
	{'f', std::bind(&lexer::fn_literal, std::placeholders::_1, std::placeholders::_2)},
	{'f', std::bind(&lexer::for_literal, std::placeholders::_1, std::placeholders::_2)},

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


	// while literal
	{'w', std::bind(&lexer::while_literal, std::placeholders::_1, std::placeholders::_2)},
	{'w', std::bind(&lexer::with_literal, std::placeholders::_1, std::placeholders::_2)},

	// struct literal
	{'s', std::bind(&lexer::struct_literal, std::placeholders::_1, std::placeholders::_2)},


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

lexer::lexer(std::istream& source) : source_(source) {
	source_.seekg(0, std::ios::beg);
}


char lexer::peek() {
	while (!source_.eof()) {
		auto ch = static_cast<char>(source_.get());
		if (!isspace(ch))
			return ch;
	}
	return 0;
}

void lexer::mark_position() {
	mark_ = source_.tellg();
}


void lexer::increment_line() {
	auto pos = source_.tellg();
	if (line_breaks_.count(pos) == 0) {
		line_++;
		column_ = 0;
		line_breaks_.insert(pos);
	}
}

bool lexer::has_next() const {
	return has_next_;
}

void lexer::rewind_one_char() {
	source_.seekg(-1, std::istream::cur);
	if (column_ > 0){
		column_--;
	}
}

void lexer::restore_position() {
	source_.seekg(mark_);
}

bool lexer::next(token_t& token) {
	if (source_.eof()) {
		has_next_ = false;
		token.value = "";
		token.line = line_;
		token.column = column_;
		token.type = token_types_t::end_of_file;
		return true;
	}

	const auto ch = static_cast<char>(tolower(read()));
	rewind_one_char();
	mark_position();

	auto case_range = s_cases.equal_range(ch);
	for (auto it = case_range.first; it != case_range.second; ++it) {
		token.radix = 10;
		token.number_type = number_types_t::none;
		token.line = line_;
		token.column = column_;
		token.number_type = number_types_t::none;
		if (it->second(this, token)) {
			token.line = line_;
			token.column = column_;
			return true;
		}
		restore_position();
	}

	token.type = token_types_t::end_of_file;
	token.value = "";
	token.line = line_;
	token.column = column_;

	has_next_ = false;

	return true;
}

char lexer::read(bool skip_whitespace) {
	while (true) {
		auto ch = static_cast<char>(source_.get());

		column_++;

		if (ch == '\n')
			increment_line();

		if (skip_whitespace && isspace(ch))
			continue;

		return ch;
	}
}

std::string lexer::read_identifier() {
	auto ch = read();
	if (ch != '_' && !isalpha(ch)) {
		return "";
	}
	column_++;
	std::stringstream stream;
	stream << ch;
	while (true) {
		ch = static_cast<char>(source_.get());
		column_++;
		if (ch == '_' || isalnum(ch)) {
			stream << ch;
		} else {
			return stream.str();
		}
	}
}

bool lexer::alias_literal(token_t& token) {
	if (match_literal("alias")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token =s_alias_literal;
			return true;
		}
	}
	return false;
}

bool lexer::break_literal(token_t& token) {
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

bool lexer::while_literal(token_t& token) {
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

bool lexer::union_literal(token_t &token) {
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

bool lexer::continue_literal(token_t& token) {
	if (match_literal("continue")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_constant_literal;
			return true;
		}
	}
	return false;
}

bool lexer::left_curly_brace(token_t& token) {
	auto ch = read();
	if (ch == '{') {
		token = s_left_curly_brace_literal;
		return true;
	}
	return false;
}

bool lexer::right_curly_brace(token_t& token) {
	auto ch = read();
	if (ch == '}') {
		token = s_right_curly_brace_literal;
		return true;
	}
	return false;
}

std::string lexer::read_until(char target_ch) {
	std::stringstream stream;
	while (true) {
		auto ch = static_cast<char>(source_.get());
		column_++;
		if (ch == '\n')
			increment_line();
		if (ch == target_ch)
			break;
		stream << ch;
	}
	return stream.str();
}

bool lexer::plus(token_t& token) {
	auto ch = read();
	if (ch == '+') {
		token =s_plus_literal;
		return true;
	}
	return false;
}

bool lexer::bang(token_t& token) {
	auto ch = read();
	if (ch == '!') {
		token = s_bang_literal;
		return true;
	}
	return false;
}

bool lexer::caret(token_t& token) {
	auto ch = read();
	if (ch == '^') {
		token = s_caret_literal;
		return true;
	}
	return false;
}

bool lexer::tilde(token_t& token) {
	auto ch = read();
	if (ch == '~') {
		token = s_tilde_literal;
		return true;
	}
	return false;
}

bool lexer::colon(token_t& token) {
	auto ch = read();
	if (ch == ':') {
		token = s_colon_literal;
		return true;
	}
	return false;
}

bool lexer::minus(token_t& token) {
	auto ch = read();
	if (ch == '-') {
		token =s_minus_literal;
		return true;
	}
	return false;
}

bool lexer::comma(token_t& token) {
	auto ch = read();
	if (ch == ',') {
		token = s_comma_literal;
		return true;
	}
	return false;
}

bool lexer::slash(token_t& token) {
	auto ch = read();
	if (ch == '/') {
		token = s_slash_literal;
		return true;
	}
	return false;
}

bool lexer::spread(token_t& token) {
	if (match_literal("...")) {
		token = s_spread_operator_literal;
		return true;
	}
	return false;
}

bool lexer::percent(token_t& token) {
	auto ch = read();
	if (ch == '%') {
		token =s_percent_literal;
		return true;
	}
	return false;
}

bool lexer::question(token_t& token) {
	auto ch = read();
	if (ch == '?') {
		token =s_question_literal;
		return true;
	}
	return false;
}

bool lexer::asterisk(token_t& token) {
	auto ch = read();
	if (ch == '*') {
		token = s_asterisk_literal;
		return true;
	}
	return false;
}

bool lexer::attribute(token_t& token) {
	auto ch = read();
	if (ch == '@') {
		token.value = read_identifier();
		if (token.value.empty()) {
			return false;
		}
		token.type = token_types_t::attribute;
		return true;
	}
	return false;
}

bool lexer::identifier(token_t& token) {
	auto name = read_identifier();
	if (name.empty()) {
		return false;
	}

	token.type = token_types_t::identifier;
	token.value = name;
	rewind_one_char();
	return true;
}

bool lexer::assignment(token_t& token) {
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

bool lexer::left_paren(token_t& token) {
	auto ch = read();
	if (ch == '(') {
		token =s_left_paren_literal;
		return true;
	}
	return false;
}

bool lexer::in_literal(token_t& token) {
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

bool lexer::right_paren(token_t& token) {
	auto ch = read();
	if (ch == ')') {
		token = s_right_paren_literal;
		return true;
	}
	return false;
}

bool lexer::fn_literal(token_t& token) {
	auto ch = read();
	if (ch == 'f') {
		ch = read();
		if (ch == 'n') {
			token = s_fn_literal;
			return true;
		}
	}
	return false;
}

bool lexer::ns_literal(token_t& token) {
	auto ch = read();
	if (ch == 'n') {
		ch = read();
		if (ch == 's') {
			token =s_namespace_literal;
			return true;
		}
	}
	return false;
}

bool lexer::if_literal(token_t& token) {
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

bool lexer::else_literal(token_t& token) {
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

bool lexer::line_comment(token_t& token) {
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

bool lexer::for_literal(token_t& token) {
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

bool lexer::null_literal(token_t& token) {
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

bool lexer::none_literal(token_t& token) {
	if (match_literal("none")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_none_literal;
			return true;
		}
	}
	return false;
}

bool lexer::cast_literal(token_t& token) {
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

bool lexer::true_literal(token_t& token) {
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

bool lexer::pipe_literal(token_t& token) {
	auto ch = read();
	if (ch == '|') {
		token = s_pipe_literal;
		return true;
	}
	return false;
}

bool lexer::string_literal(token_t& token) {
	auto ch = read();
	if (ch == '\"') {
		token.type = token_types_t::string_literal;
		token.value = read_until('"');
		return true;
	}
	return false;
}

bool lexer::false_literal(token_t& token) {
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

bool lexer::defer_literal(token_t& token) {
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

bool lexer::empty_literal(token_t& token) {
	if (match_literal("empty")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_empty_literal;
			return true;
		}
	}
	return false;
}

bool lexer::number_literal(token_t& token) {
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

			stream << ch;
		}
	} else if (ch == '@') {
		const std::string valid = "012345678";
		token.radix = 8;
		while (true) {
			ch = read();
			if (ch == '_') {
				continue;
			}

			if (valid.find_first_of(ch) == std::string::npos) {
				break;
			}

			stream << ch;
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

			stream << ch;
		}
	} else {
		const std::string valid = "0123456789_.";
		while (valid.find_first_of(ch) != std::string::npos) {
			if (ch!='_') {
				if (ch=='.') {
					token.number_type = number_types_t::floating_point;
				}
				stream << ch;
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

bool lexer::line_terminator(token_t& token) {
	auto ch = read();
	if (ch == ';') {
		token = s_semi_colon_literal;
		return true;
	}
	return false;
}

bool lexer::equals_operator(token_t& token) {
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

bool lexer::character_literal(token_t& token) {
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


bool lexer::ampersand_literal(token_t& token) {
	auto ch = read();
	if (ch == '&') {
		token = s_ampersand_literal;
		return true;
	}
	return false;
}

bool lexer::less_than_operator(token_t& token) {
	auto ch = read();
	if (ch == '<') {
		token = s_less_than_literal;
		return true;
	}
	return false;
}

bool lexer::logical_or_operator(token_t& token) {
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

bool lexer::logical_and_operator(token_t& token) {
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

bool lexer::not_equals_operator(token_t& token) {
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

bool lexer::left_square_bracket(token_t& token) {
	auto ch = read();
	if (ch == '[') {
		token = s_left_square_bracket_literal;
		return true;
	}
	return false;
}

bool lexer::right_square_bracket(token_t& token) {
	auto ch = read();
	if (ch == ']') {
		token= s_right_square_bracket_literal;
		return true;
	}
	return false;
}

bool lexer::greater_than_operator(token_t& token) {
	auto ch = read();
	if (ch == '>') {
		token = s_greater_than_literal;
		return true;
	}
	return false;
}

bool lexer::less_than_equal_operator(token_t& token) {
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

bool lexer::match_literal(const std::string& literal) {
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

bool lexer::greater_than_equal_operator(token_t& token) {
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

bool lexer::constant_literal(token_t &token) {
	if (match_literal("read_only")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_constant_literal;
			return true;
		}
	}
	return false;
}

bool lexer::else_if_literal(token_t& token) {
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

bool lexer::period(token_t &token) {
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

bool lexer::with_literal(token_t& token) {
	if (match_literal("with")) {
		auto ch = read(false);
		if (!isalnum(ch)) {
			rewind_one_char();
			token = s_true_literal;
			return true;
		}
	}
	return false;
}

}