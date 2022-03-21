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

#include "bsp/board.h"
#include "tusb.h"
#include <rp2040.h>

void board_init(void) {}
void irq_set_exclusive_handler(unsigned int num, void *handler) {}

/* cut out the middleman and map the interrupt directly to the handler */
void USB_IRQHandler(void)
{
  dcd_int_handler(0);
}

/* map rp2040 USB driver call into CMSIS standard calls */
void irq_set_enabled(unsigned int num, bool enabled)
{
  if (enabled)
    NVIC_EnableIRQ(USB_IRQn);
  else
    NVIC_DisableIRQ(USB_IRQn);
}

/* provide something basic to handle when the rp2040 USB driver gets upset */

__NO_RETURN static void bad_happened(void)
{
#ifdef DEBUG
  while (1) __BKPT();
#else
  NVIC_SystemReset();
#endif
}

void panic(const char *fmt, ...) { bad_happened(); }
void __assert(const char *__expression, const char *__filename, int __line) { bad_happened(); }
void hard_assertion_failure(void) { bad_happened(); }