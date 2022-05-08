//
// Created by 12132 on 2022/3/30.
//

#include "parser.h"
#include "fmt/format.h"
#include <ostream>
#include <sstream>
namespace gfx {
parser::parser(std::istream &source)
	: source_(source), lexer_(source)
{

}
parser::~parser() = default;

bool parser::consume()
{
	token_t token;
	return consume(token);
}

bool parser::consume(token_t &token)
{
	if (!look_ahead(0)) {
		return false;
	}

	token = tokens_.front();
	tokens_.erase(tokens_.begin());

	return token.type != token_types_t::end_of_file;
}

bool parser::peek(token_types_t type)
{
	if (!look_ahead(0))
		return false;
	auto& token = tokens_.front();
	return token.type == type;
}

bool parser::look_ahead(size_t count)
{
	while (count >= tokens_.size() && lexer_.has_next()) {
		token_t token;
		if (lexer_.next(token)) {
			tokens_.push_back(token);
		}
	}
	return !tokens_.empty();
}

bool parser::expect(class result &r, token_t &token)
{
	if (!look_ahead(0)) {
		return false;
	}

	std::string expected_name = token.name();
	auto expected_type = token.type;
	token = tokens_.front();
	if (token.type != expected_type) {
		error(r, "B016", fmt::format("expected token '{}' but found '{}'.", expected_name, token.name()),
			token.line, token.column);
		return false;
	}
	tokens_.erase(tokens_.begin());
	return true;
}

uint8_t parser::current_infix_precedence()
{
	if (!look_ahead(0)) {
		return 0;
	}

	auto& token = tokens_.front();
	auto infix_parser = infix_parser_for(token.type);
	if (infix_parser != nullptr) {
		return static_cast<uint8_t>(infix_parser->precedence());
	}
	return 0;
}

ast_builder* parser::ast_builder()
{
	return &ast_builder_;
}

ast_node_shared_ptr parser::parse(result& r)
{
	return parse_scope(r);
}

ast_node_shared_ptr parser::parse_scope(result& r)
{
	auto scope = ast_builder_.begin_scope();

	while (true) {
		if (peek(token_types_t::right_curly_brace)) {
			consume();
			break;
		}
		auto node = parse_statement(r);
		if (node == nullptr) {
			break;
		}
		scope->children.push_back(node);

		if (node->type == ast_node_types_t::statement) {
			token_t line_terminator_token;
			line_terminator_token.type = token_types_t::semi_colon;
			if (!expect(r, line_terminator_token)) {
				break;
			}
		}
	}

	if (peek(token_types_t::attribute)) {
		auto attribute_node = parse_expression(r, 0);
		scope->children.push_back(attribute_node);
	}

	return ast_builder_.end_scope();
}

ast_node_shared_ptr parser::parse_statement(result& r)
{
	ast_node_list pending_labels {};

	ast_node_shared_ptr expression;
	while (true) {
		expression = parse_expression(r, 0);
		if (expression == nullptr) {
			return nullptr;
		}
		if (expression->type == ast_node_types_t::label) {
			pending_labels.push_back(expression);
			continue;
		}
		if (expression->type == ast_node_types_t::line_comment
			||  expression->type == ast_node_types_t::block_comment
			||  expression->type == ast_node_types_t::basic_block) {
			return expression;
		}
		break;
	}

	auto statement_node = ast_builder_.statement_node();

	if (!pending_labels.empty()) {
		statement_node->lhs = ast_builder_.label_list_node();
		for (const auto& label_node : pending_labels) {
			statement_node->lhs->children.push_back(label_node);
		}
	}

	statement_node->rhs = expression;
	return statement_node;
}

infix_parser* parser::infix_parser_for(token_types_t type)
{
	auto it = s_infix_parsers.find(type);
	if (it == s_infix_parsers.end()) {
		return nullptr;
	}
	return it->second;
}

prefix_parser* parser::prefix_parser_for(token_types_t type)
{
	auto it = s_prefix_parsers.find(type);
	if (it == s_prefix_parsers.end()) {
		return nullptr;
	}
	return it->second;
}

ast_node_shared_ptr parser::parse_expression(result& r, uint8_t precedence)
{
	token_t token;
	if (!consume(token)) {
		return nullptr;
	}

	auto prefix_parser = prefix_parser_for(token.type);
	if (prefix_parser == nullptr) {
		error(r, "B021", fmt::format("prefix parser for token '{}' not found.", token.name()), token.line, token.column);
		return nullptr;
	}

	auto lhs = prefix_parser->parse(r, this, token);
	if (lhs == nullptr) {
		error(r, "B021", "unexpected empty ast node.", token.line, token.column);
		return nullptr;
	}

	if (token.is_line_comment() || token.is_block_comment() || token.is_label()) {
		return lhs;
	}

	while (precedence < current_infix_precedence()) {
		if (!consume(token)) {
			break;
		}

		auto infix_parser = infix_parser_for(token.type);
		if (infix_parser == nullptr) {
			error(r, "B021", fmt::format("infix parser for token '{}' not found.", token.name()), token.line, token.column);
			break;
		}
		lhs = infix_parser->parse(r, this, lhs, token);
		if (lhs == nullptr || r.is_failed()) {
			break;
		}
	}

	return lhs;
}

void parser::error(result &r, const std::string &code, const std::string &message, uint32_t line, uint32_t column)
{
	std::vector<std::string> source_lines {};
	source_.seekg(0, std::ios::beg);
	while (!source_.eof()) {
		std::string source_line;
		std::getline(source_, source_line);
		source_lines.push_back(source_line);
	}

	std::stringstream stream;
	stream << "\n";
	auto start_line = std::max<int32_t>(0, static_cast<int32_t>(line) - 4);
	auto stop_line = std::min<int32_t>(static_cast<int32_t>(source_lines.size()),
		static_cast<int32_t>(line + 4));
	auto message_indicator = "^ " + message;
	for (int32_t i = start_line; i < stop_line; i++) {
		if (i == static_cast<int32_t>(line + 1)) {
			stream << fmt::format("{:04d}: ", i + 1)
				   << source_lines[i] << "\n"
				   << fmt::format("{}{}", std::string(column + 6, ' '), message_indicator);
		} else {
			stream << fmt::format("{:04d}: ", i + 1)
				   << source_lines[i];
		}

		if (i < static_cast<int32_t>(stop_line - 1))
			stream << "\n";
	}

	r.add_message(code, fmt::format("{} @ {}:{}", message, line, column), stream.str(), true);
}

}