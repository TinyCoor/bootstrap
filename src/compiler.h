//
// Created by 12132 on 2022/3/30.
//

#ifndef COMPILER_H_
#define COMPILER_H_

#include "vm/terp.h"
#include "parser/parser.h"
#include "parser/scope.h"

namespace gfx {
	class compiler {
	public:
		explicit compiler(size_t heap_size, size_t stack_size);

		virtual ~compiler();

		bool initialize(result& r);

		bool compile(result& r, std::istream& input);

		bool compile_stream(result& r, std::istream& input);

	private:
		void build_scope_tree(result& r, scope* scope, const ast_node_shared_ptr& node);
	private:
		terp terp_;
		scope global_scope;
	};
}

#endif // COMPILER_H_
