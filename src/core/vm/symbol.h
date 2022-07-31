//
// Created by 12132 on 2022/7/17.
//

#ifndef VM_SYMBOL_H_
#define VM_SYMBOL_H_
#include "common/id_pool.h"
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>

namespace gfx {
enum class symbol_type_t {
    unknown,
    u8,
    u16,
    u32,
    u64,
    f32,
    f64,
    bytes,
};

static inline std::unordered_map<symbol_type_t, std::string_view> symbol_type_names = {
    {symbol_type_t::u8, "u8"},
    {symbol_type_t::u16, "u16"},
    {symbol_type_t::u32, "u32"},
    {symbol_type_t::u64, "u64"},
    {symbol_type_t::f32, "f32"},
    {symbol_type_t::f64, "f64"},
    {symbol_type_t::bytes, "bytes"},
};

static inline std::string_view symbol_type_name(symbol_type_t type)
{
    auto it = symbol_type_names.find(type);
    if (it == symbol_type_names.end()) {
        return "unknown";
    }
    return it->second;
}

static inline size_t sizeof_symbol_type(symbol_type_t type)
{
    switch (type) {
        case symbol_type_t::u8:return 1u;
        case symbol_type_t::u16:return 2u;
        case symbol_type_t::u32:return 4u;
        case symbol_type_t::u64:return 8u;
        case symbol_type_t::f32:return 4u;
        case symbol_type_t::f64:return 8u;
        case symbol_type_t::bytes:return 8u;
        default:return 0;
    }
}
static inline symbol_type_t float_symbol_type_for_size(size_t size)
{
    switch (size) {
        case 4: return symbol_type_t::f32;
        case 8: return symbol_type_t::f64;
        default:return symbol_type_t::unknown;
    }
}

static inline symbol_type_t integer_symbol_type_for_size(size_t size)
{
    switch (size) {
        case 1:return symbol_type_t::u8;
        case 2:return symbol_type_t::u16;
        case 4:return symbol_type_t::u32;
        case 8:return symbol_type_t::u64;
        default:return symbol_type_t::unknown;
    }
}

class symbol {
public:
    symbol(const std::string& name, symbol_type_t type, uint64_t offset, size_t size =0);

    ~symbol() = default;

    [[nodiscard]] std::string name() const;

    [[nodiscard]] uint64_t offset() const;

    [[nodiscard]] symbol_type_t type() const;

    void value(uint64_t value);

    void value(void* v);

    void value(double value);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] id_t pending_address_from_id() const;

    void pending_address_from_id(id_t value);

private:
    uint64_t offset_;
    std::string name_;
    symbol_type_t type_;
    size_t size_;
    id_t pending_address_from_id_ = 0;
    union {
        double float_value;
        uint64_t int_value;
        void* byte_array_value;
    }value_{};
};

using symbol_list_t = std::vector<symbol>;
}



#endif // VM_SYMBOL_H_
