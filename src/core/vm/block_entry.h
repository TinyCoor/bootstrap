//
// Created by 12132 on 2022/8/5.
//

#ifndef VM_BLOCK_ENTRY_H_
#define VM_BLOCK_ENTRY_H_
#include <any>
#include <vector>
#include <string>
#include "label.h"
#include "instruction.h"

namespace gfx {
enum class section_t : uint8_t {
    unknown,
    bss,
    ro_data,
    data,
    text
};

inline static std::string_view section_name(section_t type)
{
    switch (type) {
        case section_t::bss:    return "bss";
        case section_t::ro_data:return "ro_data";
        case section_t::data:   return "data";
        case section_t::text:   return "text";
        default: return "unknown";
    }
}

inline static section_t section_type(const std::string& name) {
    if (name == "bss")      return section_t::bss;
    if (name == "ro_data")  return section_t::ro_data;
    if (name == "data")     return section_t::data;
    if (name == "text")     return section_t::text;
    return section_t::unknown;
}

struct align_t {
    uint8_t size = 0;
};

enum data_definition_type_t : uint8_t {
    none,
    initialized,
    uninitialized
};

struct data_definition_t {
    op_sizes size = op_sizes::byte;
    std::vector<uint64_t> values;
    data_definition_type_t type = data_definition_type_t::none;
};

struct comment_t {
    uint8_t indent {};
    std::string value {};
};

struct label_t {
    label* instance = nullptr;
};

using comment_list_t = std::vector<comment_t>;

enum class block_entry_type_t : uint8_t {
    section = 1,
    comment,
    label,
    blank_line,
    align,
    instruction,
    data_definition,
};

struct block_entry_t {
    block_entry_t();

    block_entry_t(const block_entry_t& other);

    explicit block_entry_t(const align_t& align);

    explicit block_entry_t(const label_t& label);

    explicit block_entry_t(const comment_t& comment);

    explicit block_entry_t(const section_t& section);

    explicit block_entry_t(const data_definition_t& data);

    explicit block_entry_t(const instruction_t& instruction);

    template<typename T>
    T *data()
    {
        if (!data_.has_value()) {
            return nullptr;
        }
        try {
            return std::any_cast<T>(&data_);
        } catch (const std::bad_any_cast &e) {
            return nullptr;
        }
    }

    template<typename T>
    const T *data() const
    {
        if (!data_.has_value()) {
            return nullptr;
        }
        try {
            return std::any_cast<T>(&data_);
        } catch (const std::bad_any_cast &e) {
            return nullptr;
        }
    }
    [[nodiscard]] uint64_t address() const
    {
        return address_;
    }

    block_entry_t *address(uint64_t value);

    [[nodiscard]] block_entry_type_t type() const;

private:
    std::any data_;
    uint64_t address_ = 0;
    block_entry_type_t type_;
};
}
#endif // VM_BLOCK_ENTRY_H_
