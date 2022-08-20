//
// Created by 12132 on 2022/5/22.
//
#include "fmt/format.h"
#include "code_dom_formatter.h"
#include "elements/symbol_element.h"
#include "elements/cast.h"
#include "elements/label.h"
#include "elements/alias.h"
#include "elements/import.h"
#include "elements/program.h"
#include "elements/comment.h"
#include "elements/attribute.h"
#include "elements/directive.h"
#include "elements/statement.h"
#include "elements/expression.h"
#include "elements/identifier.h"
#include "elements/if_element.h"
#include "elements/initializer.h"
#include "elements/argument_list.h"
#include "elements/float_literal.h"
#include "elements/return_element.h"
#include "elements/procedure_call.h"
#include "elements/string_literal.h"
#include "elements/unary_operator.h"
#include "elements/integer_literal.h"
#include "elements/boolean_literal.h"
#include "elements/binary_operator.h"
#include "elements/namespace_element.h"
#include "elements/procedure_instance.h"
#include "elements/types/any_type.h"
#include "elements/types/type_info.h"
#include "elements/types/array_type.h"
#include "elements/types/string_type.h"
#include "elements/types/numeric_type.h"
#include "elements/types/procedure_type.h"
#include "elements/types/composite_type.h"
#include "elements/types/namespace_type.h"

namespace gfx::compiler {
code_dom_formatter::code_dom_formatter(const compiler::program* program_element,FILE* file)
	: file_(file), program_(program_element)
{
}

void code_dom_formatter::add_primary_edge(element* parent, element* child, const std::string& label)
{
	if (parent == nullptr || child == nullptr) {
		return;
	}

	std::string attributes;
	if (!label.empty()) {
		attributes = fmt::format("[ label=\"{}\"; ]", label);
	}

	edges_.insert(fmt::format("{} -> {} {};", get_vertex_name(parent), get_vertex_name(child), attributes));
}

void code_dom_formatter::add_secondary_edge(element* parent, element* child, const std::string& label)
{
	if (parent == nullptr || child == nullptr) {
		return;
	}

	std::string attributes = "[ style=dashed; ";
	if (!label.empty()) {
		attributes = fmt::format("label=\"{}\"; ", label);
	}

	attributes += "]";
	edges_.insert(fmt::format("{} -> {} {};", get_vertex_name(parent),
		get_vertex_name(child), attributes));
}

std::string code_dom_formatter::format_node(element* node)
{
	auto node_vertex_name = get_vertex_name(node);
	for (auto attr : node->attributes().as_list()) {
		add_primary_edge(node, attr);
	}

	switch (node->element_type()) {
		case element_type_t::comment: {
			auto comment_element = dynamic_cast<comment*>(node);
			auto style = ", fillcolor=green, style=\"filled\"";
			auto details = fmt::format("comment|{{type: {} | value: '{}' }}",
				comment_type_name(comment_element->type()), escape_graphviz_chars(comment_element->value()));
			return fmt::format("{}[shape=record,label=\"{}\"{}];", node_vertex_name, details, style);
		}
		case element_type_t::cast: {
			auto element = dynamic_cast<cast*>(node);
			auto style = ", fillcolor=deeppink, style=\"filled\"";
			add_primary_edge(element, element->expression());
			return fmt::format("{}[shape=record,label=\"cast|{}\"{}];", node_vertex_name,
				element->type()->symbol()->name(), style);
		}
        case element_type_t::type_info: {
            auto element = dynamic_cast<type_info*>(node);
            auto style = ", fillcolor=gainsboro, style=\"filled\"";
            for (auto fld : element->fields().as_list()) {
                add_primary_edge(element, fld);
            }
            add_primary_edge(element, element->scope());
            return fmt::format("{}[shape=record,label=\"type_info|{}\"{}];",
                node_vertex_name, element->symbol()->name(), style);
        }
		case element_type_t::if_e: {
			auto element = dynamic_cast<if_element*>(node);
			auto style = ", fillcolor=cornsilk1, style=\"filled\"";
			add_primary_edge(element, element->predicate(), "predicate");
			add_primary_edge(element, element->true_branch(), "true");
			add_primary_edge(element, element->false_branch(), "false");
			return fmt::format("{}[shape=record,label=\"if\"{}];", node_vertex_name, style);
		}
		case element_type_t::label: {
			auto element = dynamic_cast<label*>(node);
			auto style = ", fillcolor=lightblue, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"label|{}\"{}];",node_vertex_name, element->name(),
				style);
		}
		case element_type_t::import_e: {
			auto element = dynamic_cast<import*>(node);
			auto style = ", fillcolor=cyan, style=\"filled\"";
			add_primary_edge(element, element->expression());
			return fmt::format("{}[shape=record,label=\"import\"{}];", node_vertex_name, style);
		}
		case element_type_t::block: {
            auto element = dynamic_cast<block*>(node);
			auto style = ", fillcolor=floralwhite, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"block|{}\"{}];", node_vertex_name,
                               element->id(), style);
		}
		case element_type_t::proc_type_block: {
			auto style = ", fillcolor=floralwhite, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"block|proc_type\"{}];",
				node_vertex_name, style);
		}
		case element_type_t::proc_instance_block: {
			auto style = ", fillcolor=floralwhite, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"block|proc_instance\"{}];",
				node_vertex_name, style);
		}
		case element_type_t::field:{
			auto element = dynamic_cast<field*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			add_primary_edge(element, element->identifier());
			return fmt::format("{}[shape=record,label=\"field|{}\"{}];", node_vertex_name,
				element->identifier()->symbol()->name(), style);
		}
		case element_type_t::program: {
			auto program_element = dynamic_cast<program*>(node);
			auto style = ", fillcolor=aliceblue, style=\"filled\"";
			add_primary_edge(program_element, program_element->block());
			return fmt::format("{}[shape=record,label=\"program\"{}];", node_vertex_name, style);
		}
		case element_type_t::any_type:{
			auto element = dynamic_cast<any_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
            for (auto &field : element->fields().as_list()) {
                add_primary_edge(element,field);
            }
            add_primary_edge(element, element->scope());
			return fmt::format("{}[shape=record,label=\"any_type|{}\"{}];",node_vertex_name,
                element->symbol()->name(), style);
		}
		case element_type_t::return_e: {
			auto element = dynamic_cast<return_element*>(node);
			auto style = ", fillcolor=brown1, style=\"filled\"";
			for (const auto& expr : element->expressions()) {
				add_primary_edge(element, expr);
			}
			return fmt::format("{}[shape=record,label=\"return\"{}];", node_vertex_name, style);
		}
		case element_type_t::proc_type: {
			auto element = dynamic_cast<procedure_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			add_primary_edge(element, element->scope());
			return fmt::format("{}[shape=record,label=\"proc_type|{}|foreign: {}\"{}];", node_vertex_name,
                element->symbol()->name(), element->is_foreign(), style);
		}
		case element_type_t::directive: {
			auto directive_element = dynamic_cast<directive*>(node);
			auto style = ", fillcolor=darkolivegreen1, style=\"filled\"";
			add_primary_edge(directive_element, directive_element->expression());
			return fmt::format("{}[shape=record,label=\"directive|{}\"{}];", node_vertex_name,
			   directive_element->name(), style);
		}
		case element_type_t::attribute: {
			auto attribute_element = dynamic_cast<attribute*>(node);
			auto style = ", fillcolor=azure3, style=\"filled\"";
			add_primary_edge(attribute_element, attribute_element->expression());
			return fmt::format("{}[shape=record,label=\"attribute|{}\"{}];",
				node_vertex_name, attribute_element->name(), style);
		}
		case element_type_t::statement: {
			auto statement_element = dynamic_cast<statement*>(node);
			auto style = ", fillcolor=azure, style=\"filled\"";
			add_primary_edge(statement_element, statement_element->expression());
			for (auto &lbl : statement_element->labels()) {
				add_primary_edge(statement_element, lbl);
			}
			return fmt::format("{}[shape=record,label=\"statement\"{}];", node_vertex_name, style);
		}
		case element_type_t::argument_list: {
			auto args = dynamic_cast<argument_list*>(node);
			auto style = ", fillcolor=azure, style=\"filled\"";
			for (auto& arg_element : args->elements()) {
				add_primary_edge(args, arg_element);
			}
			return fmt::format("{}[shape=record,label=\"argument_list\"{}];", node_vertex_name, style);
		}
		case element_type_t::proc_call: {
			auto element = dynamic_cast<procedure_call*>(node);
			auto style = ", fillcolor=darkorchid1, style=\"filled\"";
			add_primary_edge(element, element->arguments());
			add_primary_edge(element, element->identifier()->type());
			return fmt::format("{}[shape=record,label=\"proc_call|{}\"{}];", node_vertex_name,
				element->identifier()->symbol()->name(), style);
		}
		case element_type_t::alias_type: {
			auto element = dynamic_cast<alias*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			add_primary_edge(element, element->expression());
			return fmt::format("{}[shape=record,label=\"alias_type\"{}];", node_vertex_name, style);
		}
		case element_type_t::array_type: {
			auto element = dynamic_cast<array_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			std::string entry_type_name = "unknown";
            add_primary_edge(element, element->entry_type());
			if (element->entry_type() != nullptr) {
				entry_type_name = element->entry_type()->symbol()->name();
			}
            add_primary_edge(element, element->scope());
            for (auto &field : element->fields().as_list()) {
                add_primary_edge(element,field);
            }
			return fmt::format("{}[shape=record,label=\"array_type|size: {}|type: {}\"{}];",
				node_vertex_name, element->size(), entry_type_name, style);
		}
		case element_type_t::identifier: {
			auto identifier_element = dynamic_cast<identifier*>(node);
			auto style = ", fillcolor=deepskyblue1, style=\"filled\"";
			std::string type_name = "unknown";
			if (identifier_element->type() != nullptr) {
				type_name = identifier_element->type()->symbol()->name();
			}
			auto details = fmt::format( "identifier|{}|{{type: {} | inferred: {} | constant: {} }}",
				identifier_element->symbol()->name(), type_name, identifier_element->inferred_type(),
				identifier_element->constant());
			add_primary_edge(identifier_element, identifier_element->type());
			add_primary_edge(identifier_element, identifier_element->initializer());
			return fmt::format("{}[shape=record,label=\"{}\"{}];", node_vertex_name, details, style);
		}
		case element_type_t::expression: {
			auto element = dynamic_cast<expression*>(node);
			auto style = ", fillcolor=gold3, style=\"filled\"";
			add_primary_edge(element, element->root());
			return fmt::format("{}[shape=record,label=\"expression_group\"{}];", node_vertex_name, style);
		}
		case element_type_t::string_type: {
			auto element = dynamic_cast<string_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
            for (auto &field : element->fields().as_list()) {
                add_primary_edge(element,field);
            }
            add_primary_edge(element, element->scope());
			return fmt::format("{}[shape=record,label=\"string_type|{}\"{}];", node_vertex_name,
                element->symbol()->name(), style);
		}
		case element_type_t::namespace_e: {
			auto ns_element = dynamic_cast<namespace_element*>(node);
			auto style = ", fillcolor=yellow, style=\"filled\"";
			add_primary_edge(ns_element, ns_element->expression());
			return fmt::format("{}[shape=record,label=\"namespace\"{}];", node_vertex_name, style);
		}
		case element_type_t::initializer: {
			auto initializer_element = dynamic_cast<initializer*>(node);
			auto style = ", fillcolor=darkolivegreen1, style=\"filled\"";
			add_primary_edge(initializer_element, initializer_element->expression());
			return fmt::format("{}[shape=record,label=\"initializer\"{}];", node_vertex_name, style);
		}
		case element_type_t::bool_type:
		case element_type_t::numeric_type: {
			auto element = dynamic_cast<numeric_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"numeric_type|{}\"{}];", node_vertex_name,
                               element->symbol()->name(), style);
		}
		case element_type_t::proc_instance: {
			auto element = dynamic_cast<procedure_instance*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			add_primary_edge(element, element->procedure_type());
			add_primary_edge(element, element->scope());
			return fmt::format("{}[shape=record,label=\"proc_instance\"{}];", node_vertex_name, style);
		}
		case element_type_t::float_literal: {
			auto element = dynamic_cast<float_literal*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"float_literal|{}\"{}];", node_vertex_name,
               element->value(), style);
		}
		case element_type_t::string_literal: {
			auto element = dynamic_cast<string_literal*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"string_literal|{}\"{}];", node_vertex_name,
                element->value(), style);
		}
		case element_type_t::composite_type: {
			auto element = dynamic_cast<composite_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
            add_primary_edge(element, element->scope());
			return fmt::format("{}[shape=record,label=\"composite_type|{}\"{}];", node_vertex_name,
                element->symbol()->name(), style);
		}
		case element_type_t::unary_operator: {
			auto element = dynamic_cast<unary_operator*>(node);
			auto style = ", fillcolor=slateblue1, style=\"filled\"";
			add_primary_edge(element, element->rhs());
			return fmt::format("{}[shape=record,label=\"unary_operator|{}\"{}];", node_vertex_name,
				operator_type_name(element->operator_type()), style);
		}
		case element_type_t::boolean_literal: {
			auto element = dynamic_cast<boolean_literal*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"boolean_literal|{}\"{}];", node_vertex_name,
                element->value(), style);
		}
		case element_type_t::integer_literal: {
			auto element = dynamic_cast<integer_literal*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"integer_literal|{}\"{}];", node_vertex_name,
                element->value(), style);
		}
		case element_type_t::binary_operator: {
			auto element = dynamic_cast<binary_operator*>(node);
			auto style = ", fillcolor=slateblue1, style=\"filled\"";
			add_primary_edge(element, element->lhs(), "lhs");
			add_primary_edge(element, element->rhs(), "rhs");
			return fmt::format("{}[shape=record,label=\"binary_operator|{}\"{}];", node_vertex_name,
				operator_type_name(element->operator_type()), style);
		}
		case element_type_t::namespace_type: {
			auto element = dynamic_cast<namespace_type*>(node);
			auto style = ", fillcolor=gainsboro, style=\"filled\"";
			return fmt::format("{}[shape=record,label=\"namespace_type|{}\"{}];", node_vertex_name,
                element->symbol()->name(), style);
		}
		default:
			break;
	}

	return "";
}

void code_dom_formatter::format(const std::string& title)
{
	fmt::print(file_, "digraph {{\n");
	fmt::print(file_, "rankdir=LR\n");
	fmt::print(file_, "graph [ fontsize=22 ];\n");
	fmt::print(file_, "labelloc=\"t\";\n");
	fmt::print(file_, "label=\"{}\";\n", title);

	nodes_.clear();
	edges_.clear();

    auto non_const_program = const_cast<compiler::program*>(program_);
    nodes_.insert(format_node(non_const_program));

	for (const auto& pair : non_const_program->elements()) {
		auto node_def = format_node(pair.second);
		if (node_def.empty()) {
			continue;
		}
		nodes_.insert(node_def);
		add_secondary_edge(pair.second->parent_scope(), pair.second);
	}

	for (const auto& node : nodes_) {
		fmt::print(file_, "\t{}\n", node);
	}

	for (const auto& edge : edges_) {
		fmt::print(file_, "\t{}\n", edge);
	}
	fmt::print(file_, "}}\n");
}

std::string code_dom_formatter::get_vertex_name(element* node)
{
	return fmt::format("{}_{}", element_type_name(node->element_type()), node->id());
}

std::string code_dom_formatter::escape_graphviz_chars(const std::string& value)
{
    std::string buffer;
    for (const auto &c : value) {
        if (c == '\"') {
            buffer += "\\\"";
        } else {
          buffer += c;
        }
    }
    return buffer;
}
}