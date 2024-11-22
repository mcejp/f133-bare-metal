#include "../f133-hw.h"

static void gpio_pd_set_func(int pin, int func) {
    uint32_t cfg = GPIO.pd_cfg[pin / 8];
    cfg &= ~(0xf << ((pin % 8) * 4));
    cfg |= ((func & 0xf) << ((pin % 8) * 4));
    GPIO.pd_cfg[pin / 8] = cfg;
}

int main()
{
    int pin = 22;   // PD22

    gpio_pd_set_func(pin, 1);

    int val;
    for (;;) {
        val ^= (1 << pin);
        GPIO.pd_dat = val;

        for (int i = 0; i < 81'920'000; i++) {
            __asm__ volatile("nop");
        }
    }
}
