//
// Created by 12132 on 2022/4/23.
//

#ifndef COMMON_ID_POOL_H_
#define COMMON_ID_POOL_H_
#include <set>
#include <cstdint>
#include <boost/numeric/interval.hpp>
namespace gfx {
using id_t = uint32_t;
class id_interval {
public:
	id_interval(id_t ll, id_t uu) : value_(ll, uu) {}

	bool operator<(const id_interval& s) const
	{
		return (value_.lower() < s.value_.lower()) &&
				(value_.upper() < s.value_.lower());
	}

	[[nodiscard]] id_t left() const
	{
		return value_.lower();
	}

	[[nodiscard]] id_t right() const
	{
		return value_.upper();
	}

private:
	boost::numeric::interval<id_t> value_;
};

typedef std::set<id_interval> id_set;

class id_pool {
public:
	static id_pool* instance();

	id_t allocate();

	void release(id_t id);

	bool mark_used(id_t id);

    [[maybe_unused]] bool mark_range(id_t start_id, id_t end_id);

    [[maybe_unused]] id_t allocate_from_range(id_t start_id, id_t end_id);

protected:
	id_pool();

private:
	id_set pool_;
};

}

#endif // COMMON_ID_POOL_H_
