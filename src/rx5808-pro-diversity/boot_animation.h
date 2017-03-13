/*
 * Bitmap images for boot animation

The MIT License (MIT)

Copyright (c) 2016 rx5808-pro-diversity

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <avr/pgmspace.h>
#define BOOT_ANIMATION_FRAME_SIZE 128 * 64 / 8

// You can use this program to create the bitmap array:
//    http://en.radzio.dxp.pl/bitmap_converter/
//
// Use the following settings:
//    Byte Orientation: Horizontal
//    Endianness: Little
//    Pixels/byte: 8
//
// Insert and remove frames as you wish and update BOOT_ANIMATION_FRAMES in
// settings.h to match the number you've got.
//
// NOTE: This takes up quite a lot of program memory space so you may need to
//       limit the number of frames you have, or disable some other features.

PROGMEM const unsigned char BOOT_ANIMATION_BITMAPS [BOOT_ANIMATION_FRAMES][BOOT_ANIMATION_FRAME_SIZE] = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xF8, 0x00, 0x00,
        0x0F, 0xFE, 0x03, 0xFF, 0x00, 0x07, 0xFC, 0x03, 0xFF, 0x80, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00,
        0x0F, 0xFF, 0x83, 0xFF, 0xC0, 0x1F, 0xFF, 0x03, 0xFF, 0xE0, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF0, 0x3F, 0xFF, 0x83, 0xFF, 0xF0, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF0, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xE7, 0xF3, 0xF3, 0xF8, 0xFF, 0xFF, 0xE3, 0xF3, 0xF8, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF8, 0xFE, 0x0F, 0xE3, 0xF1, 0xFC, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xC3, 0xF3, 0xF0, 0xF9, 0xFC, 0x07, 0xF3, 0xF1, 0xFC, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x00, 0x01, 0xFF, 0xF8, 0x00, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF1, 0xFC, 0x07, 0xF3, 0xFF, 0xF0, 0x0C, 0x01, 0xFF, 0xFC, 0x03, 0x80,
        0x0F, 0xFF, 0x83, 0xFF, 0xE0, 0xFE, 0x0F, 0xF3, 0xFF, 0xE0, 0x7F, 0xC3, 0xFF, 0xFC, 0x3F, 0xE0,
        0x0F, 0xFE, 0x03, 0xFF, 0xE0, 0xFF, 0x1F, 0xE3, 0xFF, 0x80, 0x7F, 0xE3, 0xFF, 0xFE, 0x7F, 0xF0,
        0x0F, 0xE0, 0x03, 0xFF, 0xF0, 0xFF, 0xFF, 0xE3, 0xF8, 0x00, 0x7F, 0xE7, 0xF0, 0xFE, 0x7F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF7, 0xF0, 0x7F, 0xFF, 0xC3, 0xF0, 0x00, 0x7F, 0xEF, 0xE0, 0x3F, 0x3F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x3F, 0xFF, 0x83, 0xF0, 0x00, 0x7F, 0xCF, 0xC0, 0x1F, 0x3F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x1F, 0xFF, 0x03, 0xF0, 0x00, 0x7F, 0x9F, 0x80, 0x1F, 0x9F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF1, 0xFC, 0x0F, 0xFE, 0x03, 0xF0, 0x00, 0x7F, 0x9F, 0x00, 0x0F, 0x9F, 0xF0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x00, 0x0F, 0xCF, 0xF0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x00, 0x0F, 0xCF, 0xF0,
        0x00, 0x07, 0xC0, 0xFC, 0x7C, 0x1E, 0x0F, 0xFF, 0x07, 0xF0, 0x7F, 0x3F, 0x00, 0x0F, 0xCF, 0xF0,
        0x00, 0x07, 0xE0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x1F, 0xF0, 0x7F, 0x9F, 0x80, 0x0F, 0x9F, 0xF0,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x7F, 0xCF, 0x80, 0x1F, 0x9F, 0xF0,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x7F, 0xCF, 0xC0, 0x3F, 0x3F, 0xF0,
        0x00, 0x07, 0xF8, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x7F, 0xE7, 0xE0, 0x7F, 0x3F, 0xF0,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x7F, 0xE7, 0xF9, 0xFE, 0x7F, 0xF0,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x7F, 0xE3, 0xFF, 0xFE, 0x7F, 0xF0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x7F, 0xC3, 0xFF, 0xFC, 0x1F, 0xE0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0xE0, 0x00, 0x01, 0xFF, 0xFC, 0x00, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x1F, 0xF0, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x0F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x07, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x03, 0xFC, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00,
        0x00, 0x07, 0xEF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE3, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x7F, 0xFF, 0x03, 0xF8, 0x3F, 0xFC, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x3F, 0xFF, 0x03, 0xF8, 0x3F, 0xF8, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0xFC, 0x1F, 0xFE, 0x03, 0xF8, 0x3F, 0xF0, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0x7C, 0x0F, 0xF8, 0x03, 0xF0, 0x3F, 0xE0, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00,
        0x0F, 0xFE, 0x03, 0xFF, 0x00, 0x07, 0xFC, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x07, 0xF8, 0x00, 0x00,
        0x0F, 0xFF, 0x83, 0xFF, 0xC0, 0x1F, 0xFF, 0x03, 0xFF, 0xE0, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF0, 0x3F, 0xFF, 0x83, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xC0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF0, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x00, 0x0F, 0xFF, 0xF8, 0x00,
        0x0F, 0xE7, 0xF3, 0xF3, 0xF8, 0xFF, 0xFF, 0xE3, 0xF3, 0xF8, 0x00, 0x00, 0x0F, 0xFF, 0xFC, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF8, 0xFE, 0x0F, 0xE3, 0xF1, 0xFC, 0x03, 0xC0, 0x0F, 0xFF, 0xFC, 0x00,
        0x0F, 0xC3, 0xF3, 0xF0, 0xF9, 0xFC, 0x07, 0xF3, 0xF1, 0xFC, 0x07, 0xE0, 0x03, 0xFF, 0xF8, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x0F, 0xF8, 0x00, 0xFF, 0xF8, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x0F, 0xFC, 0x00, 0x1F, 0xF0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x0F, 0xFC, 0x3F, 0x07, 0xF0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x1F, 0xFC, 0x7F, 0xC1, 0xE0, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF1, 0xFC, 0x07, 0xF3, 0xFF, 0xF0, 0x1F, 0xF8, 0xFF, 0xF0, 0x40, 0x00,
        0x0F, 0xFF, 0x83, 0xFF, 0xE0, 0xFE, 0x0F, 0xF3, 0xFF, 0xE0, 0x3F, 0xF1, 0xFF, 0xFE, 0x00, 0x00,
        0x0F, 0xFE, 0x03, 0xFF, 0xE0, 0xFF, 0x1F, 0xE3, 0xFF, 0x80, 0x3F, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
        0x0F, 0xE0, 0x03, 0xFF, 0xF0, 0xFF, 0xFF, 0xE3, 0xF8, 0x00, 0x3F, 0x8F, 0xF8, 0x7F, 0x00, 0x00,
        0x0F, 0xC0, 0x03, 0xF7, 0xF0, 0x7F, 0xFF, 0xC3, 0xF0, 0x00, 0x7F, 0x9F, 0xC0, 0x3F, 0x00, 0x00,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x3F, 0xFF, 0x83, 0xF0, 0x00, 0x7F, 0x9F, 0x80, 0x1F, 0x00, 0x00,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x1F, 0xFF, 0x03, 0xF0, 0x00, 0xFF, 0x9F, 0x80, 0x1F, 0x80, 0x00,
        0x0F, 0xC0, 0x03, 0xF1, 0xFC, 0x0F, 0xFE, 0x03, 0xF0, 0x00, 0xFF, 0x9F, 0x00, 0x0F, 0x9F, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xFF, 0x9F, 0x00, 0x0F, 0x9F, 0xE0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xDF, 0x00, 0x0F, 0x9F, 0xF0,
        0x00, 0x07, 0xC0, 0xFC, 0x7C, 0x1E, 0x0F, 0xFF, 0x07, 0xF0, 0x7F, 0x9F, 0x00, 0x0F, 0x9F, 0xF0,
        0x00, 0x07, 0xE0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x1F, 0xF0, 0x0F, 0x9F, 0x00, 0x0F, 0x9F, 0xF0,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x00, 0x1F, 0x80, 0x1F, 0x9F, 0xF0,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x00, 0x0F, 0x80, 0x1F, 0x9F, 0xF0,
        0x00, 0x07, 0xF8, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x00, 0x0F, 0xE0, 0x7F, 0x1F, 0xE0,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x00, 0x0F, 0xF9, 0xFF, 0x1F, 0xE0,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x00, 0x0F, 0xFF, 0xFC, 0x7F, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x00, 0x03, 0xFF, 0xF8, 0xFF, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0xE0, 0x00, 0x20, 0xFF, 0xF1, 0xFF, 0x80,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x1F, 0xF0, 0x00, 0x78, 0x1F, 0xC3, 0xFF, 0x80,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x0F, 0xF8, 0x00, 0xFE, 0x07, 0x83, 0xFF, 0x80,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x07, 0xF8, 0x01, 0xFF, 0xC0, 0x03, 0xFF, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x03, 0xFC, 0x01, 0xFF, 0xF0, 0x00, 0xFF, 0x00,
        0x00, 0x07, 0xEF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x01, 0xFF, 0xFE, 0x00, 0x7E, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x03, 0xFF, 0xFF, 0x00, 0x1C, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xE3, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x7F, 0xFF, 0x03, 0xF8, 0x3F, 0xFC, 0x00, 0x3F, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x3F, 0xFF, 0x03, 0xF8, 0x3F, 0xF8, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0xFC, 0x1F, 0xFE, 0x03, 0xF8, 0x3F, 0xF0, 0x00, 0x01, 0xFE, 0x00, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0x7C, 0x0F, 0xF8, 0x03, 0xF0, 0x3F, 0xE0, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0xFE, 0x03, 0xFF, 0x00, 0x07, 0xFC, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0x83, 0xFF, 0xC0, 0x1F, 0xFF, 0x03, 0xFF, 0xE0, 0x00, 0x0E, 0x00, 0x03, 0x80, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF0, 0x3F, 0xFF, 0x83, 0xFF, 0xF0, 0x00, 0x1F, 0x00, 0x0F, 0xC0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF0, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x3F, 0x80, 0x1F, 0xE0, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x7F, 0x80, 0x1F, 0xF0, 0x00,
        0x0F, 0xE7, 0xF3, 0xF3, 0xF8, 0xFF, 0xFF, 0xE3, 0xF3, 0xF8, 0x00, 0xFF, 0xC0, 0x3F, 0xF8, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF8, 0xFE, 0x0F, 0xE3, 0xF1, 0xFC, 0x01, 0xFF, 0xE0, 0x3F, 0xFC, 0x00,
        0x0F, 0xC3, 0xF3, 0xF0, 0xF9, 0xFC, 0x07, 0xF3, 0xF1, 0xFC, 0x03, 0xFF, 0xE0, 0x3F, 0xFE, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x07, 0xFF, 0xE0, 0x1F, 0xFF, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x0F, 0xFF, 0x00, 0x0F, 0xFF, 0x80,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x1F, 0xF8, 0x03, 0xC7, 0xFF, 0xC0,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x3F, 0xF0, 0x3F, 0xE3, 0xFF, 0xE0,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF1, 0xFC, 0x07, 0xF3, 0xFF, 0xF0, 0x3F, 0xE3, 0xFF, 0xF1, 0xFF, 0xC0,
        0x0F, 0xFF, 0x83, 0xFF, 0xE0, 0xFE, 0x0F, 0xF3, 0xFF, 0xE0, 0x3F, 0xE7, 0xFF, 0xF8, 0xFF, 0xC0,
        0x0F, 0xFE, 0x03, 0xFF, 0xE0, 0xFF, 0x1F, 0xE3, 0xFF, 0x80, 0x1F, 0xE7, 0xFF, 0xFC, 0x7F, 0x80,
        0x0F, 0xE0, 0x03, 0xFF, 0xF0, 0xFF, 0xFF, 0xE3, 0xF8, 0x00, 0x1F, 0xC7, 0xE0, 0xFE, 0x3F, 0x00,
        0x0F, 0xC0, 0x03, 0xF7, 0xF0, 0x7F, 0xFF, 0xC3, 0xF0, 0x00, 0x07, 0xCF, 0xC0, 0x3F, 0x1E, 0x00,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x3F, 0xFF, 0x83, 0xF0, 0x00, 0x03, 0xCF, 0x80, 0x1F, 0x88, 0x00,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x1F, 0xFF, 0x03, 0xF0, 0x00, 0x00, 0x0F, 0x80, 0x1F, 0xC0, 0x00,
        0x0F, 0xC0, 0x03, 0xF1, 0xFC, 0x0F, 0xFE, 0x03, 0xF0, 0x00, 0x00, 0x1F, 0x00, 0x0F, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x0F, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x0F, 0x80, 0x00,
        0x00, 0x07, 0xC0, 0xFC, 0x7C, 0x1E, 0x0F, 0xFF, 0x07, 0xF0, 0x00, 0x1F, 0x00, 0x0F, 0x80, 0x00,
        0x00, 0x07, 0xE0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x1F, 0xF0, 0x00, 0x3F, 0x00, 0x0F, 0x00, 0x00,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x00, 0x3F, 0x80, 0x1F, 0x08, 0x00,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x01, 0x1F, 0x80, 0x1F, 0x3C, 0x00,
        0x00, 0x07, 0xF8, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x0F, 0x8F, 0xE0, 0x7F, 0x7F, 0x00,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x1F, 0xC7, 0xFB, 0xFE, 0x7F, 0x80,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x3F, 0xE3, 0xFF, 0xFE, 0x7F, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x3F, 0xF1, 0xFF, 0xFC, 0x7F, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0xE0, 0x7F, 0xF8, 0xFF, 0xE0, 0xFF, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x1F, 0xF0, 0x3F, 0xFC, 0x7F, 0x00, 0xFF, 0xC0,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x0F, 0xF8, 0x1F, 0xFE, 0x1C, 0x03, 0xFF, 0x80,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x07, 0xF8, 0x0F, 0xFF, 0x00, 0x3F, 0xFF, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x03, 0xFC, 0x07, 0xFF, 0x80, 0x7F, 0xFE, 0x00,
        0x00, 0x07, 0xEF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x03, 0xFF, 0xC0, 0x7F, 0xFC, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x01, 0xFF, 0xC0, 0x7F, 0xF8, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0xFF, 0xC0, 0x3F, 0xF0, 0x00,
        0x00, 0x07, 0xE3, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0x7F, 0x80, 0x1F, 0xE0, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x7F, 0xFF, 0x03, 0xF8, 0x3F, 0xFC, 0x00, 0x3F, 0x00, 0x1F, 0xC0, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x3F, 0xFF, 0x03, 0xF8, 0x3F, 0xF8, 0x00, 0x1E, 0x00, 0x07, 0x80, 0x00,
        0x00, 0x07, 0xE0, 0xFC, 0x1F, 0xFE, 0x03, 0xF8, 0x3F, 0xF0, 0x00, 0x0C, 0x00, 0x02, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0x7C, 0x0F, 0xF8, 0x03, 0xF0, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00,
        0x0F, 0xFE, 0x03, 0xFF, 0x00, 0x07, 0xFC, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0x83, 0xFF, 0xC0, 0x1F, 0xFF, 0x03, 0xFF, 0xE0, 0x00, 0x03, 0xFE, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF0, 0x3F, 0xFF, 0x83, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF0, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x00,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFF, 0xC3, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x0F, 0xE7, 0xF3, 0xF3, 0xF8, 0xFF, 0xFF, 0xE3, 0xF3, 0xF8, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF8, 0xFE, 0x0F, 0xE3, 0xF1, 0xFC, 0x03, 0xFF, 0xFF, 0x00, 0x3E, 0x00,
        0x0F, 0xC3, 0xF3, 0xF0, 0xF9, 0xFC, 0x07, 0xF3, 0xF1, 0xFC, 0x03, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x01, 0xFF, 0x06, 0x01, 0xFF, 0x00,
        0x0F, 0xC7, 0xF3, 0xF1, 0xF9, 0xF8, 0x03, 0xF3, 0xF1, 0xFC, 0x01, 0xFE, 0x00, 0x03, 0xFF, 0x80,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x00, 0xFC, 0x7A, 0x01, 0xFF, 0x80,
        0x0F, 0xFF, 0xE3, 0xFF, 0xF9, 0xF8, 0x03, 0xF3, 0xFF, 0xF8, 0x00, 0x78, 0xFF, 0xF9, 0xFF, 0xC0,
        0x0F, 0xFF, 0xC3, 0xFF, 0xF1, 0xFC, 0x07, 0xF3, 0xFF, 0xF0, 0x00, 0x79, 0xFF, 0xF8, 0xFF, 0xC0,
        0x0F, 0xFF, 0x83, 0xFF, 0xE0, 0xFE, 0x0F, 0xF3, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xFC, 0xFF, 0xC0,
        0x0F, 0xFE, 0x03, 0xFF, 0xE0, 0xFF, 0x1F, 0xE3, 0xFF, 0x80, 0x00, 0x07, 0xFF, 0xFC, 0x7F, 0xE0,
        0x0F, 0xE0, 0x03, 0xFF, 0xF0, 0xFF, 0xFF, 0xE3, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0xFE, 0x7F, 0xE0,
        0x0F, 0xC0, 0x03, 0xF7, 0xF0, 0x7F, 0xFF, 0xC3, 0xF0, 0x00, 0x00, 0x0F, 0xC0, 0x3E, 0x3F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x3F, 0xFF, 0x83, 0xF0, 0x00, 0x00, 0x1F, 0x80, 0x1F, 0x3F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF3, 0xF8, 0x1F, 0xFF, 0x03, 0xF0, 0x00, 0x00, 0x3F, 0x00, 0x0F, 0x3F, 0xF0,
        0x0F, 0xC0, 0x03, 0xF1, 0xFC, 0x0F, 0xFE, 0x03, 0xF0, 0x00, 0x1E, 0x3F, 0x00, 0x0F, 0x9F, 0xF8,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xFF, 0x3F, 0x00, 0x0F, 0x9F, 0xF0,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x3F, 0x00, 0x0F, 0x8F, 0xF0,
        0x00, 0x07, 0xC0, 0xFC, 0x7C, 0x1E, 0x0F, 0xFF, 0x07, 0xF1, 0xFF, 0x1F, 0x00, 0x0F, 0xCF, 0xC0,
        0x00, 0x07, 0xE0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x1F, 0xF0, 0xFF, 0x9F, 0x00, 0x1F, 0x86, 0x00,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0xFF, 0x8F, 0x80, 0x1F, 0x80, 0x00,
        0x00, 0x07, 0xF0, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x7F, 0xCF, 0x80, 0x3F, 0x00, 0x00,
        0x00, 0x07, 0xF8, 0xFC, 0x7E, 0x1F, 0x9F, 0xFF, 0x3F, 0xF0, 0x7F, 0xCF, 0xE0, 0x7E, 0x00, 0x00,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x7F, 0xC7, 0xFF, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xFC, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x3F, 0xE7, 0xFF, 0xFC, 0x00, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0x80, 0x3F, 0xE3, 0xFF, 0xF9, 0xC0, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x3F, 0xE0, 0x1F, 0xF3, 0xFF, 0xF3, 0xE0, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x1F, 0xF0, 0x1F, 0xF0, 0xBF, 0xE7, 0xE0, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x0F, 0xF8, 0x1F, 0xF8, 0x00, 0x07, 0xF0, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x07, 0xF8, 0x0F, 0xF8, 0x00, 0x0F, 0xF8, 0x00,
        0x00, 0x07, 0xFF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x03, 0xFC, 0x0F, 0xF0, 0x1F, 0xFF, 0xF8, 0x00,
        0x00, 0x07, 0xEF, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x07, 0xC0, 0x1F, 0xFF, 0xF8, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7E, 0x1F, 0x83, 0xF8, 0x01, 0xFC, 0x07, 0x00, 0x1F, 0xFF, 0xF8, 0x00,
        0x00, 0x07, 0xE7, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0x00, 0x1F, 0xFF, 0xF8, 0x00,
        0x00, 0x07, 0xE3, 0xFC, 0x7F, 0xFF, 0x83, 0xF8, 0x3F, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x7F, 0xFF, 0x03, 0xF8, 0x3F, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xC0, 0x00,
        0x00, 0x07, 0xE1, 0xFC, 0x3F, 0xFF, 0x03, 0xF8, 0x3F, 0xF8, 0x00, 0x00, 0x0F, 0xFE, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0xFC, 0x1F, 0xFE, 0x03, 0xF8, 0x3F, 0xF0, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00,
        0x00, 0x07, 0xE0, 0x7C, 0x0F, 0xF8, 0x03, 0xF0, 0x3F, 0xE0, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
};