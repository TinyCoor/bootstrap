//
// Created by 12132 on 2022/7/30.
//

#ifndef VM_STACK_FRAME_H_
#define VM_STACK_FRAME_H_
#include <map>
#include <stack>
#include <string>
#include <string_view>
#include <cstdint>
namespace gfx{
enum class stack_frame_entry_type_t : uint8_t {
    local = 1,
    parameter,
    return_slot
};

inline static std::string_view stack_frame_entry_type_name(stack_frame_entry_type_t type) {
    switch (type) {
        case stack_frame_entry_type_t::local:
            return "local";
        case stack_frame_entry_type_t::parameter:
            return "parameter";
        case stack_frame_entry_type_t::return_slot:
            return "return_slot";
        default:  return "unknown";
    }
}

struct stack_frame_entry_t {
    int32_t offset;
    std::string name;
    stack_frame_entry_type_t type;
};
struct stack_frame_t;
struct stack_frame_t {
    explicit stack_frame_t(stack_frame_t* parent);

    stack_frame_entry_t* add(stack_frame_entry_type_t type, const std::string& name, int32_t offset);

    stack_frame_entry_t* find(const std::string& name);

    stack_frame_entry_t* find_up(const std::string& name);

    stack_frame_t* parent_ = nullptr;
    std::map<std::string, stack_frame_entry_t> entries {};
};
}
#endif // VM_STACK_FRAME_H_
