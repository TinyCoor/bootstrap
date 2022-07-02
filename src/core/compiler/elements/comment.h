//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_COMMENT_H_
#define COMPILER_ELEMENTS_COMMENT_H_
#include "element.h"
namespace gfx::compiler {
class comment : public element {
public:
	comment(element* parent, comment_type_t type, const std::string& value);

	std::string value() const ;

	comment_type_t type() const;

private:
	std::string value_;
	comment_type_t type_;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_COMMENT_H_
