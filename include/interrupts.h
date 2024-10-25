#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <types.h>

class Interrupts {
private:

public:
    static u_short check_and_service(u_char &ime);
    static void set_if(u_char bit);
    static void reset_if(u_char bit);
    static u_char is_pending();
};

#endif //INTERRUPTS_H
