//
// Created by 12132 on 2022/5/22.
//

#ifndef COMPILER_CODE_DOM_FORMATTER_H_
#define COMPILER_CODE_DOM_FORMATTER_H_
#include <string>
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
class code_dom_formatter {
public:
	code_dom_formatter(const compiler::program* program_element, FILE* file);

	void format(const std::string& title);

private:
	std::string format_node(element* node);

    static std::string get_vertex_name(element* node);

    static std::string escape_graphviz_chars(const std::string& value);

    void add_primary_edge(element* parent, element* child, const std::string& label = "");

    void add_secondary_edge(element* parent, element* child, const std::string& label = "");
private:
	FILE* file_ = nullptr;
	std::set<std::string> edges_ {};
	std::set<std::string> nodes_ {};
	const compiler::program* program_ = nullptr;
};
}

#endif // COMPILER_CODE_DOM_FORMATTER_H_
