//
// Created by 12132 on 2022/3/30.
//

#ifndef ALPHA_COMPILER_H_
#define ALPHA_COMPILER_H_

#include "src/vm/terp.h"
#include "src/parser/parser.h"
#include "src/parser/symbol_table.h"

namespace gfx {
	class compiler {
	public:
		explicit compiler(size_t heap_size);

		virtual ~compiler();

		inline uint64_t address() const
		{
			return address_;
		}

		bool initialize(result& r);

		inline symbol_table* symbol_table()
		{
			return &symbol_table_;
		}

		bool compile(result& r, std::istream& input);

		bool compile_stream(result& r, std::istream& input);

	private:
		terp terp_;
		uint64_t address_;

		class symbol_table symbol_table_ {};
	};
}

#endif // ALPHA_COMPILER_H_
