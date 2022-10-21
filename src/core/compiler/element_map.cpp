//
// Created by 12132 on 2022/8/6.
//

#include "element_map.h"
#include "elements/element.h"
namespace gfx::compiler {

element_map::element_map()
{
}

element_map::~element_map()
{
    clear();
}

void element_map::clear()
{
    elements_by_type_.clear();
    for (const auto& it : elements_by_id_) {
        delete it.second;
    }
    elements_by_id_.clear();
}

void element_map::remove(id_t id)
{
    auto element = find(id);
    if (element == nullptr) {
        return;
    }
    element_list_t owned_elements {};
    element->owned_elements(owned_elements);

    for (auto owned : owned_elements) {
        remove(owned->id());
    }

    remove_index_by_type(element);
    elements_by_id_.erase(id);
    delete element;
}

element* element_map::find(id_t id)
{
    auto it = elements_by_id_.find(id);
    if (it == elements_by_id_.end()) {
        return nullptr;
    }
    return it->second;
}

element_by_id_map_t::iterator element_map::end()
{
    return elements_by_id_.end();
}

void element_map::add(compiler::element* element)
{
    elements_by_id_.insert(std::make_pair(element->id(), element));
    add_index_by_type(element);
}

element_by_id_map_t::iterator element_map::begin()
{
    return elements_by_id_.begin();
}

void element_map::add_index_by_type(compiler::element* element)
{
    auto it = elements_by_type_.find(element->element_type());
    if (it == elements_by_type_.end()) {
        element_list_t list {};
        list.emplace_back(element);
        elements_by_type_.insert(std::make_pair(element->element_type(), list));
    } else {
        element_list_t& list = it->second;
        list.emplace_back(element);
    }
}

element_by_id_map_t::const_iterator element_map::end() const
{
    return elements_by_id_.cend();
}

element_list_t element_map::find_by_type(element_type_t type)
{
    element_list_t list {};
    auto it = elements_by_type_.find(type);
    if (it != elements_by_type_.end()) {
        element_list_t& index_list = it->second;
        for (auto e : index_list)
            list.emplace_back(e);
    }
    return list;
}

element_by_id_map_t::const_iterator element_map::begin() const
{
    return elements_by_id_.cbegin();
}

void element_map::remove_index_by_type(compiler::element* element)
{
    auto it = elements_by_type_.find(element->element_type());
    if (it == elements_by_type_.end()) {
        return;
    }

    element_list_t& list = it->second;
    auto element_it = std::find_if(list.begin(),list.end(), [&element](auto item) { return item == element; });
    if (element_it == list.end()) {
        return;
    }
    list.erase(element_it);
}
}