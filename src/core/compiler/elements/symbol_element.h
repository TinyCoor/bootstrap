//
// Created by 12132 on 2022/8/6.
//

#ifndef COMPILER_ELEMENTS_SYMBOL_ELEMENT_H_
#define COMPILER_ELEMENTS_SYMBOL_ELEMENT_H_
#include "element.h"
namespace gfx::compiler {
class symbol_element : public element {
public:
    symbol_element(compiler::block *parent_scope, const std::string &name, const string_list_t &namespaces);

    std::string name() const;

    bool is_qualified() const;

    void constant(bool value);

    void cache_fully_qualified_name();

    std::string fully_qualified_name();

    const string_list_t &namespaces() const;

    qualified_symbol_t qualified_symbol() const;

    bool operator== (const symbol_element& other) const;

    bool operator== (const qualified_symbol_t& other) const;

protected:
    bool on_is_constant() const override;

private:
    std::string name_;
    bool is_constant_ = false;
    string_list_t namespaces_{};
    std::string fully_qualified_name_ {};
};
}
#endif //COMPILER_ELEMENTS_SYMBOL_ELEMENT_H_
