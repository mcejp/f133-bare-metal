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

int main()
{
    // UART3 (pins PB6/PB7): select function 7
    // GPIO->pb_cfg[0] = (7 << (6*4)) | (7 << (7*4));
    gpio_pb_set_func(6, 7);
    gpio_pb_set_func(7, 7);

    // enable clock
	CCU.uart_bgr_reg |= (CCU_UART_BGR_REG_UART0_GATING << 3);
    // disable reset
	CCU.uart_bgr_reg |= (CCU_UART_BGR_REG_UART0_RST << 3);

    // divisor 13 -> 115200 baud
    uart_init(UART3, 13);

    uint64_t my_sp;
    __asm__("mv %0, sp" : "=r"(my_sp));
    uart_putstr(UART3, "sp:      ");
    uart_puthex(UART3, my_sp);
    uart_putc(UART3, '\n');

    uint64_t my_pc;
    __asm__("0: la %0, 0b" : "=r"(my_pc));
    uart_putstr(UART3, "pc:      ");
    uart_puthex(UART3, my_pc);
    uart_putc(UART3, '\n');

    uint64_t my_misa;
    __asm__("csrr %0, misa" : "=r"(my_misa));
    uart_putstr(UART3, "misa:    ");
    uart_puthex(UART3, my_misa);
    uart_putc(UART3, '\n');

    uint64_t my_mstatus;
    __asm__("csrr %0, mstatus" : "=r"(my_mstatus));
    uart_putstr(UART3, "mstatus: ");
    uart_puthex(UART3, my_mstatus);
    uart_putc(UART3, '\n');

    // pin PD22
    GPIO.pd_cfg[2] = (1 << 24);

    int val;
    int ch = 'A';
    for (;;) {
        val ^= (1 << 22);
        GPIO.pd_dat = val;

        for (int i = 0; i < 81'920'000; i++) {
            __asm__ volatile("nop");
        }

        uart_putstr(UART3, "Hello, world\n");
    }
}
