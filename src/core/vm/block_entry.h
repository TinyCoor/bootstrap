//
// Created by 12132 on 2022/8/5.
//

#ifndef VM_BLOCK_ENTRY_H_
#define VM_BLOCK_ENTRY_H_
#include <any>
#include <vector>
#include <string>
#include "instruction.h"

namespace gfx {
enum class section_t : uint8_t {
    bss = 1,
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
    }
    return "unknown";
}

struct align_t {
    uint8_t size = 0;
};

enum data_definition_type_t : uint8_t {
    initialized = 1,
    uninitialized
};

struct data_definition_t {
    op_sizes size;
    uint64_t value = 0;
    data_definition_type_t type;
};

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

    inline void blank_lines(uint16_t count)
    {
        blank_lines_ += count;
    }

    inline uint16_t blank_lines() const
    {
        return blank_lines_;
    }

    void label(class label *label);

    [[nodiscard]] block_entry_type_t type() const;

    void comment(const std::string &value);

    [[nodiscard]] const std::vector<class label *> &labels() const;

    [[nodiscard]] const std::vector<std::string> &comments() const;

private:
    std::any data_;
    block_entry_type_t type_;
    uint16_t blank_lines_ = 0;
    std::vector<class label *> labels_{};
    std::vector<std::string> comments_{};
};
}
#endif // VM_BLOCK_ENTRY_H_
