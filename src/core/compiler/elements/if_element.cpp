//
// Created by 12132 on 2022/5/14.
//

#include "if_element.h"
#include "fmt/format.h"
namespace gfx::compiler {
if_element::if_element(element *parent, element *predicate, element *true_branch, element *false_branch)
	: element(parent, element_type_t::if_e), predicate_(predicate), true_branch_(true_branch),
		false_branch_(false_branch)
{

}

element *if_element::predicate()
{
	return predicate_;
}

element *if_element::true_branch()
{
	return true_branch_;
}

element *if_element::false_branch()
{
	return false_branch_;
}

bool if_element::on_emit(result &r, assembler &assembler, const emit_context_t &context)
{
   return predicate_->emit(r, assembler, emit_context_t::for_if_element(context,
            true_branch_->label_name(), false_branch_->label_name()));
}
}
