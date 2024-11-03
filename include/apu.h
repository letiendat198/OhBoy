#ifndef APU_H
#define APU_H
#include <cstdint>

class Channel1 {
private:
    uint8_t sweep;

};

class Channel2 {

};

class Channel3 {

};

class Channel4 {

};

class APU {
private:
    Channel1 channel1;
    Channel2 channel2;
    Channel3 channel3;
    Channel4 channel4;
public:
    void tick();
};

#endif //APU_H
