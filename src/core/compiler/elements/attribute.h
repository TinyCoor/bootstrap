//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ATTRIBUTE_H_
#define COMPILER_ELEMENTS_ATTRIBUTE_H_
#include <string>
#include "element.h"

namespace gfx::compiler {
class attribute : public element {
public:
	attribute(block* parent_scope, const std::string& name, element* expr);

	element* expression();

	std::string name() const;

protected:
    bool on_as_bool(bool &value) const override;

    bool on_as_float(double &value) const override;

    bool on_as_integer(uint64_t &value) const override;

    bool on_as_string(std::string& value) const override;

    void on_owned_elements(element_list_t& list) override;
private:
	std::string name_;
	element* expr_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_ATTRIBUTE_H_
