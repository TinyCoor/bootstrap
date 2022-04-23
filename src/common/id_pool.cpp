//
// Created by 12132 on 2022/4/23.
//

#include "id_pool.h"
#include <numeric>

namespace gfx {
id_pool::id_pool() {
	pool_.insert(id_interval(1, std::numeric_limits<id_t>::max()));
}

id_t id_pool::allocate() {
	id_interval first = *(pool_.begin());
	auto id = first.left();
	pool_.erase(pool_.begin());
	if (first.left() + 1 <= first.right()) {
		pool_.insert(id_interval(first.left() + 1 , first.right()));
	}
	return id;
}

id_pool* id_pool::instance() {
	static id_pool pool;
	return &pool;
}

void id_pool::release(id_t id) {
	auto it = pool_.find(id_interval(id, id));
	if (it != pool_.end() && it->left() <= id && it->right() > id) {
		return;
	}
	it = pool_.upper_bound(id_interval(id, id));
	if (it == pool_.end()) {
		return;
	} else {
		id_interval _interval = *(it);
		if (id + 1 != _interval.left()) {
			pool_.insert(id_interval(id, id));
		} else {
			if (it != pool_.begin()) {
				auto it_2 = it;
				--it_2;
				if (it_2->right() + 1 == id) {
					id_interval _interval2 = *(it_2);
					pool_.erase(it);
					pool_.erase(it_2);
					pool_.insert(
						id_interval(_interval2.left(),
									_interval.right()));
				} else {
					pool_.erase(it);
					pool_.insert(id_interval(id, _interval.right()));
				}
			} else {
				pool_.erase(it);
				pool_.insert(id_interval(id, _interval.right()));
			}
		}
	}
}

bool id_pool::mark_used(id_t id) {
	auto it = pool_.find(id_interval(id, id));
	if (it == pool_.end()) {
		return false;
	} else {
		id_interval free_interval = *(it);
		pool_.erase(it);
		if (free_interval.left() < id) {
			pool_.insert(id_interval(free_interval.left(), id - 1));
		}
		if (id + 1 <= free_interval.right()) {
			pool_.insert(id_interval(id + 1, free_interval.right()));
		}
		return true;
	}
}

bool id_pool::mark_range(id_t start_id, id_t end_id) {
	for (size_t id = static_cast<size_t>(start_id);
		 id < static_cast<size_t>(end_id);
		 ++id) {
		auto success = mark_used(static_cast<id_t>(id));
		if (!success)
			return false;
	}
	return true;
}

id_t id_pool::allocate_from_range(id_t start_id, id_t end_id) {
	for (size_t id = static_cast<size_t>(start_id); id < static_cast<size_t>(end_id); ++id) {
		auto success = mark_used(static_cast<id_t>(id));
		if (success) {
			return static_cast<id_t>(id);
		}
	}
	return 0;
}

}