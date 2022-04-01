//
// Created by 12132 on 2022/3/30.
//

#ifndef ALPHA_COMPILER_H_
#define ALPHA_COMPILER_H_

#include <cstdint>
#include "terp.h"
#include "alpha_parser.h"
#include "symbol_table.h"
#include "evaluator.h"

namespace gfx {
	class alpha_compiler {
	public:
		explicit alpha_compiler(size_t heap_size);

		virtual ~alpha_compiler();

		inline uint64_t address() const
		{
			return address_;
		}

		bool initialize(result& r);

		inline symbol_table* symbol_table()
		{
			return &symbol_table_;
		}

		bool compile(result& r, const parser_input_t& input);

		bool compile_stream(result& r, const parser_input_t& input);

	private:
		terp terp_;
		uint64_t address_;
		evaluator evaluator_;
		alpha_parser parser_ {};
		class symbol_table symbol_table_ {};
	};
}

#endif // ALPHA_COMPILER_H_
