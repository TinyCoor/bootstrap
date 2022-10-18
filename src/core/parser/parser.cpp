//
// Created by 12132 on 2022/3/30.
//

#include "parser.h"
#include "fmt/format.h"
#include "ast_formatter.h"
#include <ostream>

namespace gfx {
parser::parser(source_file *source)
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
	if (!look_ahead(0)) {
        return false;
    }
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
		source_->error(r, "B016", fmt::format("expected token '{}' but found '{}'.", expected_name, token.name()),
			token.location);
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

ast_node_shared_ptr parser::parse(gfx::result& r)
{
    token_t empty_token {};
	return parse_scope(r, empty_token);
}

bool parser::current(token_t& token)
{
    if (!look_ahead(0)) {
        return false;
    }

    token = tokens_.front();
    return token.type != token_types_t::end_of_file;
}

ast_node_shared_ptr parser::parse_scope(result& r, token_t &token)
{
	auto scope = ast_builder_.begin_scope();
    scope->location.start(token.location.start());
	while (true) {
		if (peek(token_types_t::right_curly_brace)) {
            token_t right_curly_brace;
            current(right_curly_brace);
			consume();
            scope->location.end(right_curly_brace.location.end());
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
                return nullptr;
			}
		}

		if (!scope->pending_attributes.empty()) {
			for (const auto& attr_node : scope->pending_attributes) {
				node->rhs->children.push_back(attr_node);
			}
			scope->pending_attributes.clear();
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
        if (expression->is_comment()) {
            return expression;
        }

		if (expression->is_attribute()) {
			ast_builder_.current_scope()->pending_attributes.push_back(expression);
			token_t line_terminator_token;
			line_terminator_token.type = token_types_t::semi_colon;
			if (!expect(r, line_terminator_token)) {
				break;
			}
			continue;
		}
		if (expression->is_label()) {
			pending_labels.push_back(expression);
			continue;
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
    statement_node->location = expression->location;
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
        source_->error(r, "B021", fmt::format("prefix parser for token '{}' not found.",
			 token.name()), token.location);
		return nullptr;
	}

	auto lhs = prefix_parser->parse(r, this, token);
	if (lhs == nullptr) {
        source_->error(r, "B021", "unexpected empty ast node.", token.location);
		return nullptr;
	}

	if (token.is_line_comment() || token.is_label()) {
		return lhs;
	}

	while (precedence < current_infix_precedence()) {
		if (!consume(token)) {
			break;
		}

		auto infix_parser = infix_parser_for(token.type);
		if (infix_parser == nullptr) {
            source_->error(r, "B021", fmt::format("infix parser for token '{}' not found.",
				 token.name()), token.location);
			break;
		}
		lhs = infix_parser->parse(r, this, lhs, token);
		if (lhs == nullptr || r.is_failed()) {
			break;
		}
	}

	return lhs;
}

ast_node_shared_ptr parser::expect_expression(result& r, ast_node_types_t expected_type, uint8_t precedence)
{
	auto node = parse_expression(r, precedence);
	if (node == nullptr) {
		return nullptr;
	}

	if (node->type != expected_type) {
        source_->error(r, "B031",
			fmt::format("unexpected '{}', wanted '{}'.", node->name(), ast_node_type_name(expected_type)),
			node->token.location);
		return nullptr;
	}

	return node;
}

void parser::write_ast_graph(const std::filesystem::path &path, const ast_node_shared_ptr &program_node)
{
	auto close_required = false;
	FILE* ast_output_file = stdout;
	if (!path.empty()) {
		ast_output_file = fopen(path.string().c_str(),"wt");
		close_required = true;
	}

	ast_formatter formatter(program_node, ast_output_file);
	formatter.format(fmt::format("AST Graph: {}", path.string()));

	if (close_required) {
		fclose(ast_output_file);
	}
}

void parser::error(result &r, const std::string &code, const std::string &message, const source_location &location)
{
    source_->error(r, code, message, location);
}

}