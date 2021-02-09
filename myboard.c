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

#include "hardware/pll.h"

/*
This is a more streamlined alternative to the current pico-sdk based TinyUSB board support package.
Sticking to C and avoiding all that C++ yields a much smaller executable.
*/

/* overhaul of clock_configure() from pico-sdk to use much less memory */
bool simple_clock_configure(enum clock_index clk_index, uint32_t src, uint32_t auxsrc, bool glitchless)
{
  const uint32_t div = 0x100; /* always 1:1 ratio */

  clock_hw_t *clock = &clocks_hw->clk[clk_index];

  // If increasing divisor, set divisor before source. Otherwise set source
  // before divisor. This avoids a momentary overspeed when e.g. switching
  // to a faster source and increasing divisor to compensate.
  if (div > clock->div)
    clock->div = div;

  // If switching a glitchless slice (ref or sys) to an aux source, switch
  // away from aux *first* to avoid passing glitches when changing aux mux.
  // Assume (!!!) glitchless source 0 is no faster than the aux source.
  if (glitchless)
  {
    hw_clear_bits(&clock->ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
    while (!(clock->selected & 1u));
  }
  // If no glitchless mux, cleanly stop the clock to avoid glitches
  // propagating when changing aux mux. Note it would be a really bad idea
  // to do this on one of the glitchless clocks (clk_sys, clk_ref).
  else
  {
    hw_clear_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
  }

  // Set aux mux first, and then glitchless mux if this clock has one
  hw_write_masked(&clock->ctrl,
    (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
    CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS
  );

  if (glitchless)
  {
    hw_write_masked(&clock->ctrl,
      src << CLOCKS_CLK_REF_CTRL_SRC_LSB,
      CLOCKS_CLK_REF_CTRL_SRC_BITS
    );
    while (!(clock->selected & (1u << src)));
  }

  hw_set_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);

  // Now that the source is configured, we can trust that the user-supplied
  // divisor is a safe value.
  clock->div = div;

  return true;
}

static void usb_clock_init(void)
{
  hw_set_bits(&resets_hw->reset, RESETS_RESET_PLL_USB_BITS);
  hw_clear_bits(&resets_hw->reset, RESETS_RESET_PLL_USB_BITS);
  while (~resets_hw->reset_done & RESETS_RESET_PLL_USB_BITS);

  pll_init(pll_usb_hw, 1, 480 * 1000000, 5, 2);

  // CLK SYS = PLL USB (48MHz) / 1 = 48MHz
  simple_clock_configure(clk_sys,
                  CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                  CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                  true);

  // CLK USB = PLL USB (48MHz) / 1 = 48MHz
  simple_clock_configure(clk_usb,
                  0, // No GLMUX
                  CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                  false);
}

void board_init(void)
{
  usb_clock_init();
}

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
