/**
 * @file bit_bang.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-03-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BIT_BANG_H
#define BIT_BANG_H

void pack_framebuffer(const uint8_t *fb_unpacked, uint8_t *fb_packed);
void unpack_framebuffer(const uint8_t *fb_packed, uint8_t *fb_unpacked);
void write_frame(uint8_t *framebuffer, gpio_num_t pin);

#endif // BIT_BANG_H
