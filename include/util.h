#ifndef UTIL_H
#define UTIL_H

#include "types.h"

class SafeOperations {
public:
    static ArithmeticResult<u_char> safe_add(u_char a, u_char b);
    static ArithmeticResult<u_short> safe_add(u_short a, u_short b);

    static ArithmeticResult<u_char> safe_sub(u_char a, u_char b);
    static ArithmeticResult<u_short> safe_sub(u_short a, u_short b);
};

#endif //UTIL_H
