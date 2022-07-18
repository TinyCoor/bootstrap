//
// Created by 12132 on 2022/7/17.
//

#ifndef BOOTSTRAP_SRC_CORE_VM_INSTRUCTION_BLOCK_H_
#define BOOTSTRAP_SRC_CORE_VM_INSTRUCTION_BLOCK_H_
#include "instruction.h"
#include "label.h"
#include <vector>
#include <string>
namespace gfx {
class instruction_block {
public:
    instruction_block();

    virtual ~instruction_block();

    void push(double value);

    void push(uint64_t value);

    void call(const std::string& proc_name);

    label* make_label(const std::string& name);

private:
    std::vector<instruction_block*> blocks_ {};
    std::vector<instruction_t> instructions_ {};
    std::map<std::string, label*> labels_ {};
    std::map<std::string, size_t> label_to_instruction_map_{};
};
}

#endif //BOOTSTRAP_SRC_CORE_VM_INSTRUCTION_BLOCK_H_
