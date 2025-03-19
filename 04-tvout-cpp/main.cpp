#include "../f133-hw.h"

static void uart_putc(UART_hw& uart, char c)
{
    while ((uart.usr & UART_USR_TFNF) == 0) {}

    uart.rbr_thr_dll = c;
}

static void uart_init(UART_hw& uart, int divisor)
{
    uart.ier_dlh = 0;
    uart.iir_fcr = 0xf7;
    uart.mcr = 0;

    // Set UART_LCR[DLAB] to 1
    uart.lcr |= (1<<7);

    uart.rbr_thr_dll = divisor & 0xff;
    uart.ier_dlh = (divisor >> 8) & 0xff;
    uart.lcr &= ~(1<<7);
    uart.lcr = (uart.lcr & ~0x1f) | (0x3 << 0) | (0 << 2) | (0x0 << 3);
}

static void uart_putstr(UART_hw& uart, const char* s) {
    while (*s) {
        uart_putc(uart, *s++);
    }
}

static const char hex_alphabet[] = "0123456789abcdef";

static void uart_puthex(UART_hw& uart, uint64_t value) {
    for (int i = 60; i >= 32; i -= 4) {
        uart_putc(uart, hex_alphabet[(value >> i) % 16]);
    }
    uart_putc(uart, '_');
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(uart, hex_alphabet[(value >> i) % 16]);
    }
}

static void gpio_pb_set_func(int pin, int func) {
    uint32_t cfg = GPIO.pb_cfg[pin / 8];
    cfg &= ~(0xf << ((pin % 8) * 4));
    cfg |= ((func & 0xf) << ((pin % 8) * 4));
    GPIO.pb_cfg[pin / 8] = cfg;
}

static inline uint64_t counter(void)
{
    uint64_t cnt;
     __asm__ __volatile__("csrr %0, time\n" : "=r"(cnt) :: "memory");
    return cnt;
}

void sdelay(unsigned long us)
{
    uint64_t t1 = counter();
    uint64_t t2 = t1 + us * 24;
    do {
        t1 = counter();
    } while (t2 >= t1);
}

static uint32_t read32(long address) {
    return *(volatile uint32_t*) address;
}
static void write32(long address, uint32_t value) {
    *(volatile uint32_t*) address = value;
}

#define PLL_VIDEO0_CTRL_REG 0x0040

// Adapted from https://github.com/xboot/xfel/blob/master/payloads/d1_f133/f133-ddr/source/sys-clock.c#L212
static void set_video0_pll()
{
    uint32_t val;
    long addr = (long) &CCU.pll_video0_ctrl_reg;

    // disable PLL, disable lock
    write32(addr, read32(addr) & ~((1 << 31) | (1 << 29)));

    if (!(read32(addr) & (1 << 31)))
    {
        val = read32(addr);
        // write32(addr, val | (1 << 31) | (1 << 30));

        // Change frequency to 4*216 MHz

        // unknown: should set lowest bit?
        write32(addr, (1<<31) | (1<<30) | (1<<27) | (71 << 8) | (1<<1) | (1<<0));

        // UNKNOWN: may need o disable /2 to compoensate for "div2y" block

        /* Lock enable */
        val = read32(addr);
        val |= (1 << 29);
        write32(addr, val);

        /* Wait pll stable */
        while(!(read32(addr) & (0x1 << 28)));
        sdelay(20);

        /* Lock disable */
        val = read32(addr);
        val &= ~(1 << 29);
        write32(addr, val);
    }
}

int main()
{
    // UART3 (pins PB6/PB7): select function 7
    gpio_pb_set_func(6, 7);
    gpio_pb_set_func(7, 7);

    // enable clock & disable reset
    CCU.uart_bgr_reg |= (CCU_UART_BGR_REG_UART0_GATING << 3);
    CCU.uart_bgr_reg |= (CCU_UART_BGR_REG_UART0_RST << 3);

    // divisor 13 -> 115200 baud
    uart_init(UART3, 13);

    uart_putstr(UART3, "Hello world\n");
    uart_putstr(UART3, "set_video0_pll\n");

    set_video0_pll();

    uart_putstr(UART3, "enable it\n");

    // Enable TVE peripheral, take out of reset
    // clk = PLL_VIDEO0(1x)
    CCU.tve_clk_reg = 0x8000'0000;
    CCU.tve_bgr_reg = 0x0003'0003;

    uart_putstr(UART3, "program registers\n");

    // Program registers as per maybe:
    //   https://github.com/AvaotaSBC/linux/blob/main/bsp/drivers/video/sunxi/disp2/tv/drv_tv.c
    TVE_TOP.dac_map = 0x1;
    // TVE_DAC_CFG0: set enable=1
    TVE_TOP.dac_cfg[0] |= (1<<0);
    // TODO: TVE_DAC_CFG1/2 ?

    TVE.tve_000_reg = TVE_000_TVE_EN | TVE_000_CLOCK_GATE_DIS;
    // enable color bars
    TVE.tve_004_reg = TVE_004_CVBS_EN | TVE_004_COLOR_BAR_MODE | TVE_004_COLOR_BAR_TYPE;
    uart_putstr(UART3, "done.\n");

    for (;;) {
    }
}
