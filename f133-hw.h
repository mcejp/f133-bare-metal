#ifndef F133_HW_H
#define F133_HW_H

#include <stdint.h>

#define GPIO_BASE 0x02000000

struct GPIO_hw {
    volatile uint32_t unused_0[0x98/4];
    volatile uint32_t pd_cfg2;
    volatile uint32_t unused_009c;
    volatile uint32_t pd_dat;
};

#define GPIO ((struct GPIO_hw*) GPIO_BASE)

#endif
