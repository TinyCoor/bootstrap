//
// Created by 12132 on 2022/4/20.
//

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include "common/result.h"
#include "terp.h"
#include <istream>

namespace gfx {
class assembler {
public:
	assembler();

	virtual ~assembler();

	bool assemble(result& r,  terp& t, std::istream& source, uint64_t address);

private:

};
}
#endif // ASSEMBLER_H_
