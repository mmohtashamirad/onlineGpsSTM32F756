#ifndef SYSTEM_CLOCK_AND_TIME_HXX
#define SYSTEM_CLOCK_AND_TIME_HXX
#include <stdint.h>

class ClockAndTime
{
public:
    static uint64_t getSystem_uS();
    static uint64_t getSystem_mS();
    static void periodicTimerInit();
    static void delay_ms(uint32_t dly);
    static void delay_us(uint32_t dly);
    static void softDelay_ms(uint32_t dly);
    static void IncreaseMillies();

private:
    static uint64_t mili_seconds_;
    static uint64_t micro_seconds_;
};

#endif // SYSTEM_CLOCK_AND_TIME_HXX
