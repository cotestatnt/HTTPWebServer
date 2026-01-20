/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include "random.h"

extern unsigned long micros();
#define GETENTROPY_MAX_LEN 256


int get_entropy(uint32_t* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        uint32_t entropy = 0;

        // Collect 32 bits of entropy by reading the least significant bit (LSB) of micros()
        for (int j = 0; j < 32; j++) {
            entropy <<= 1;                       // Shift bits to the left to make space for the new bit
            entropy |= micros() & 0x1;          // Use only the LSB from the value returned by micros()
        }

        buffer[i] = entropy; // Store the generated 32-bit random value in the buffer
    }
    return 0; // Indicate success
}


uint32_t get_random(void)
{
    uint32_t random_number;
    int result = get_entropy(&random_number, sizeof(random_number));
    assert(result == 0);
    (void)result;
    return random_number;
}

void fill_random(void *buf, size_t len)
{
    assert(buf != NULL);
    int result;

    // Note that we can't use getentropy() with len > 256 directly (see getentropy man page),
    // hence reading in chunks
    const size_t FULL_CHUNKS_NUM = (len / GETENTROPY_MAX_LEN);
    const size_t REST_CHUNK_SIZE = len % GETENTROPY_MAX_LEN;

    for (size_t chunk_num = 0; chunk_num < FULL_CHUNKS_NUM; chunk_num++) {
        result = get_entropy(buf + chunk_num * GETENTROPY_MAX_LEN, GETENTROPY_MAX_LEN);
        assert(result == 0);
        (void)result;
    }

    result = get_entropy(buf + FULL_CHUNKS_NUM * GETENTROPY_MAX_LEN, REST_CHUNK_SIZE);
    assert(result == 0);
    (void)result;
}