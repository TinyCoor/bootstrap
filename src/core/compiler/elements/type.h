//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "element.h"
#include "element_types.h"
namespace gfx::compiler {

/// todo make its template
// enum class type_t {
// 	int_t,
// 	float_t,
// };
//
// template<type_t, template<type_t> DataHolder>
// class types {
//   using type = type_t
// };
//
// template<>
// class types<type_t::int_t>
// {
//
// };
/// template<>
class type : public element {
public:
    type(block* parent_scope, element_type_t type, const std::string& name);

	~type() override;

	std::string name() const;

	void name(const std::string& value);

    size_t size_in_bytes() const;

    bool initialize(result &r, compiler::program* program);
protected:
	void size_in_bytes(size_t value);

	virtual bool on_initialize(result& r, compiler::program* program);
private:
	std::string name_;
	size_t size_in_bytes_ {};
};

}
#endif // COMPILER_ELEMENTS_TYPE_H_
