//
// Created by 12132 on 2022/7/17.
//

#ifndef LABEL_H_
#define LABEL_H_
#include "common/id_pool.h"
#include <string>
namespace gfx {
class label;

struct label_ref_t {
    id_t id;
    std::string name;
    label* resolved = nullptr;
};

class label {
public:
    explicit label(const std::string& name);

    uint64_t address() const;

    std::string name() const;

    void address(uint64_t value);

private:
    std::string name_;
    uint64_t address_ = 0;
};

}

#endif // LABEL_H_
