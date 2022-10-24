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

using comment_list_t = std::vector<comment_t>;

enum class block_entry_type_t : uint8_t {
    section = 1,
    memo,
    align,
    instruction,
    data_definition,
};

struct block_entry_t {
    block_entry_t();

    explicit block_entry_t(const align_t& align);

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
    [[nodiscard]] uint64_t address() const
    {
        return address_;
    }

    block_entry_t *address(uint64_t value)
    {
        address_ = value;
        for (auto label : labels_) {
            label->address(value);
        }
        return this;
    }

    block_entry_t* blank_lines(uint16_t count)
    {
        blank_lines_ += count;
        return this;
    }

    [[nodiscard]] uint16_t blank_lines() const
    {
        return blank_lines_;
    }

    block_entry_t *label(class label *label);

    [[nodiscard]] block_entry_type_t type() const;

    [[nodiscard]] const std::vector<class label *> &labels() const;

    comment_list_t comments();

    block_entry_t *comment(const std::string& value, uint8_t indent = 0);

private:
    std::any data_;
    block_entry_type_t type_;
    uint64_t address_ = 0;
    uint16_t blank_lines_ = 0;
    std::vector<class label *> labels_{};
    comment_list_t comments_ {};
};
}
#endif // VM_BLOCK_ENTRY_H_
