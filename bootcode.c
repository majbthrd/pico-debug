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
#include "hardware/resets.h"

/* this is code that runs before main() and cannot be used once execution reaches main() (since it exists in RAM we have not reserved) */

__attribute__ (( section(".bootc") )) static void pll_init(pll_hw_t *pll, uint32_t refdiv, uint32_t vco_freq, uint32_t post_div1, uint8_t post_div2)
{
    uint32_t ref_mhz = XOSC_MHZ / refdiv;

    // What are we multiplying the reference clock by to get the vco freq
    // (The regs are called div, because you divide the vco output and compare it to the refclk)
    uint32_t fbdiv = vco_freq / (ref_mhz * 1000000UL);

    // fbdiv
    assert(fbdiv >= 16 && fbdiv <= 320);

    // Check divider ranges
    assert((post_div1 >= 1 && post_div1 <= 7) && (post_div2 >= 1 && post_div2 <= 7));

    // post_div1 should be >= post_div2
    // from appnote page 11
    // postdiv1 is designed to operate with a higher input frequency
    // than postdiv2
    assert(post_div2 <= post_div1);

    // Check that reference frequency is no greater than vco / 16
    assert(ref_mhz <= (vco_freq / 16));

    // Set up post dividers - div1 feeds into div2 so if div1 is 5 and div2 is 2 then you get a divide by 10
    uint32_t pdiv = (post_div1 << PLL_PRIM_POSTDIV1_LSB) |
                    (post_div2 << PLL_PRIM_POSTDIV2_LSB);

    uint32_t pll_reset = (pll_usb_hw == pll) ? RESETS_RESET_PLL_USB_BITS : RESETS_RESET_PLL_SYS_BITS;
    reset_block(pll_reset);
    unreset_block_wait(pll_reset);
    
    // Turn off PLL in case it is already running
    pll->pwr = 0xffffffff;
    pll->fbdiv_int = 0;

    pll->cs = refdiv;

    // Put calculated value into feedback divider
    pll->fbdiv_int = fbdiv;

    // Turn on PLL
    uint32_t power = PLL_PWR_PD_BITS | // Main power
                     PLL_PWR_VCOPD_BITS; // VCO Power

    hw_clear_bits(&pll->pwr, power);

    // Wait for PLL to lock
    while (!(pll->cs & PLL_CS_LOCK_BITS));

    // Set up post dividers
    pll->prim = pdiv;

    // Turn on post divider
    hw_clear_bits(&pll->pwr, PLL_PWR_POSTDIVPD_BITS);
}

/*
This is a more streamlined alternative to the current pico-sdk based TinyUSB board support package.
Sticking to C and avoiding all that C++ yields a much smaller executable.
*/

/* overhaul of clock_configure() from pico-sdk to use much less memory */
__attribute__ (( section(".bootc") )) bool simple_clock_configure(enum clock_index clk_index, uint32_t src, uint32_t auxsrc, bool glitchless)
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

__attribute__ (( section(".bootc") )) static void usb_clock_init(void)
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

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);

#define rom_hword_as_ptr(rom_address) (void *)(uint32_t)(*(uint16_t *)rom_address)

__attribute__ (( section(".bootc") )) static void *rom_func_lookup(uint32_t code)
{
  rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)rom_hword_as_ptr(0x18);
  uint16_t *func_table = (uint16_t *)rom_hword_as_ptr(0x14);
  return rom_table_lookup(func_table, code);
}

__attribute__ (( section(".bootc") )) static void *rom_data_lookup(uint32_t code)
{
  rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)rom_hword_as_ptr(0x18);
  uint16_t *data_table = (uint16_t *)rom_hword_as_ptr(0x16);
  return rom_table_lookup(data_table, code);
}

__attribute__ (( section(".bootc") )) static uint32_t rom_table_code(char c1, char c2)
{
  return (c2 << 8) | c1;
}

typedef uint32_t (*pop_fn)(uint32_t);
typedef void (*void_fn)(void);

extern char unique_id[17];

__attribute__ (( section(".bootc") )) void SystemInit(void)
{
  void_fn connect_internal_flash = rom_func_lookup(rom_table_code('I', 'F'));
  void_fn flash_exit_xip = rom_func_lookup(rom_table_code('E', 'X'));

  connect_internal_flash();
  flash_exit_xip();

  hw_write_masked(&ioqspi_hw->io[1].ctrl,
      IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB,
      IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_BITS
  );

  /* Unique ID command and 32-bits of pipeline delay */
  for (int i = 0; i < 5; i++)
  {
    ssi_hw->dr0 = 0x4B;
    while (!(ssi_hw->sr & SSI_SR_RFNE_BITS));
    (void)ssi_hw->dr0;
  }
  /* read the 64-bits and write as hex chars */
  char *pnt = unique_id;
  for (int i = 0; i < 8; i++)
  {
    ssi_hw->dr0 = 0;
    while (!(ssi_hw->sr & SSI_SR_RFNE_BITS));
    uint8_t byte = ssi_hw->dr0;
    *pnt++ = "0123456789ABCDEF"[(byte >> 4) & 0xf];
    *pnt++ = "0123456789ABCDEF"[(byte >> 0) & 0xf];
  }

  hw_write_masked(&ioqspi_hw->io[1].ctrl,
      IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_HIGH << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB,
      IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_BITS
  );

  usb_clock_init();
}
