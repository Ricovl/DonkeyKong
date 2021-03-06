#include <stdint.h>
#include "font.h"

uint8_t font_data[448] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 037 (%)
	0x3C, 0x66, 0x6E, 0x76, 0x66, 0x3C, 0x00, 0x00,	// Char 038 (&)
	0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00,	// Char 039 (')
	0x3C, 0x66, 0x0C, 0x18, 0x30, 0x7E, 0x00, 0x00,	// Char 040 (()
	0x7E, 0x0C, 0x18, 0x0C, 0x66, 0x3C, 0x00, 0x00,	// Char 041 ())
	0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x00, 0x00,	// Char 042 (*)
	0x7E, 0x60, 0x7C, 0x06, 0x66, 0x3C, 0x00, 0x00,	// Char 043 (+)
	0x3C, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00, 0x00,	// Char 044 (,)
	0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x00, 0x00,	// Char 045 (-)
	0x3C, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x00,	// Char 046 (.)
	0x3C, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00, 0x00,	// Char 047 (/)
	0x38, 0x4C, 0xC6, 0xC6, 0xC6, 0x64, 0x38, 0x00,	// Char 048 (0)
	0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,	// Char 049 (1)
	0x7C, 0xC6, 0x0E, 0x3C, 0x78, 0xE0, 0xFE, 0x00,	// Char 050 (2)
	0x7E, 0x0C, 0x18, 0x3C, 0x06, 0xC6, 0x7C, 0x00,	// Char 051 (3)
	0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x00,	// Char 052 (4)
	0xFC, 0xC0, 0xFC, 0x06, 0x06, 0xC6, 0x7C, 0x00,	// Char 053 (5)
	0x3C, 0x60, 0xC0, 0xFC, 0xC6, 0xC6, 0x7C, 0x00,	// Char 054 (6)
	0xFE, 0xC6, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,	// Char 055 (7)
	0x78, 0xC4, 0xE4, 0x78, 0x9E, 0x86, 0x7C, 0x00,	// Char 056 (8)
	0x7C, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0x78, 0x00,	// Char 057 (9)
	0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00,	// Char 058 (:)
	0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x00, 0x00,	// Char 059 (;)
	0x18, 0x30, 0x60, 0x40, 0x60, 0x30, 0x18, 0x00,	// Char 060 (<)
	0x00, 0x00, 0xFC, 0x00, 0xFC, 0x00, 0x00, 0x00,	// Char 061 (=)
	0x60, 0x30, 0x18, 0x08, 0x18, 0x30, 0x60, 0x00,	// Char 062 (>)
	0x78, 0x84, 0x04, 0x18, 0x20, 0x00, 0x20, 0x00,	// Char 063 (?)
	0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x7E, 0x00,	// Char 064 (@)
	0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0x00,	// Char 065 (A)
	0xFC, 0xC6, 0xC6, 0xFC, 0xC6, 0xC6, 0xFC, 0x00,	// Char 066 (B)
	0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00,	// Char 067 (C)
	0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xCC, 0xF8, 0x00,	// Char 068 (D)
	0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00,	// Char 069 (E)
	0xFE, 0xC0, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0x00,	// Char 070 (F)
	0x3E, 0x60, 0xC0, 0xCE, 0xC6, 0x66, 0x3E, 0x00,	// Char 071 (G)
	0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00,	// Char 072 (H)
	0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,	// Char 073 (I)
	0x06, 0x06, 0x06, 0x06, 0x06, 0xC6, 0x7C, 0x00,	// Char 074 (J)
	0xC6, 0xCC, 0xD8, 0xF0, 0xF8, 0xDC, 0xCE, 0x00,	// Char 075 (K)
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00,	// Char 076 (L)
	0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00,	// Char 077 (M)
	0xC6, 0xE6, 0xF6, 0xFE, 0xDE, 0xCE, 0xC6, 0x00,	// Char 078 (N)
	0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00,	// Char 079 (O)
	0xFC, 0xC6, 0xC6, 0xC6, 0xFC, 0xC0, 0xC0, 0x00,	// Char 080 (P)
	0x7C, 0xC6, 0xC6, 0xC6, 0xDE, 0xCC, 0x7A, 0x00,	// Char 081 (Q)
	0xFC, 0xC6, 0xC6, 0xCE, 0xF8, 0xDC, 0xCE, 0x00,	// Char 082 (R)
	0x78, 0xCC, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00,	// Char 083 (S)
	0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,	// Char 084 (T)
	0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00,	// Char 085 (U)
	0xC6, 0xC6, 0xC6, 0xEE, 0x7C, 0x38, 0x10, 0x00,	// Char 086 (V)
	0xC6, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x44, 0x00,	// Char 087 (W)
	0xC6, 0xEE, 0x7C, 0x38, 0x7C, 0xEE, 0xC6, 0x00,	// Char 088 (X)
	0x66, 0x66, 0x24, 0x3C, 0x18, 0x18, 0x18, 0x00,	// Char 089 (Y)
	0xFE, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xFE, 0x00,	// Char 090 (Z)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00,	// Char 091 ([)
	0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00,	// Char 092 (\)
};