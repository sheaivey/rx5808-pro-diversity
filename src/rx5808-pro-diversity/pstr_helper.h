#ifndef PSTR_HELPER_H
#define PSTR_HELPER_H

#include <avr/pgmspace.h>

// Modified PSTR that pushes string into a char* buffer for easy use.
//
// There is only one buffer so this will cause problems if you need to pass two
// strings to one function.
#define PSTR2(x) PSTRtoBuffer_P(PSTR(x))
#define PSTR2_BUFFER_SIZE 48 // May need adjusted depending on your needs.

char *PSTRtoBuffer_P(PGM_P str);

#endif
