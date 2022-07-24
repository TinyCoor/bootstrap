//
// Created by 12132 on 2022/7/24.
//

#ifndef VM_REGISTER_H_
#define VM_REGISTER_H_
#include <cstdint>
#include <variant>
#include <array>
#include <optional>
template<typename T>
struct register_t {
    static constexpr uint8_t register_slot_size = sizeof(T);
    static constexpr uint8_t register_size = 64u;
    using value_type = T;
    using reference_type = T&;
    using pointer_type = T*;
    std::array<T, 64> val;
};

template<typename T>
struct i_register_file {
    static constexpr uint8_t register_slot_size = register_t<T>::register_slot_size;
    static constexpr uint8_t register_size = register_t<T>::register_size;
    register_t<T> i_registers;
    uint64_t pc;
    uint64_t sp;
    uint64_t fr;
    uint64_t sr;
};

template<typename T, typename = std::enable_if<std::is_floating_point_v<T>>>
struct f_register_file {
    register_t<T> f_registers;
    uint64_t fp;
};

template<typename T>
struct register_file {
    static constexpr bool hasFPU = false;
    static constexpr uint8_t register_slot_size = i_register_file<T>::register_slot_size;
    static constexpr uint8_t register_size = i_register_file<T>::register_size;
    i_register_file<T> register_file;
};

template<>
struct register_file<uint32_t>
{
    static constexpr bool hasFPU = true;
    static constexpr uint8_t register_slot_size = i_register_file<uint32_t>::register_slot_size;
    static constexpr uint8_t register_size = i_register_file<uint32_t>::register_size;
    i_register_file<uint32_t> i_file;
    f_register_file<float> f_file;
};

template<>
struct register_file<uint64_t>
{
    static constexpr bool hasFPU = true;
    static constexpr uint8_t register_slot_size = i_register_file<uint64_t>::register_slot_size;
    static constexpr uint8_t register_size = i_register_file<uint64_t>::register_size;
    i_register_file<uint64_t> i_file;
    f_register_file<double> f_file;
};

#endif // VM_REGISTER_H_
