/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Peter Lawrence
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* this code was tweaked specifically for pico-debug; it is *NOT* a generic crt implementation */

.macro ISR_HANDLER name=
  .section .vectors, "ax"
  .word \name
.endm

.macro ISR_RESERVED
  .section .vectors, "ax"
  .word 0
.endm

.macro ISR_UNUSED name=
  .section .vectors, "ax"
  .word exit_handler
.endm

  .syntax unified
  .global boot_entry
  .extern main
  .extern multicore_launch_core1_raw
  .global exit
  .weak exit

  .section .boot, "ax"
  .thumb_func

boot_entry:
#ifndef STAY_IN_SRAM
  /* disable flash cache (allowing XIP_SRAM access) */
  ldr r0, =0x14000000
  ldr r1, =0
  str r1, [r0]
  /* undocumented step to ensure XIP_SRAM is ready for accesses */
  ldr r1, [r0, #4]
#endif

  ldr r0, =__SRAM_segment_end__
  mov sp, r0
  bl SystemInit

  ldr r2, =__vectors_start__ /* origin of where vector table now resides */
  ldr r1, [r2] /* load stack pointer from user app */
  ldr r0, [r2, #4] /* load reset address from user app */
  ldr r3, =multicore_launch_core1_raw
  blx r3

sleep:
  wfi
  b sleep

  .section .vectors, "ax"
  .code 16  
  .global _vectors  
  .align 8

_vectors:
  .word __stack_end__
  .word reset_handler
ISR_UNUSED NMI_Handler
ISR_UNUSED HardFault_Handler
ISR_UNUSED MemManage_Handler 
ISR_UNUSED BusFault_Handler
ISR_UNUSED UsageFault_Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_UNUSED SVC_Handler
ISR_UNUSED DebugMon_Handler
ISR_RESERVED
ISR_UNUSED PendSV_Handler
ISR_UNUSED SysTick_Handler 
ISR_UNUSED TIMER0_IRQHandler
ISR_UNUSED TIMER1_IRQHandler
ISR_UNUSED TIMER2_IRQHandler
ISR_UNUSED TIMER3_IRQHandler
ISR_UNUSED PWM_IRQHandler
ISR_HANDLER USB_IRQHandler
ISR_UNUSED XIP_IRQHandler
ISR_UNUSED PIO0_0_IRQHandler
ISR_UNUSED PIO0_1_IRQHandler
ISR_UNUSED PIO1_0_IRQHandler
ISR_UNUSED PIO1_1_IRQHandler
ISR_UNUSED DMA0_IRQHandler
ISR_UNUSED DMA1_IRQHandler
ISR_UNUSED IO_IRQHandler
ISR_UNUSED QSPI_IRQHandler
ISR_UNUSED SIO0_IRQHandler
ISR_UNUSED SIO1_IRQHandler
ISR_UNUSED CLOCKS_Handler
ISR_UNUSED SPI0_Handler
ISR_UNUSED SPI1_IRQHandler
ISR_UNUSED UART0_IRQHandler
ISR_UNUSED UART1_Handler
ISR_UNUSED ADC_Handler
ISR_UNUSED I2C0_IRQHandler
ISR_UNUSED I2C1_IRQHandler
ISR_UNUSED RTC_IRQHandler

  .thumb_func
reset_handler:
  /* zero-ize the bss */
  ldr r0, =__bss_start__
  ldr r1, =__bss_end__
  movs r2, #0
bss_loop:
  cmp r0, r1
  beq bss_finished
  strb r2, [r0]
  adds r0, r0, #1
  b bss_loop
bss_finished:

  /* Setup initial call frame */
  movs r0, #0
  mov lr, r0
  mov r12, sp

  /* jump to app entry point */
  ldr r2, =main
  blx r2

  .thumb_func
exit:

  /* handler if app returned or unexpected IRQ happened */
exit_handler:
  b exit_handler

  /* Setup attributes of stack section so that it doesn't take up room in the elf file */
  .section .stack, "wa", %nobits
