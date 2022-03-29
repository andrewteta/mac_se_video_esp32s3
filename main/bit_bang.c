/**
 * @file bit_bang.c
 * @author A. Teta
 * @brief Utility module to write a 1-bit video stream to a GPIO pin
 * @version 0.1
 * @date 2022-03-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "driver/gpio.h"

#define H_RES (512)
#define V_RES (384)

#define N_PIXELS (H_RES * V_RES)

// static uint8_t framebuffer[N_PIXELS];
// static uint8_t framebuffer_packed[N_PIXELS / 8];

void pack_framebuffer(const uint8_t *fb_unpacked, uint8_t *fb_packed)
{
    for (int i = 0; i < N_PIXELS; i += 8)
    {
        fb_packed[i / 8] =
            (fb_unpacked[i] & 0x01) | ((fb_unpacked[i + 1] << 1) & 0x02) | ((fb_unpacked[i + 2] << 2) & 0x04) | ((fb_unpacked[i + 3] << 3) & 0x08) | ((fb_unpacked[i + 4] << 4) & 0x10) | ((fb_unpacked[i + 5] << 5) & 0x20) | ((fb_unpacked[i + 6] << 6) & 0x40) | ((fb_unpacked[i + 7] << 7) & 0x80);
    }
}

void unpack_framebuffer(const uint8_t *fb_packed, uint8_t *fb_unpacked)
{
    for (int i = 0; i < N_PIXELS / 8; i++)
    {
        fb_unpacked[i * 8] = fb_packed[i] & 0x01;
        fb_unpacked[(i * 8) + 1] = (fb_packed[i] >> 1) & 0x01;
        fb_unpacked[(i * 8) + 2] = (fb_packed[i] >> 2) & 0x01;
        fb_unpacked[(i * 8) + 3] = (fb_packed[i] >> 3) & 0x01;
        fb_unpacked[(i * 8) + 4] = (fb_packed[i] >> 4) & 0x01;
        fb_unpacked[(i * 8) + 5] = (fb_packed[i] >> 5) & 0x01;
        fb_unpacked[(i * 8) + 6] = (fb_packed[i] >> 6) & 0x01;
        fb_unpacked[(i * 8) + 7] = (fb_packed[i] >> 7) & 0x01;
    }
}

/**
 * @brief Writes values from framebuffer_unpacked to GPIO pin
 *
 * @param line line index to write (row)
 * @param pin GPIO pin number (e.g. GPIO_NUM_n)
 */
static inline void _write_line(uint8_t *framebuffer, int line, gpio_num_t pin)
{
    int start_index = line * H_RES;

    for (int i = start_index; i < H_RES + start_index; i++)
    {
        if (framebuffer[i])
            gpio_set_level(pin, 1);
        else
            gpio_set_level(pin, 0);

        // delay by spinning
        for (int d = 0; d < 1000; d++)
            ;
    }
}

/**
 * @brief Write frame to GPIO pin
 *
 * @param pin
 */
void write_frame(uint8_t *framebuffer, gpio_num_t pin)
{
    for (int i = 0; i < V_RES; i++)
    {
        _write_line(framebuffer, i, pin);
        for (int d = 0; d < 10000; d++)
            ;
    }
}
