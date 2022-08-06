//
// Created by 12132 on 2022/8/6.
//

#ifndef COMPILER_ELEMENT_MAP_H_
#define COMPILER_ELEMENT_MAP_H_

#include "elements/element_types.h"

namespace gfx::compiler {
using element_by_id_map_t = std::unordered_map<id_t, element*>;
using element_by_type_map_t = std::unordered_map<element_type_t, element_list_t>;

class element_map {
public:
    element_map();

    virtual ~element_map();

    void clear();

    void remove(id_t id);

    element* find(id_t id);

    element_by_id_map_t::iterator end();

    void add(compiler::element* element);

    element_by_id_map_t::iterator begin();

    element_list_t find_by_type(element_type_t type);

    element_by_id_map_t::const_iterator cend() const;

    element_by_id_map_t::const_iterator cbegin() const;

private:
    void add_index_by_type(compiler::element* element);

    void remove_index_by_type(compiler::element* element);

private:
    element_by_id_map_t elements_by_id_ {};
    element_by_type_map_t elements_by_type_ {};
};

}
#endif // COMPILER_ELEMENT_MAP_H_
