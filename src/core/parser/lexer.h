//
// Created by 12132 on 2022/4/3.
//

#ifndef LEXER_H_
#define LEXER_H_
#include "token.h"
#include "common/source_file.h"
#include <functional>
#include <fstream>
#include <set>
#include <map>

namespace gfx {

class lexer {
public:
	using lexer_case_callable = std::function<bool (lexer*, token_t&)>;

	explicit lexer(source_file* source);

	[[nodiscard]] bool has_next() const;

	bool next(token_t& token);

    [[nodiscard]] const result& result() const;

private:
	bool plus(token_t& token);

	bool bang(token_t& token);

	bool period(token_t& token);

	bool minus(token_t& token);

	bool comma(token_t& token);

	bool label(token_t& token);

	bool slash(token_t& token);

	bool caret(token_t& token);

	bool tilde(token_t& token);

	bool colon(token_t& token);

	bool spread(token_t& token);

	bool percent(token_t& token);

	bool asterisk(token_t& token);

	bool question(token_t& token);

	bool attribute(token_t& token);

	bool proc_literal(token_t& token);

	bool in_literal(token_t& token);

	bool if_literal(token_t& token);

	bool left_paren(token_t& token);

	bool xor_literal(token_t& token);

	bool shl_literal(token_t& token);

	bool shr_literal(token_t& token);

	bool rol_literal(token_t& token);

	bool ror_literal(token_t& token);

	bool identifier(token_t& token);

	bool assignment(token_t& token);

	bool ns_literal(token_t& token);

	bool for_literal(token_t& token);

	bool union_literal(token_t& token);

	bool right_paren(token_t& token);

	bool else_literal(token_t& token);

	bool line_comment(token_t& token);

	bool pipe_literal(token_t& token);

	bool null_literal(token_t& token);

	bool directive(token_t& token);

	bool enum_literal(token_t& token);

	bool struct_literal(token_t& token);

	bool return_literal(token_t& token);

	bool cast_literal(token_t& token);

	bool true_literal(token_t& token);

	bool false_literal(token_t& token);

	bool defer_literal(token_t& token);

	bool break_literal(token_t& token);

	bool while_literal(token_t& token);

	bool alias_literal(token_t& token);

	bool number_literal(token_t& token);

    bool module_literal(token_t& token);

	bool scope_operator(token_t& token);

	bool string_literal(token_t& token);

	bool import_literal(token_t& token);

	bool line_terminator(token_t& token);

	bool equals_operator(token_t& token);

	bool else_if_literal(token_t& token);

	bool left_curly_brace(token_t& token);

	bool continue_literal(token_t& token);

	bool constant_literal(token_t& token);

	bool with_literal(token_t& token);

    bool from_literal(token_t& token);

	bool right_curly_brace(token_t& token);

	bool ampersand_literal(token_t& token);

	bool character_literal(token_t& token);

	bool less_than_operator(token_t& token);

	bool not_equals_operator(token_t& token);

    bool transmute_literal(token_t& token);

	bool block_comment(token_t& token);

	bool left_square_bracket(token_t& token);

	bool logical_or_operator(token_t& token);

	bool logical_and_operator(token_t& token);

	bool right_square_bracket(token_t& token);

	bool greater_than_operator(token_t& token);

    bool constant_assignment(token_t& token);

	bool less_than_equal_operator(token_t& token);

	bool greater_than_equal_operator(token_t& token);

private:
    void set_token_location(token_t &token);

    [[maybe_unused]] rune_t peek();

	void rewind_one_char();

	std::string read_identifier();

	std::string read_until(char target_ch);

	rune_t read(bool skip_whitespace = true);

	bool match_literal(const std::string& literal);
private:
	bool has_next_ = true;
    gfx::result result_ {};
	source_file* source_;
    static std::multimap<rune_t, lexer_case_callable> s_cases;
};

}

#endif // LEXER_H_
