//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FIELD_H_
#define COMPILER_ELEMENTS_FIELD_H_
#include <map>
#include <string>
#include <memory>
#include "type.h"
#include "element.h"
#include "initializer.h"
namespace gfx::compiler {
class field : public element {
public:
	field(element* parent, const std::string& name, type* type, initializer* initializer);

	~field() override;

	compiler::type* type();

	std::string name() const;

	bool inferred_type() const;

	void type(compiler::type* t);

	compiler::initializer* initializer();

	void initializer(compiler::initializer* v);

private:
	std::string name_;
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
	compiler::initializer* initializer_ = nullptr;
};

struct field_map_t {
	field_map_t(element* parent) : parent_(parent) {
	}

	~field_map_t() {
		for (auto field : fields_)
			delete field.second;
		fields_.clear();
	}

	void add(const std::string& name, compiler::type* type, compiler::initializer* initializer)
	{
		auto field = new compiler::field(parent_, name, type, initializer);
		field->type(type);
		fields_.insert(std::make_pair(name, field));
	}

	inline size_t size() const
	{
		return fields_.size();
	}

	bool remove(const std::string& name)
	{
		return fields_.erase(name) > 0;
	}

	compiler::field* find(const std::string& name)
	{
		auto it = fields_.find(name);
		if (it != fields_.end()) {
			return it->second;
		}
		return nullptr;
	}

private:
	element* parent_ = nullptr;
	std::unordered_map<std::string, field*> fields_ {};
};
}

#endif // COMPILER_ELEMENTS_FIELD_H_
