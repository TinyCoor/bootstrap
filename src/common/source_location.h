//
// Created by 12132 on 2022/9/2.
//

#ifndef COMMON_SOURCE_LOCATION_H_
#define COMMON_SOURCE_LOCATION_H_
#include <cstdint>

class source_location {
public:
    source_location();

    source_location(uint32_t line, uint32_t start_column,
                    uint32_t end_column);

    uint32_t line() const;

    void line(uint32_t value);

    uint32_t end_column() const;

    uint32_t start_column() const;

    void end_column(uint32_t value);

    void start_column(uint32_t value);
private:
    uint32_t line_ = 0;
    uint32_t end_column_ = 0;
    uint32_t start_column_ = 0;
};

#endif // COMMON_SOURCE_LOCATION_H_
