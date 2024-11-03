#include "../include/util.h"

ArithmeticResult<uint8_t> SafeOperations::safe_add(uint8_t a, uint8_t b) {
    uint8_t max = 0xFF;
    uint8_t dif = max - a;
    uint8_t result;
    uint8_t carry=0;
    if (b<=dif) result = a + b;
    else {
        result = b - dif - 1;
        carry = 1;
    }
    return ArithmeticResult {
        result,
        carry
    };
}

ArithmeticResult<uint16_t> SafeOperations::safe_add(uint16_t a, uint16_t b) {
    uint16_t max = 0xFFFF;
    uint16_t dif = max - a;
    uint16_t result;
    uint16_t carry=0;
    if (b<=dif) result = a +b;
    else {
        result = b - dif - 1;
        carry = 1;
    }
    return ArithmeticResult {
        result,
        carry
    };
}

ArithmeticResult<uint8_t> SafeOperations::safe_sub(uint8_t a, uint8_t b) {
    uint8_t result;
    uint8_t carry = 0;
    if (a >= b) result = a-b;
    else {
        result = 0xFF - (b-a) + 1;
        carry = 1;
    }
    return ArithmeticResult {
        result,
        carry
    };
}

ArithmeticResult<uint16_t> SafeOperations::safe_sub(uint16_t a, uint16_t b) {
    uint16_t result;
    uint16_t carry = 0;
    if (a >= b) result = a-b;
    else {
        result = 0xFFFF - (b-a) + 1;
        carry = 1;
    }
    return ArithmeticResult {
        result,
        carry
    };
}



