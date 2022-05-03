//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_IDENTIFIER_H_
#define COMPILER_ELEMENTS_IDENTIFIER_H_

#include "type.h"
#include "element.h"
#include "initializer.h"
namespace gfx::compiler {

class identifier : public element {
public:
	identifier(element* parent, const std::string& name, const compiler::initializer& initializer);

	~identifier() override;

	compiler::type* type();

	std::string name() const;

	bool is_constant() const;

	bool inferred_type() const;

	void type(compiler::type* t);

	const compiler::initializer& initializer() const;

private:
	std::string name_;
	bool constant_ = false;
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
	compiler::initializer initializer_;
};

struct identifier_map_t {
	void add(const std::string& name, identifier* identifier)
	{
		identifiers_.insert(std::make_pair(name, identifier));
	}

	size_t size() const
	{
		return identifiers_.size();
	}

	bool remove(const std::string& name)
	{
		return identifiers_.erase(name) > 0;
	}

	identifier* find(const std::string& name)
	{
		auto it = identifiers_.find(name);
		if (it != identifiers_.end()) {
			return it->second;
		}
		return nullptr;
	}

	// XXX: add ability to get range of identifiers for overloads

private:
	std::unordered_multimap<std::string, identifier*> identifiers_ {};
};
}
#endif // COMPILER_ELEMENTS_IDENTIFIER_H_
