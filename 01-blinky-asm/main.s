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
    li t0, (1 << 24)
    li a0, GPIO_BASE + PD_CFG2
    sw t0, 0(a0)
 
    li t1, (1 << 22)
0:
    xor t0, t0, t1
    li a0, GPIO_BASE + PD_DAT
    sw t0, 0(a0)
  
    lui t2, 10000
1:
    nop
    addi t2, t2, -1
    bgtz t2, 1b
    j 0b
    .end
