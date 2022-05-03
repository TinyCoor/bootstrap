//
// Created by 12132 on 2022/5/3.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_LINE_COMMENT_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_LINE_COMMENT_H_
#include "element.h"
namespace gfx::compiler {
class line_comment : public element {
public:
	line_comment(element* parent, const std::string& value);
	line_comment(element* parent, std::string&& value);

	std::string value() const
	{
		return value_;
	}
private:
	std::string value_;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_LINE_COMMENT_H_
