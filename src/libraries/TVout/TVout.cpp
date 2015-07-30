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

/* A note about how Color is defined for this version of TVout
 *
 * Where ever choosing a color is mentioned the following are true:
 * 	BLACK	=0
 *	WHITE	=1
 *	INVERT	=2
 *	All others will be ignored.
*/

#include "TVout.h"


/* Call this to start video output with the default resolution.
 *
 * Arguments:
 *	mode:
 *		The video standard to follow:
 *		PAL		=1	=_PAL
 *		NTSC	=0	=_NTSC
 *
 * Returns:
 *	0 if no error.
 *	4 if there is not enough memory.
 */
char TVout::begin(uint8_t mode) {

	return begin(mode,128,96);
} // end of begin


/* call this to start video output with a specified resolution.
 *
 * Arguments:
 *	mode:
 *		The video standard to follow:
 *		PAL		=1	=_PAL
 *		NTSC	=0	=_NTSC
 *	x:
 *		Horizonal resolution must be divisable by 8.
 *	y:
 *		Vertical resolution.
 *
 *	Returns:
 *		0 if no error.
 *		1 if x is not divisable by 8.
 *		2 if y is to large (NTSC only cannot fill PAL vertical resolution by 8bit limit)
 *		4 if there is not enough memory for the frame buffer.
 */
char TVout::begin(uint8_t mode, uint8_t x, uint8_t y) {

	// check if x is divisable by 8
	if ( !(x & 0xF8))
		return 1;
	x = x/8;

	screen = (unsigned char*)malloc(x * y * sizeof(unsigned char));
	if (screen == NULL)
		return 4;

	cursor_x = 0;
	cursor_y = 0;

	render_setup(mode,x,y,screen);
	clear_screen();
	return 0;
} // end of begin


/* Stop video render and free the used memory.
 */
 void TVout::end() {
	TIMSK1 = 0;
	free(screen);
}
/* Enable genlock
*/
void TVout::genlock() {
    display.enable_genlock=0;
}

/* Select video clock
*/
void TVout::video_clock(uint8_t mode) {
    select_clock(mode);
}



/* Fill the screen with some color.
 *
 * Arguments:
 *	color:
 *		The color to fill the screen with.
 *		(see color note at the top of this file)
*/
void TVout::fill(uint8_t color) {
	switch(color) {
		case BLACK:
			cursor_x = 0;
			cursor_y = 0;
			for (int i = 0; i < (display.hres)*display.vres; i++)
				display.screen[i] = 0;
			break;
		case WHITE:
			cursor_x = 0;
			cursor_y = 0;
			for (int i = 0; i < (display.hres)*display.vres; i++)
				display.screen[i] = 0xAA;
			break;
		case INVERT:
			for (int i = 0; i < display.hres*display.vres; i++)
				display.screen[i] = ~display.screen[i];
			break;
	}
} // end of fill


/* Gets the Horizontal resolution of the screen
 *
 * Returns:
 *	The horizonal resolution.
*/
unsigned char TVout::hres() {
	return display.hres*8;
} // end of hres


/* Gets the Vertical resolution of the screen
 *
 * Returns:
 *	The vertical resolution
*/
unsigned char TVout::vres() {
	return display.vres;
} // end of vres


/* Return the number of characters that will fit on a line
 *
 * Returns:
 *	The number of characters that will fit on a text line starting from x=0.
 *	Will return -1 for dynamic width fonts as this cannot be determined.
*/
char TVout::char_line() {
	return ((display.hres*8)/pgm_read_byte(font));
} // end of char_line


/* delay for x ms
 * The resolution is 16ms for NTSC and 20ms for PAL
 *
 * Arguments:
 *	x:
 *		The number of ms this function should consume.
*/
void TVout::delay(unsigned int x) {
	unsigned long time = millis() + x;
	while(millis() < time);
} // end of delay


/* Delay for x frames, exits at the end of the last display line.
 * delay_frame(1) is useful prior to drawing so there is little/no flicker.
 *
 * Arguments:
 *	x:
 *		The number of frames to delay for.
 */
void TVout::delay_frame(unsigned int x) {
	int stop_line = (int)(display.start_render + (display.vres*(display.vscale_const+1)))+1;
	while (x) {
		while (display.scanLine != stop_line);
		while (display.scanLine == stop_line);
		x--;
	}
} // end of delay_frame


/* Get the time in ms since begin was called.
 * The resolution is 16ms for NTSC and 20ms for PAL
 *
 * Returns:
 *	The time in ms since video generation has started.
*/
unsigned long TVout::millis() {
	if (display.lines_frame == _NTSC_LINE_FRAME) {
		return display.frames * _NTSC_TIME_SCANLINE * _NTSC_LINE_FRAME / 1000;
	}
	else {
		return display.frames * _PAL_TIME_SCANLINE * _PAL_LINE_FRAME / 1000;
	}
} // end of millis


/* force the number of times to display each line.
 *
 * Arguments:
 *	sfactor:
 *		The scale number of times to repeate each line.
 */
void TVout::force_vscale(char sfactor) {
	delay_frame(1);
	display.vscale_const = sfactor - 1;
	display.vscale = sfactor - 1;
}


/* force the output start time of a scanline in micro seconds.
 *
 * Arguments:
 *	time:
 *		The new output start time in micro seconds.
 */
void TVout::force_outstart(uint8_t time) {
	delay_frame(1);
	display.output_delay = ((time * _CYCLES_PER_US) - 1);
}


/* force the start line for active video
 *
 * Arguments:
 *	line:
 *		The new active video output start line
 */
void TVout::force_linestart(uint8_t line) {
	delay_frame(1);
	display.start_render = line;
}


/* Set the color of a pixel
 *
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *	c:
 *		The color of the pixel
 *		(see color note at the top of this file)
 */
void TVout::set_pixel(uint8_t x, uint8_t y, char c) {
	if (x >= display.hres*8 || y >= display.vres)
		return;
	sp(x,y,c);
} // end of set_pixel


/* get the color of the pixel at x,y
 *
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *
 * Returns:
 *	The color of the pixel.
 *	(see color note at the top of this file)
 *
 * Thank you gijs on the arduino.cc forum for the non obviouse fix.
*/
unsigned char TVout::get_pixel(uint8_t x, uint8_t y) {
	if (x >= display.hres*8 || y >= display.vres)
		return 0;
	if (display.screen[x/8+y*display.hres] & (0x80 >>(x&7)))
		return 1;
	return 0;
} // end of get_pixel


/* Draw a line from one point to another
 *
 * Arguments:
 *	x0:
 *		The x coordinate of point 0.
 *	y0:
 *		The y coordinate of point 0.
 *	x1:
 *		The x coordinate of point 1.
 *	y1:
 *		The y coordinate of point 1.
 *	c:
 *		The color of the line.
 *		(see color note at the top of this file)
 */
void TVout::draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c) {

	if (x0 > display.hres*8 || y0 > display.vres || x1 > display.hres*8 || y1 > display.vres)
		return;
	if (x0 == x1)
		draw_column(x0,y0,y1,c);
	else if (y0 == y1)
		draw_row(y0,x0,x1,c);
	else {
		int e;
		signed int dx,dy,j, temp;
		signed char s1,s2, xchange;
		signed int x,y;

		x = x0;
		y = y0;

		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		}

		e = ((int)dy<<1) - dx;

		for (j=0; j<=dx; j++) {
			sp(x,y,c);

			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
} // end of draw_line


/* Fill a row from one point to another
 *
 * Argument:
 *	line:
 *		The row that fill will be performed on.
 *	x0:
 *		edge 0 of the fill.
 *	x1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void TVout::draw_row(uint8_t line, uint16_t x0, uint16_t x1, uint8_t c) {
	uint8_t lbit, rbit;

	if (x0 == x1)
		set_pixel(x0,line,c);
	else {
		if (x0 > x1) {
			lbit = x0;
			x0 = x1;
			x1 = lbit;
		}
		lbit = 0xff >> (x0&7);
		x0 = x0/8 + display.hres*line;
		rbit = ~(0xff >> (x1&7));
		x1 = x1/8 + display.hres*line;
		if (x0 == x1) {
			lbit = lbit & rbit;
			rbit = 0;
		}
		if (c == WHITE) {
			screen[x0++] |= lbit;
			while (x0 < x1)
				screen[x0++] = 0xff;
			screen[x0] |= rbit;
		}
		else if (c == BLACK) {
			screen[x0++] &= ~lbit;
			while (x0 < x1)
				screen[x0++] = 0;
			screen[x0] &= ~rbit;
		}
		else if (c == INVERT) {
			screen[x0++] ^= lbit;
			while (x0 < x1)
				screen[x0++] ^= 0xff;
			screen[x0] ^= rbit;
		}
	}
} // end of draw_row


/* Fill a column from one point to another
 *
 * Argument:
 *	row:
 *		The row that fill will be performed on.
 *	y0:
 *		edge 0 of the fill.
 *	y1:
 *		edge 1 of the fill.
 *	c:
 *		the color of the fill.
 *		(see color note at the top of this file)
*/
void TVout::draw_column(uint8_t row, uint16_t y0, uint16_t y1, uint8_t c) {

	unsigned char bit;
	int byte;

	if (y0 == y1)
		set_pixel(row,y0,c);
	else {
		if (y1 < y0) {
			bit = y0;
			y0 = y1;
			y1 = bit;
		}
		bit = 0x80 >> (row&7);
		byte = row/8 + y0*display.hres;
		if (c == WHITE) {
			while ( y0 <= y1) {
				screen[byte] |= bit;
				byte += display.hres;
				y0++;
			}
		}
		else if (c == BLACK) {
			while ( y0 <= y1) {
				screen[byte] &= ~bit;
				byte += display.hres;
				y0++;
			}
		}
		else if (c == INVERT) {
			while ( y0 <= y1) {
				screen[byte] ^= bit;
				byte += display.hres;
				y0++;
			}
		}
	}
}


/* draw a rectangle at x,y with a specified width and height
 *
 * Arguments:
 *	x0:
 *		The x coordinate of upper left corner of the rectangle.
 *	y0:
 *		The y coordinate of upper left corner of the rectangle.
 *	w:
 *		The widht of the rectangle.
 *	h:
 *		The height of the rectangle.
 *	c:
 *		The color of the rectangle.
 *		(see color note at the top of this file)
 *	fc:
 *		The fill color of the rectangle.
 *		(see color note at the top of this file)
 *		default =-1 (no fill)
*/
void TVout::draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc) {

	if (fc != -1) {
		for (unsigned char i = y0; i < y0+h; i++)
			draw_row(i,x0,x0+w,fc);
	}
	draw_line(x0,y0,x0+w,y0,c);
	draw_line(x0,y0,x0,y0+h,c);
	draw_line(x0+w,y0,x0+w,y0+h,c);
	draw_line(x0,y0+h,x0+w,y0+h,c);
} // end of draw_rect


/* draw a circle given a coordinate x,y and radius both filled and non filled.
 *
 * Arguments:
 * 	x0:
 *		The x coordinate of the center of the circle.
 *	y0:
 *		The y coordinate of the center of the circle.
 *	radius:
 *		The radius of the circle.
 *	c:
 *		The color of the circle.
 *		(see color note at the top of this file)
 *	fc:
 *		The color to fill the circle.
 *		(see color note at the top of this file)
 *		defualt  =-1 (do not fill)
 */
void TVout::draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc) {

	int f = 1 - radius;
	int ddF_x = 1;
	int	ddF_y = -2 * radius;
	int x = 0;
	int y = radius;
	uint8_t pyy = y,pyx = x;


	//there is a fill color
	if (fc != -1)
		draw_row(y0,x0-radius,x0+radius,fc);

	sp(x0, y0 + radius,c);
	sp(x0, y0 - radius,c);
	sp(x0 + radius, y0,c);
	sp(x0 - radius, y0,c);

	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		//there is a fill color
		if (fc != -1) {
			//prevent double draws on the same rows
			if (pyy != y) {
				draw_row(y0+y,x0-x,x0+x,fc);
				draw_row(y0-y,x0-x,x0+x,fc);
			}
			if (pyx != x && x != y) {
				draw_row(y0+x,x0-y,x0+y,fc);
				draw_row(y0-x,x0-y,x0+y,fc);
			}
			pyy = y;
			pyx = x;
		}
		sp(x0 + x, y0 + y,c);
		sp(x0 - x, y0 + y,c);
		sp(x0 + x, y0 - y,c);
		sp(x0 - x, y0 - y,c);
		sp(x0 + y, y0 + x,c);
		sp(x0 - y, y0 + x,c);
		sp(x0 + y, y0 - x,c);
		sp(x0 - y, y0 - x,c);
	}
} // end of draw_circle


/* place a bitmap at x,y where the bitmap is defined as {width,height,imagedata....}
 *
 * Arguments:
 *	x:
 *		The x coordinate of the upper left corner.
 *	y:
 *		The y coordinate of the upper left corner.
 *	bmp:
 *		The bitmap data to print.
 *	i:
 *		The offset into the image data to start at.  This is mainly used for fonts.
 *		default	=0
 *	width:
 *		Override the bitmap width. This is mainly used for fonts.
 *		default =0 (do not override)
 *	height:
 *		Override the bitmap height. This is mainly used for fonts.
 *		default	=0 (do not override)
*/
void TVout::bitmap(uint8_t x, uint8_t y, const unsigned char * bmp,
				   uint16_t i, uint8_t width, uint8_t lines) {

	uint8_t temp, lshift, rshift, save, xtra;
	uint16_t si = 0;

	rshift = x&7;
	lshift = 8-rshift;
	if (width == 0) {
		width = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}
	if (lines == 0) {
		lines = pgm_read_byte((uint32_t)(bmp) + i);
		i++;
	}

	if (width&7) {
		xtra = width&7;
		width = width/8;
		width++;
	}
	else {
		xtra = 8;
		width = width/8;
	}

	for (uint8_t l = 0; l < lines; l++) {
		si = (y + l)*display.hres + x/8;
		if (width == 1)
			temp = 0xff >> rshift + xtra;
		else
			temp = 0;
		save = screen[si];
		screen[si] &= ((0xff << lshift) | temp);
		temp = pgm_read_byte((uint32_t)(bmp) + i++);
		screen[si++] |= temp >> rshift;
		for ( uint16_t b = i + width-1; i < b; i++) {
			save = screen[si];
			screen[si] = temp << lshift;
			temp = pgm_read_byte((uint32_t)(bmp) + i);
			screen[si++] |= temp >> rshift;
		}
		if (rshift + xtra < 8)
			screen[si-1] |= (save & (0xff >> rshift + xtra));	//test me!!!
		if (rshift + xtra - 8 > 0)
			screen[si] &= (0xff >> rshift + xtra - 8);
		screen[si] |= temp << lshift;
	}
} // end of bitmap


/* shift the pixel buffer in any direction
 * This function will shift the screen in a direction by any distance.
 *
 * Arguments:
 *	distance:
 *		The distance to shift the screen
 *	direction:
 *		The direction to shift the screen the direction and the integer values:
 *		UP		=0
 *		DOWN	=1
 *		LEFT	=2
 *		RIGHT	=3
*/
void TVout::shift(uint8_t distance, uint8_t direction) {
	uint8_t * src;
	uint8_t * dst;
	uint8_t * end;
	uint8_t shift;
	uint8_t tmp;
	switch(direction) {
		case UP:
			dst = display.screen;
			src = display.screen + distance*display.hres;
			end = display.screen + display.vres*display.hres;

			while (src <= end) {
				*dst = *src;
				*src = 0;
				dst++;
				src++;
			}
			break;
		case DOWN:
			dst = display.screen + display.vres*display.hres;
			src = dst - distance*display.hres;
			end = display.screen;

			while (src >= end) {
				*dst = *src;
				*src = 0;
				dst--;
				src--;
			}
			break;
		case LEFT:
			shift = distance & 7;

			for (uint8_t line = 0; line < display.vres; line++) {
				dst = display.screen + display.hres*line;
				src = dst + distance/8;
				end = dst + display.hres-2;
				while (src <= end) {
					tmp = 0;
					tmp = *src << shift;
					*src = 0;
					src++;
					tmp |= *src >> (8 - shift);
					*dst = tmp;
					dst++;
				}
				tmp = 0;
				tmp = *src << shift;
				*src = 0;
				*dst = tmp;
			}
			break;
		case RIGHT:
			shift = distance & 7;

			for (uint8_t line = 0; line < display.vres; line++) {
				dst = display.screen + display.hres-1 + display.hres*line;
				src = dst - distance/8;
				end = dst - display.hres+2;
				while (src >= end) {
					tmp = 0;
					tmp = *src >> shift;
					*src = 0;
					src--;
					tmp |= *src << (8 - shift);
					*dst = tmp;
					dst--;
				}
				tmp = 0;
				tmp = *src >> shift;
				*src = 0;
				*dst = tmp;
			}
			break;
	}
} // end of shift


/* Inline version of set_pixel that does not perform a bounds check
 * This function will be replaced by a macro.
*/
static void inline sp(uint8_t x, uint8_t y, char c) {
	if (c==1)
		display.screen[(x/8) + (y*display.hres)] |= 0x80 >> (x&7);
	else if (c==0)
		display.screen[(x/8) + (y*display.hres)] &= ~0x80 >> (x&7);
	else
		display.screen[(x/8) + (y*display.hres)] ^= 0x80 >> (x&7);
} // end of sp


/* set the vertical blank function call
 * The function passed to this function will be called one per frame. The function should be quickish.
 *
 * Arguments:
 *	func:
 *		The function to call.
 */
void TVout::set_vbi_hook(void (*func)()) {
	vbi_hook = func;
} // end of set_vbi_hook


/* set the horizonal blank function call
 * This function passed to this function will be called one per scan line.
 * The function MUST be VERY FAST(~2us max).
 *
 * Arguments:
 *	funct:
 *		The function to call.
 */
void TVout::set_hbi_hook(void (*func)()) {
	hbi_hook = func;
} // end of set_bhi_hook


/* Simple tone generation
 *
 * Arguments:
 *	frequency:
 *		the frequency of the tone
 * courtesy of adamwwolf
 */
void TVout::tone(unsigned int frequency) {
	tone(frequency, 0);
} // end of tone


/* Simple tone generation
 *
 * Arguments:
 *	frequency:
 *		the frequency of the tone
 *	duration_ms:
 *		The duration to play the tone in ms
 * courtesy of adamwwolf
 */
void TVout::tone(unsigned int frequency, unsigned long duration_ms) {

	if (frequency == 0)
		return;

#define TIMER 2
	//this is init code
	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A |= _BV(WGM21);
	TCCR2B |= _BV(CS20);
	//end init code

	//most of this is taken from Tone.cpp from Arduino
	uint8_t prescalarbits = 0b001;
	uint32_t ocr = 0;


    DDR_SND |= _BV(SND_PIN); //set pb3 (digital pin 11) to output

    //we are using an 8 bit timer, scan through prescalars to find the best fit
	ocr = F_CPU / frequency / 2 - 1;
    prescalarbits = 0b001;  // ck/1: same for both timers
    if (ocr > 255) {
        ocr = F_CPU / frequency / 2 / 8 - 1;
        prescalarbits = 0b010;  // ck/8: same for both timers

        if (ocr > 255) {
			ocr = F_CPU / frequency / 2 / 32 - 1;
			prescalarbits = 0b011;
        }

        if (ocr > 255) {
			ocr = F_CPU / frequency / 2 / 64 - 1;
			prescalarbits = TIMER == 0 ? 0b011 : 0b100;
			if (ocr > 255) {
				ocr = F_CPU / frequency / 2 / 128 - 1;
				prescalarbits = 0b101;
			}

			if (ocr > 255) {
				ocr = F_CPU / frequency / 2 / 256 - 1;
				prescalarbits = TIMER == 0 ? 0b100 : 0b110;
				if (ocr > 255) {
					// can't do any better than /1024
					ocr = F_CPU / frequency / 2 / 1024 - 1;
					prescalarbits = TIMER == 0 ? 0b101 : 0b111;
				}
			}
        }
    }
    TCCR2B = prescalarbits;

	if (duration_ms > 0)
		remainingToneVsyncs = duration_ms*60/1000; //60 here represents the framerate
	else
		remainingToneVsyncs = -1;

    // Set the OCR for the given timer,
    OCR2A = ocr;
    //set it to toggle the pin by itself
    TCCR2A &= ~(_BV(COM2A1)); //set COM2A1 to 0
    TCCR2A |= _BV(COM2A0);
} // end of tone

/* Stops tone generation
 */
void TVout::noTone() {
	TCCR2B = 0;
	PORT_SND &= ~(_BV(SND_PIN)); //set pin 11 to 0
} // end of noTone
