#ifndef F133_HW_H
#define F133_HW_H

#include <stdint.h>

#define GPIO_BASE 0x02000000
#define CCU_BASE 0x02001000
#define UART0_BASE 0x02500000
#define UART3_BASE 0x02500c00

#define CCU_UART_BGR_REG_UART0_GATING (1<<0)
#define CCU_UART_BGR_REG_UART0_RST (1<<16)

#define UART_USR_TFNF (1<<1)

struct CCU_hw {
    volatile uint32_t unused_0[0x90c/4];
    volatile uint32_t uart_bgr_reg;
};

struct GPIO_hw {
    volatile uint32_t unused_0[0x30/4];
    volatile uint32_t pb_cfg[2];
    volatile uint32_t unused_38[(0x90-0x38)/4];
    volatile uint32_t pd_cfg[3];
    volatile uint32_t unused_9c;
    volatile uint32_t pd_dat;
};

struct UART_hw {
    volatile uint32_t rbr_thr_dll;
    volatile uint32_t ier_dlh;
    volatile uint32_t iir_fcr;
    volatile uint32_t lcr;
    volatile uint32_t mcr;
    volatile uint32_t lsr;
    volatile uint32_t msr;
    volatile uint32_t sch;
    volatile uint32_t unused_20[(0x7c-0x20) / 4];
    volatile uint32_t usr;
};

#define CCU (*(struct CCU_hw*) CCU_BASE)
#define GPIO (*(struct GPIO_hw*) GPIO_BASE)
#define UART3 (*(struct UART_hw*) UART3_BASE)

#endif
