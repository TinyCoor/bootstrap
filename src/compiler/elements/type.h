//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "element.h"
#include "element_types.h"
namespace gfx::compiler {
class type : public element {
public:
	explicit type(const std::string& name);

	~type() override;

	inline std::string name() const
	{
		return name_;
	}

private:
	std::string name_;
};

struct type_map_t {
	void add(const std::string& name, compiler::type* type)
	{
		types_.insert(std::make_pair(name, type));
	}

	size_t size() const
	{
		return types_.size();
	}

	bool remove(const std::string& name)
	{
		return types_.erase(name) > 0;
	}

	compiler::type* find(const std::string& name)
	{
		auto it = types_.find(name);
		if (it != types_.end()) {
			return it->second;
		}
		return nullptr;
	}

private:
	std::unordered_map<std::string, type*> types_ {};
};
}
#endif // COMPILER_ELEMENTS_TYPE_H_
