#include "util.h"

ArithmeticResult<u_char> SafeOperations::safe_add(u_char a, u_char b) {
    u_char max = 0xFF;
    u_char dif = max - a;
    u_char result;
    u_char carry=0;
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

ArithmeticResult<u_short> SafeOperations::safe_add(u_short a, u_short b) {
    u_short max = 0xFFFF;
    u_short dif = max - a;
    u_short result;
    u_short carry=0;
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

ArithmeticResult<u_char> SafeOperations::safe_sub(u_char a, u_char b) {
    u_char result;
    u_char carry = 0;
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

ArithmeticResult<u_short> SafeOperations::safe_sub(u_short a, u_short b) {
    u_short result;
    u_short carry = 0;
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



