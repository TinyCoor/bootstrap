//
// Created by 12132 on 2022/9/2.
//

#ifndef COMMON_SOURCE_LOCATION_H_
#define COMMON_SOURCE_LOCATION_H_
#include <cstdint>

struct location_t {
    uint32_t line = 0;
    uint32_t column = 0;
};

class source_location {
public:
    [[nodiscard]] const location_t &end() const;

    [[nodiscard]] const location_t &start() const;

    void end(const location_t &value);

    void start(const location_t &value);

    void end(uint32_t line, uint32_t column);

    void start(uint32_t line, uint32_t column);

private:
    location_t start_ {};
    location_t end_ {};
};

#endif // COMMON_SOURCE_LOCATION_H_
