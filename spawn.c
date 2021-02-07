/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rp2040.h>

static inline bool multicore_fifo_rvalid() {
    return !!(sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS);
}

static inline void multicore_fifo_drain() {
    while (multicore_fifo_rvalid())
        (void) sio_hw->fifo_rd;
}

static inline bool multicore_fifo_wready() {
    return !!(sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS);
}

static inline void multicore_fifo_push_blocking(uint32_t data) {
    // We wait for the fifo to have some space
    while (!multicore_fifo_wready())
        tight_loop_contents();

    sio_hw->fifo_wr = data;

    // Fire off an event to the other core
    __SEV();
}

static inline uint32_t multicore_fifo_pop_blocking(void) {
    // If nothing there yet, we wait for an event first,
    // to try and avoid too much busy waiting
    while (!multicore_fifo_rvalid())
        __WFE();

    return sio_hw->fifo_rd;
}

__attribute__ (( section(".bootc") )) void multicore_launch_core1_raw(void (*entry)(void), uint32_t *sp, uint32_t vector_table) {
    uint32_t cmd_sequence[] = {0, 0, 1, (uintptr_t) vector_table, (uintptr_t) sp, (uintptr_t) entry};

    uint seq = 0;
    do {
        uint cmd = cmd_sequence[seq];
        // we drain before sending a 0
        if (!cmd) {
            multicore_fifo_drain();
            __SEV(); // core 1 may be waiting for fifo space
        }
        multicore_fifo_push_blocking(cmd);
        uint32_t response = multicore_fifo_pop_blocking();
        // move to next state on correct response otherwise start over
        seq = cmd == response ? seq + 1 : 0;
    } while (seq < (sizeof(cmd_sequence) / sizeof(*cmd_sequence)));
}
