//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#define COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#include "element_types.h"
#include "block.h"
namespace gfx::compiler {
class namespace_element : public element {
public:
	namespace_element(block* parent_scope, const std::string &name, element* expr);

	element* expression();

    std::string name() const;

    void name(const std::string& value);
protected:

    bool on_is_constant() const override;

    bool on_emit(result& r, emit_context_t& context) override;

    compiler::type* on_infer_type(const compiler::program* program) override;

private:
    std::string name_;
	element* expression_ = nullptr;
};

}
#endif // COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
