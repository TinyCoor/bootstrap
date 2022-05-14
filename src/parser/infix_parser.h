//
// Created by 12132 on 2022/5/2.
//

#ifndef PARSER_INFIX_PARSER_H_
#define PARSER_INFIX_PARSER_H_
#include "ast.h"
#include "src/common/result.h"
namespace gfx {
class parser;

enum class precedence_t : uint8_t {
	assignment = 1,
	comma,
	conditional,
	sum,
	product,
	logical,
	relational,
	bitwise,
	exponent,
	block_comment,
	prefix,
	postfix,
	cast,
	type,
	variable,
	call
};

/// 中缀
class infix_parser {
public:
	virtual ~infix_parser() = default;

	virtual ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) = 0;

	virtual precedence_t precedence() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class type_identifier_infix_parser : public infix_parser {
public:
	type_identifier_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class symbol_infix_parser : public infix_parser {
public:
	symbol_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class proc_call_infix_parser : public infix_parser {
public:
	proc_call_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
							  token_t& token) override;

	precedence_t precedence() const override;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class binary_operator_infix_parser : public infix_parser {
public:
	binary_operator_infix_parser(precedence_t precedence, bool is_right_associative) noexcept;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;

private:
	precedence_t precedence_;
	bool is_right_associative_ = false;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class assignment_infix_parser : public infix_parser {
public:
	assignment_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

class block_comment_infix_parser : public infix_parser {
public:
	block_comment_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
		token_t& token) override;

	precedence_t precedence() const override;
};

class cast_infix_parser : public infix_parser {
public:
	cast_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token) override;

	precedence_t precedence() const override;
};

class comma_infix_parser : public infix_parser {
public:
	comma_infix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
		token_t& token) override;

	precedence_t precedence() const override;
};


ast_node_shared_ptr create_type_identifier_node(result& r, parser* parser, token_t& token);

ast_node_shared_ptr create_cast_node(result& r, parser* parser, token_t& token);

ast_node_shared_ptr create_symbol_node(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token);

void pairs_to_list(const ast_node_shared_ptr& target, const ast_node_shared_ptr& root);

 ast_node_shared_ptr create_expression_node(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token) ;

}

#endif // PARSER_INFIX_PARSER_H_
