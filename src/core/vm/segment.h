//
// Created by 12132 on 2022/7/17.
//

#ifndef BOOTSTRAP_SRC_CORE_VM_SEGMENT_H_
#define BOOTSTRAP_SRC_CORE_VM_SEGMENT_H_
#include "symbol.h"
namespace gfx {
enum class segment_type_t {
    code,
    data,
    stack,
    constant,
};

static inline std::unordered_map<segment_type_t, std::string_view> segment_type_names = {
    {segment_type_t::code,     "code"},
    {segment_type_t::data,     "data"},
    {segment_type_t::stack,    "stack"},
    {segment_type_t::constant, "constant"},
};

static inline std::string_view segment_type_name(segment_type_t type)
{
    auto it = segment_type_names.find(type);
    if (it == segment_type_names.end()) {
        return "unknown";
    }
    return it->second;
}

class segment {
public:
    segment(const std::string &name, segment_type_t type);

    size_t size() const;

    std::string name() const;

    gfx::symbol* symbol(const std::string& name);

    segment_type_t type() const;

    bool initialize() const;

    void initialize(bool value);

    symbol_list_t symbols();

    gfx::symbol* symbol(const std::string& name, symbol_type_t type, size_t size = 0);
private:
    uint64_t address_ =0;
    uint64_t offset_ = 0;
    std::string name_;
    segment_type_t type_;
    bool initialized = false;
    std::unordered_map<std::string, gfx::symbol> symbols_{};
};

using segment_list_t = std::vector<segment>;
}

#endif //BOOTSTRAP_SRC_CORE_VM_SEGMENT_H_
