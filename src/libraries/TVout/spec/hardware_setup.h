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

// sound is output on OC2A
// sync output is on OC1A

//ENABLE_FAST_OUTPUT chooses the highest bit of a port over the original output method
//comment out this line to switch back to the original output pins.
#define ENABLE_FAST_OUTPUT

#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

// device specific settings.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#if defined(ENABLE_FAST_OUTPUT)
#define PORT_VID	PORTA
#define	DDR_VID		DDRA
#define VID_PIN		7
#else
//video
#define PORT_VID	PORTB
#define	DDR_VID		DDRB
#define VID_PIN		6
#endif
//sync
#define PORT_SYNC	PORTB
#define DDR_SYNC	DDRB
#define	SYNC_PIN	5
//sound
#define PORT_SND	PORTB
#define DDR_SND		DDRB
#define	SND_PIN		4

#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
//video
#if defined(ENABLE_FAST_OUTPUT)
#define PORT_VID	PORTA
#define	DDR_VID		DDRA
#define VID_PIN		7
#else
#define PORT_VID	PORTD
#define	DDR_VID		DDRD
#define VID_PIN		4
#endif
//sync
#define PORT_SYNC	PORTD
#define DDR_SYNC	DDRD
#define SYNC_PIN	5
//sound
#define PORT_SND	PORTD
#define DDR_SND		DDRD
#define	SND_PIN		7

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
//video
#if defined(ENABLE_FAST_OUTPUT)
#define PORT_VID	PORTD
#define	DDR_VID		DDRD
#define	VID_PIN		7
#else
#define PORT_VID	PORTB
#define	DDR_VID		DDRB
#define	VID_PIN		0
#endif
//sync
#define PORT_SYNC	PORTB
#define DDR_SYNC	DDRB
#define SYNC_PIN	1
//sound
#define PORT_SND	PORTB
#define DDR_SND		DDRB
#define	SND_PIN		3

#elif defined (__AVR_AT90USB1286__)
//video
#define PORT_VID	PORTF
#define	DDR_VID		DDRF
#define	VID_PIN		7
//sync
#define PORT_SYNC	PORTB
#define DDR_SYNC	DDRB
#define SYNC_PIN	5
//sound
#define PORT_SND	PORTB
#define DDR_SND		DDRB
#define	SND_PIN		4
#endif

//automatic BST/BLD/ANDI macro definition
#if VID_PIN == 0
#define BLD_HWS		"bld	r16,0\n\t"
#define BST_HWS		"bst	r16,0\n\t"
#define ANDI_HWS	"andi	r16,0xFE\n"
#elif VID_PIN == 1
#define BLD_HWS		"bld	r16,1\n\t"
#define BST_HWS		"bst	r16,1\n\t"
#define ANDI_HWS	"andi	r16,0xFD\n"
#elif VID_PIN == 2
#define BLD_HWS		"bld	r16,2\n\t"
#define BST_HWS		"bst	r16,2\n\t"
#define ANDI_HWS	"andi	r16,0xFB\n"
#elif VID_PIN == 3
#define BLD_HWS		"bld	r16,3\n\t"
#define BST_HWS		"bst	r16,3\n\t"
#define ANDI_HWS	"andi	r16,0xF7\n"
#elif VID_PIN == 4
#define BLD_HWS		"bld	r16,4\n\t"
#define BST_HWS		"bst	r16,4\n\t"
#define ANDI_HWS	"andi	r16,0xEF\n"
#elif VID_PIN == 5
#define BLD_HWS		"bld	r16,5\n\t"
#define BST_HWS		"bst	r16,5\n\t"
#define ANDI_HWS	"andi	r16,0xDF\n"
#elif VID_PIN == 6
#define BLD_HWS		"bld	r16,6\n\t"
#define BST_HWS		"bst	r16,6\n\t"
#define ANDI_HWS	"andi	r16,0xBF\n"
#elif VID_PIN == 7
#define BLD_HWS		"bld	r16,7\n\t"
#define BST_HWS		"bst	r16,7\n\t"
#define ANDI_HWS	"andi	r16,0x7F\n"
#endif
#endif