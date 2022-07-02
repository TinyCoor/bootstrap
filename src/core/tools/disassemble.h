//
// Created by 12132 on 2022/3/28.
//

#ifndef DISASSEMBLE_H_
#define DISASSEMBLE_H_
#include <string>
#include "src/common/result.h"
#include "src/vm/instruction.h"
namespace gfx {
class Disassemble {
public:
	std::string disassemble(result& r, uint64_t address);
	std::string disassemble(const instruction_t& inst) const;
};

}

#endif // DISASSEMBLE_H_
