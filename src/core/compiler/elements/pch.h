//
// Created by 12132 on 2022/11/4.
//

#ifndef COMPILER_ELEMENTS_PCH_H_
#define COMPILER_ELEMENTS_PCH_H_
#include "element.h"
#include "element_types.h"
#include "program.h"
#include "cast.h"
#include "label.h"
#include "module.h"
#include "transmute.h"
#include "types/type.h"
#include "import.h"
#include "raw_block.h"
#include "comment.h"
#include "types/any_type.h"
#include "types/bool_type.h"
#include "types/tuple_type.h"
#include "types/array_type.h"
#include "if_element.h"
#include "expression.h"
#include "attribute.h"
#include "identifier.h"
#include "directive.h"
#include "statement.h"
#include "types/type_info.h"
#include "types/string_type.h"
#include "types/numeric_type.h"
#include "types/procedure_type.h"
#include "types/unknown_type.h"
#include "types/pointer_type.h"
#include "types/module_type.h"
#include "argument_list.h"
#include "boolean_literal.h"
#include "string_literal.h"
#include "float_literal.h"
#include "integer_literal.h"
#include "return_element.h"
#include "unary_operator.h"
#include "symbol_element.h"
#include "types/composite_type.h"
#include "procedure_call.h"
#include "binary_operator.h"
#include "namespace_element.h"
#include "types/namespace_type.h"
#include "procedure_instance.h"
#include "identifier_reference.h"
#include "module_reference.h"
#include "intrinics/size_of_intrinsic.h"
#include "intrinics/alloc_intrinsic.h"
#include "intrinics/free_intrinsic.h"
#include "intrinics/align_of_intrinsic.h"
#include "intrinics/type_of_intrinsic.h"
#include "intrinics/address_of_intrinsic.h"

#endif //COMPILER_ELEMENTS_PCH_H_
