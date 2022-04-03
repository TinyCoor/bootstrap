//
// Created by 12132 on 2022/3/30.
//

#ifndef PARSER_H_
#define PARSER_H_
#include "result.h"
#include "ast.h"
#include <map>
#include <stack>
#include <string>
namespace gfx {

class symbol_table;

class parser {
public:
	parser() = default;

	const result &result() const{
		return result_;
	}

private:
	class result result_;
};

}
#endif // PARSER_H_
