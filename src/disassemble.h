//
// Created by 12132 on 2022/3/28.
//

#ifndef BOOTSTRAP__DISASSEMBLE_H_
#define BOOTSTRAP__DISASSEMBLE_H_
#include <string>
#include "result.h"
#include "instruction.h"
namespace gfx {
class Disassemble {
public:
	std::string disassemble(result& r, uint64_t address);
	std::string disassemble(const instruction_t& inst) const;
};

}

#endif //BOOTSTRAP__DISASSEMBLE_H_
