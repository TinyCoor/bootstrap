//
// Created by 12132 on 2022/5/2.
//

#ifndef BOOTSTRAP_SRC_PARSER_PREFIX_PARSER_H_
#define BOOTSTRAP_SRC_PARSER_PREFIX_PARSER_H_
#include "ast.h"
#include "src/common/result.h"
#include "infix_parser.h"
namespace gfx {
class parser;

/////////////////////////////////////////////////////////////////////////////////////////////
/// 前缀
class prefix_parser {
public:
	virtual ~prefix_parser() = default;

	virtual ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class constant_prefix_parser : public prefix_parser {
public:
	constant_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

class with_prefix_parser : public prefix_parser {
public:
	with_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class defer_prefix_parser : public prefix_parser {
public:
	defer_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class namespace_prefix_parser : public prefix_parser {
public:
	namespace_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class union_prefix_parser : public prefix_parser {
public:
	union_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};


/////////////////////////////////////////////////////////////////////////////////////////////

class struct_prefix_parser : public prefix_parser {
public:
	struct_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class enum_prefix_parser : public prefix_parser {
public:
	enum_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class for_in_prefix_parser : public prefix_parser {
public:
	for_in_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class return_prefix_parser : public prefix_parser {
public:
	return_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class if_prefix_parser : public prefix_parser {
public:
	if_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class type_identifier_prefix_parser : public prefix_parser {
public:
	type_identifier_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

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

class proc_expression_prefix_parser : public prefix_parser {
public:
	proc_expression_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class unary_operator_prefix_parser : public prefix_parser {
public:
	explicit unary_operator_prefix_parser(precedence_t precedence) noexcept;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;

private:
	precedence_t _precedence;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class keyword_literal_prefix_parser : public prefix_parser {
public:
	keyword_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class number_literal_prefix_parser : public prefix_parser {
public:
	number_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r,parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class string_literal_prefix_parser : public prefix_parser {
public:
	string_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class char_literal_prefix_parser : public prefix_parser {
public:
	char_literal_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class line_comment_prefix_parser : public prefix_parser {
public:
	line_comment_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////
class block_comment_prefix_parser : public prefix_parser {
public:
	block_comment_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r,parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////
class symbol_reference_prefix_parser : public prefix_parser {
public:
	symbol_reference_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class directive_prefix_parser : public prefix_parser {
public:
	directive_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class attribute_prefix_parser : public prefix_parser  {
public:
	attribute_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class array_subscript_prefix_parser : public prefix_parser {
public:
	array_subscript_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////
class label_prefix_parser : public prefix_parser {
public:
	label_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

///////////////////////////////////////////////////////////////////////////

class cast_prefix_parser : public prefix_parser {
public:
	cast_prefix_parser() = default;

	ast_node_shared_ptr parse(result& r, parser* parser, token_t& token) override;
};

}

#endif //BOOTSTRAP_SRC_PARSER_PREFIX_PARSER_H_
