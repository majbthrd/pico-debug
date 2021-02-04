/* this function was salvaged from pico-bootrom/bootrom/bootrom_main.c */

/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rp2040.h>

// USB bootloader requires clk_sys and clk_usb at 48 MHz. For this to work,
// xosc must be running at 12 MHz. It is possible that:
//
// - No crystal is present (and XI may not be properly grounded)
// - xosc output is much greater than 12 MHz
//
// In this case we *must* leave clk_sys in a safe state, and ideally, never
// return from this function. This is because boards which are not designed to
// use USB will still enter the USB bootcode when booted with a blank flash.

void _usb_clock_setup(void) {
    // First make absolutely sure clk_ref is running: needed for resuscitate,
    // and to run clk_sys while configuring sys PLL. Assume that rosc is not
    // configured to run faster than clk_sys max (as this is officially out of
    // spec)
    // If user previously configured clk_ref to a different source (e.g.
    // GPINx), then halted that source, the glitchless mux can't switch away
    // from the dead source-- nothing we can do about this here.
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB;
    hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);

    // Resuscitate logic will switch clk_sys to clk_ref if it is inadvertently stopped
    clocks_hw->resus.ctrl =
            CLOCKS_CLK_SYS_RESUS_CTRL_ENABLE_BITS |
            (CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT_RESET
                    << CLOCKS_CLK_SYS_RESUS_CTRL_TIMEOUT_LSB);

    // Resetting PLL regs or changing XOSC range can glitch output, so switch
    // clk_sys away before touching. Not worried about clk_usb as USB is held
    // in reset.
    hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
    while (!(clocks_hw->clk[clk_sys].selected & 1u));
    // rosc can not (while following spec) run faster than clk_sys max, so
    // it's safe now to clear dividers in clkslices.
    clocks_hw->clk[clk_sys].div = 0x100; // int 1 frac 0
    clocks_hw->clk[clk_usb].div = 0x100;

    // Try to get the crystal running. If no crystal is present, XI should be
    // grounded, so STABLE counter will never complete. Poor designs might
    // leave XI floating, in which case we may eventually drop through... in
    // this case we rely on PLL not locking, and/or resuscitate counter.
    //
    // Don't touch range setting: user would only have changed if crystal
    // needs it, and running crystal out of range can produce glitchy output.
    // Note writing a "bad" value (non-aax) to RANGE has no effect.
    xosc_hw->ctrl = XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB;
    while (!(xosc_hw->status & XOSC_STATUS_STABLE_BITS));

    // Sys PLL setup:
    // - VCO freq 1200 MHz, so feedback divisor of 100. Range is 400 MHz to 1.6 GHz
    // - Postdiv1 of 5, down to 240 MHz (appnote recommends postdiv1 >= postdiv2)
    // - Postdiv2 of 5, down to 48 MHz
    //
    // Total postdiv of 25 means that too-fast xtal will push VCO out of
    // lockable range *before* clk_sys goes out of closure (factor of 1.88)
    hw_set_bits(&resets_hw->reset, RESETS_RESET_PLL_SYS_BITS);
    hw_clear_bits(&resets_hw->reset, RESETS_RESET_PLL_SYS_BITS);
    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PLL_SYS_BITS));
    pll_sys_hw->cs = 1u << PLL_CS_REFDIV_LSB;
    pll_sys_hw->fbdiv_int = 100;
    pll_sys_hw->prim =
            (5u << PLL_PRIM_POSTDIV1_LSB) |
            (5u << PLL_PRIM_POSTDIV2_LSB);

    // Power up VCO, wait for lock
    hw_clear_bits(&pll_sys_hw->pwr, PLL_PWR_PD_BITS | PLL_PWR_VCOPD_BITS);
    while (!(pll_sys_hw->cs & PLL_CS_LOCK_BITS));

    // Power up post-dividers, which ungates PLL final output
    hw_clear_bits(&pll_sys_hw->pwr, PLL_PWR_POSTDIVPD_BITS);

    // Glitchy switch of clk_usb, clk_sys aux to sys PLL output.
    clocks_hw->clk[clk_sys].ctrl = 0;
    clocks_hw->clk[clk_usb].ctrl =
            CLOCKS_CLK_USB_CTRL_ENABLE_BITS |
            (CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS
                    << CLOCKS_CLK_USB_CTRL_AUXSRC_LSB);

    // Glitchless switch of clk_sys to aux source (sys PLL)
    hw_set_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
    while (!(clocks_hw->clk[clk_sys].selected & 0x2u));
}
