//
// Created by 12132 on 2022/4/23.
//

#include "field.h"
#include "identifier.h"
namespace gfx::compiler {

field::field(compiler::module* module, block *parent, compiler::identifier *identifier)
	: element(module, parent, element_type_t::field), identifier_(identifier)
{

}

compiler::identifier *field::identifier()
{
	return identifier_;
}
void field::on_owned_elements(element_list_t &list)
{
    if (identifier_ != nullptr) {
        list.emplace_back(identifier_);
    }
}

}