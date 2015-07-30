/*
Copyright (c) 2010 Myles Metzer

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
/*
This library provides a simple method for outputting data to a tv
from a frame buffer stored in sram.  This implementation is done
completly by interupt and will return give as much cpu time to the
application as possible.
*/
#ifndef TVOUT_H
#define TVOUT_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "video_gen.h"
#include "spec/hardware_setup.h"
#include "spec/video_properties.h"

// macros for readability when selecting mode.
#define PAL						1
#define	NTSC					0
#define _PAL					1
#define _NTSC					0

#define WHITE					1
#define BLACK					0
#define INVERT					2

#define UP						0
#define DOWN					1
#define LEFT					2
#define RIGHT					3

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

// Macros for clearer usage
#define clear_screen()				fill(0)
#define invert(color)				fill(2)

/*
TVout.cpp contains a brief expenation of each function.
*/
class TVout {
public:
	uint8_t * screen;
	
	char begin(uint8_t mode);
	char begin(uint8_t mode, uint8_t x, uint8_t y);
	void end();
    void genlock();
    void video_clock(uint8_t mode);
    
	//accessor functions
	unsigned char hres();
	unsigned char vres();
	char char_line();
	
	//flow control functions
	void delay(unsigned int x);
	void delay_frame(unsigned int x);
	unsigned long millis();
	
	//override setup functions
	void force_vscale(char sfactor);
	void force_outstart(uint8_t time);
	void force_linestart(uint8_t line);
	
	//basic rendering functions
	void set_pixel(uint8_t x, uint8_t y, char c);
	unsigned char get_pixel(uint8_t x, uint8_t y);
	void fill(uint8_t color);
	void shift(uint8_t distance, uint8_t direction);
	void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c);
	void draw_row(uint8_t line, uint16_t x0, uint16_t x1, uint8_t c);
	void draw_column(uint8_t row, uint16_t y0, uint16_t y1, uint8_t c);
	void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1); 
	void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1);
	void bitmap(uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0);
	
	//hook setup functions
	void set_vbi_hook(void (*func)());
	void set_hbi_hook(void (*func)());

	//tone functions
	void tone(unsigned int frequency, unsigned long duration_ms);
	void tone(unsigned int frequency);
	void noTone();
	
//The following function definitions can be found in TVoutPrint.cpp
//printing functions
	void print_char(uint8_t x, uint8_t y, unsigned char c);
	void set_cursor(uint8_t, uint8_t);
	void select_font(const unsigned char * f);

    void write(uint8_t);
    void write(const char *str);
    void write(const uint8_t *buffer, uint8_t size);

    void print(const char[]);
    void print(char, int = BYTE);
    void print(unsigned char, int = BYTE);
    void print(int, int = DEC);
    void print(unsigned int, int = DEC);
    void print(long, int = DEC);
    void print(unsigned long, int = DEC);
    void print(double, int = 2);
	
	void print(uint8_t, uint8_t, const char[]);
	void print(uint8_t, uint8_t, char, int = BYTE);
	void print(uint8_t, uint8_t, unsigned char, int = BYTE);
	void print(uint8_t, uint8_t, int, int = DEC);
	void print(uint8_t, uint8_t, unsigned int, int = DEC);
	void print(uint8_t, uint8_t, long, int = DEC);
	void print(uint8_t, uint8_t, unsigned long, int = DEC);
	void print(uint8_t, uint8_t, double, int = 2);
	
	void println(uint8_t, uint8_t, const char[]);
    void println(uint8_t, uint8_t, char, int = BYTE);
    void println(uint8_t, uint8_t, unsigned char, int = BYTE);
    void println(uint8_t, uint8_t, int, int = DEC);
    void println(uint8_t, uint8_t, unsigned int, int = DEC);
    void println(uint8_t, uint8_t, long, int = DEC);
    void println(uint8_t, uint8_t, unsigned long, int = DEC);
    void println(uint8_t, uint8_t, double, int = 2);
    void println(uint8_t, uint8_t);

    void println(const char[]);
    void println(char, int = BYTE);
    void println(unsigned char, int = BYTE);
    void println(int, int = DEC);
    void println(unsigned int, int = DEC);
    void println(long, int = DEC);
    void println(unsigned long, int = DEC);
    void println(double, int = 2);
    void println(void);
	
	void printPGM(const char[]);
	void printPGM(uint8_t, uint8_t, const char[]);
	
private:
	uint8_t cursor_x,cursor_y;
	const unsigned char * font;
	
	void inc_txtline();
    void printNumber(unsigned long, uint8_t);
    void printFloat(double, uint8_t);
};

static void inline sp(unsigned char x, unsigned char y, char c); 
#endif
