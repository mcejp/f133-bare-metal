# source: https://steward-fu.github.io/website/mcu/f133/asm_led.htm

    .global _start
  
    .equ GPIO_BASE, 0x02000000
    .equ PD_CFG2,   0x0098
    .equ PD_DAT,    0x00a0
   
    .text
    .long 0x0a00006f
    .byte 'e','G','O','N','.','B','T','0'
    .long 0x5F0A6C39
    .long 0x8000
    .long 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
 
    .org 0x00a0
_vector:
    j _start
   
    .org 0x0100
_start:
    # stack at end of 64M DDR memory
    la sp, 0x44000000
    j main
    .end
