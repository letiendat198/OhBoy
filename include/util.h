#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include "types.h"

class SafeOperations {
public:
    static ArithmeticResult<uint8_t> safe_add(uint8_t a, uint8_t b);
    static ArithmeticResult<uint16_t> safe_add(uint16_t a, uint16_t b);

    static ArithmeticResult<uint8_t> safe_sub(uint8_t a, uint8_t b);
    static ArithmeticResult<uint16_t> safe_sub(uint16_t a, uint16_t b);
};

#endif //UTIL_H
