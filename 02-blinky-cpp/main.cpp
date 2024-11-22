#include "../f133-hw.h"

int main()
{
    // pin PD22
    GPIO->pd_cfg2 = (1 << 24);

    int val;
    for (;;) {
        val ^= (1 << 22);
        GPIO->pd_dat = val;

        for (int i = 0; i < 81'920'000; i++) {
            __asm__ volatile("nop");
        }
    }
}
