//
// Created by 12132 on 2022/10/22.
//

#include "intrinsic.h"
#include "argument_list.h"
namespace gfx::compiler {

intrinsic::intrinsic(compiler::module *module, compiler::block *parent_scope,
    compiler::identifier_reference *reference, compiler::argument_list *args)
     : element(module, parent_scope, element_type_t::intrinsic),
         arguments_(args) ,reference_(reference)
{

}
compiler::argument_list *intrinsic::arguments()
{
    return arguments_;
}

compiler::identifier_reference *intrinsic::reference()
{
    return reference_;
}

void intrinsic::reference(compiler::identifier_reference *value)
{
    reference_ = value;
}

void intrinsic::on_owned_elements(element_list_t &list)
{
    if (arguments_) {
        list.push_back(arguments_);
    }
}

bool intrinsic::on_infer_type(const session &session, type_inference_result_t& result)
{
    return false;
}
}