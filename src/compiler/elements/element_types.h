//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#define COMPILER_ELEMENTS_ELEMENT_TYPES_H_
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace gfx::compiler {

class type;
class field;
class block;
class element;
class program;
class directive;
class array_type;
class string_type;
class procedure_type;
class attribute;
class identifier;
class expression;
class initializer;
class float_literal;
class operator_base;
class unary_operator;
class integer_literal;
class binary_operator;
class namespace_element;

using element_list_t = std::vector<element*>;

using attribute_map_t = std::map<std::string, attribute*>;
using directive_map_t = std::map<std::string, directive*>;

}
#endif // COMPILER_ELEMENTS_ELEMENT_TYPES_H_
