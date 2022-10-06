//
// Created by 12132 on 2022/7/17.
//

#ifndef LABEL_H_
#define LABEL_H_
#include "common/id_pool.h"
#include <string>
namespace gfx {
class label {
public:
    explicit label(const std::string& name);

    [[nodiscard]] uint64_t address() const;

    [[nodiscard]] std::string name() const;

    void address(uint64_t value);

private:
    std::string name_;
    uint64_t address_ = 0;
};

}

#endif // LABEL_H_
