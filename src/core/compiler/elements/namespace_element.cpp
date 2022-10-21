//
// Created by 12132 on 2022/4/23.
//

#include "namespace_element.h"
#include "initializer.h"
#include "symbol_element.h"
#include "program.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
namespace_element::namespace_element(compiler::module* module, block* parent_scope, element* expr)
	: element(module, parent_scope, element_type_t::namespace_e), expression_(expr)
{
}

element *namespace_element::expression()
{
	return expression_;
}

compiler::type *namespace_element::on_infer_type(const compiler::session& session)
{
	return session.program().find_type(qualified_symbol_t{.name = "namespace"});
}

bool compiler::namespace_element::on_is_constant() const
{
    return true;
}

bool namespace_element::on_emit(compiler::session &session)
{
    if (expression_ == nullptr) {
        return true;
    }

    return expression_->emit(session);
}

std::string namespace_element::name()
{
    std::string name("unknown");
    switch (parent_element()->element_type()) {
        case element_type_t::initializer: {
            auto parent_init = dynamic_cast<compiler::initializer*>(parent_element());
            auto parent_identifier = dynamic_cast<compiler::identifier*>(parent_init->parent_element());
            name = parent_identifier->symbol()->fully_qualified_name();
            break;
        }
        case element_type_t::identifier: {
            auto parent_identifier = dynamic_cast<compiler::identifier*>(parent_element());
            name = parent_identifier->symbol()->fully_qualified_name();
            break;
        }
        default:
            break;
    }
    return name;
}

void namespace_element::on_owned_elements(element_list_t &list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}

}