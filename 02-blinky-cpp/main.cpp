#include <cstdint>

// TODO: already out there?
#define GPIO_BASE 0x02000000
// #define PD_CFG2   0x0098
// #define PD_DAT    0x00a0

struct GPIO_hw {
    volatile uint32_t unused_0[0x98/4];
    volatile uint32_t pd_cfg2;
    volatile uint32_t unused_009c;
    volatile uint32_t pd_dat;
};

#define GPIO ((struct GPIO_hw*) GPIO_BASE)

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
