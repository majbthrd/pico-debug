#ifndef _RP2040_H
#define _RP2040_H

#define __NVIC_PRIO_BITS          2

typedef enum
{
  NonMaskableInt_IRQn = -14,
  HardFault_IRQn      = -13,
  SVC_IRQn            = -5,
  PendSV_IRQn         = -2,
  SysTick_IRQn        = -1,

  TIMER0_IRQn         = 0,
  TIMER1_IRQn         = 1,
  TIMER2_IRQn         = 2,
  TIMER3_IRQn         = 3,
  PWM_IRQn            = 4,
  USB_IRQn            = 5,
  XIP_IRQn            = 6,
  PIO0_0_IRQn         = 7,
  PIO0_1_IRQn         = 8,
  PIO1_0_IRQn         = 9,
  PIO1_1_IRQn         = 10,
  DMA0_IRQn           = 11,
  DMA1_IRQn           = 12,
  IO_IRQn             = 13,
  QSPI_IRQn           = 14,
  SIO0_IRQn           = 15,
  SIO1_IRQn           = 16,
  CLOCKS_IRQn         = 17,
  SPI0_IRQn           = 18,
  SPI1_IRQn           = 19,
  UART0_IRQn          = 20,
  UART1_IRQn          = 21,
  ADC_IRQn            = 22,
  I2C0_IRQn           = 23,
  I2C1_IRQn           = 24,
  RTC_IRQn            = 25,
} IRQn_Type;

#include "core_cm0plus.h"

#include "hardware/regs/addressmap.h"

#include "hardware/regs/adc.h"
#include "hardware/regs/busctrl.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/dma.h"
#include "hardware/regs/dreq.h"
#include "hardware/regs/i2c.h"
#include "hardware/regs/intctrl.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/io_qspi.h"
#include "hardware/regs/m0plus.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/pads_qspi.h"
#include "hardware/regs/pio.h"
#include "hardware/regs/pll.h"
#include "hardware/regs/psm.h"
#include "hardware/regs/pwm.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/rosc.h"
#include "hardware/regs/rtc.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/spi.h"
#include "hardware/regs/ssi.h"
#include "hardware/regs/syscfg.h"
#include "hardware/regs/sysinfo.h"
#include "hardware/regs/tbman.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/usb.h"
#include "hardware/regs/vreg_and_chip_reset.h"
#include "hardware/regs/watchdog.h"
#include "hardware/regs/xip.h"
#include "hardware/regs/xosc.h"

#include "hardware/structs/adc.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/dma.h"
#include "hardware/structs/i2c.h"
#include "hardware/structs/interp.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/mpu.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/pads_qspi.h"
#include "hardware/structs/pio.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/psm.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/resets.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/rtc.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/spi.h"
#include "hardware/structs/ssi.h"
#include "hardware/structs/syscfg.h"
#include "hardware/structs/systick.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/uart.h"
#include "hardware/structs/usb.h"
#include "hardware/structs/vreg_and_chip_reset.h"
#include "hardware/structs/watchdog.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/structs/xosc.h"

#endif
