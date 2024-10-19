#ifndef TYPES_H
#define TYPES_H

#define u_char unsigned char
#define u_short unsigned short

template <typename T>
struct ArithmeticResult {
    T result;
    T carry;
};

#endif //TYPES_H
