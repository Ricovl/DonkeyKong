#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>

// LCD_Control = LCDEN_ENABLE | LCD_8BBP | LCD_isCOLOR | LCD_isTFT | LCD_MONO4 | LCD_SINGLE_PANEL | LCD_BGR | LCD_BEBO_LE | LCD_BEPO_LE | LCD_ENABLE
//RGB looks amazing!!

#define LCD_Control		(*(uint32_t*)0xE30018)

#define LCDEN_ENABLE	(1<<0)	//LCD signals CLLP, CLCP, CLFP, CLAC, and CLLE enabled (HIGH)
#define LCDEN_DISABLE	(0<<0)	//LCD signals CLLP, CLCP, CLFP, CLAC, and CLLE disabled (LOW)

#define LCD_1BBP		(0<<1)
#define LCD_2BBP		(1<<1)
#define LCD_4BBP		(2<<1)
#define LCD_8BBP		(3<<1)
#define LCD_16BBP		(6<<1)
#define LCD_24BBP		(5<<1)

#define LCD_isCOLOR		(0<<4)	//STN LCD is color
#define LCD_isGRAYSCALE	(1<<4)	//STN LCD is monochrome

#define LCD_isSTD		(0<<5)	//is an STN display. Use gray scaler
#define LCD_isTFT		(1<<5)	//is a TFT display. Do not use gray scaler

#define LCD_MONO4		(0<<6)	//mono LCD uses 4-bit interface
#define LCD_MONO8		(1<<6)	//mono LCD uses 8-bit interface

#define LCD_SINGLE_PANEL (0<<7)	//single-panel LCD is in use
#define LCD_DUAL_PANEL	 (1<<7)	//dual-panel LCD is in use

#define	LCD_RGB			(0<<8)	//RGB normal output
#define	LCD_BGR			(1<<8)	//BGR red and blue swapped

#define LCD_BEBO_LE		(0<<9)	//little-endian byte order
#define LCD_BEBO_BE		(1<<9)	//big-endian byte order

#define LCD_BEPO_LE		(0<<10)	//little-endian ordering within a byte
#define LCD_BEPO_BE		(1<<10)	//big-endian pixel ordering within a byte

#define LCD_DISABLE		(0<<11)	//power not gated through to LCD panel and CLD[23:0] signals disabled, (held LOW)
#define LCD_ENABLE		(1<<11)	//power gated through to LCD panel and CLD[23:0] signals enabled, (active)


//Cursor_Control = CURSOR_ENABLE | CURSOR_NUM0
#define Cursor_Control	((*(uint32_t*)0xE30C00))

#define CURSOR_ENABLE	(1<<0)	//Cursor is displayed.
#define CURSOR_DISABLE	(0<<0)	//Cursor not displayed

#define CURSOR_NUM0		(0<<4)	//Cursor0
#define CURSOR_NUM1		(1<<4)	//Cursor1
#define CURSOR_NUM2		(2<<4)	//Cursor2
#define CURSOR_NUM3		(3<<4)	//Cursor3



//Cursor_Config = CURSOR_64x64 | CURSOR_SYNC
#define Cursor_Config	((*(uint32_t*)0xE30C04))

#define CURSOR_32x32	(0<<0)	//32x32 pixel cursor
#define CURSOR_64x64	(1<<0)	//64x64 pixel cursor

#define CURSOR_ASYNC	(0<<1)	//Cursor coordinates asynchronous
#define CURSOR_SYNC		(1<<1)	//Cursor coordinates synchronized to frame synchronization pulse


#define CursorImage		(**(uint8_t(**)[1024])0xE30800)

#define CursorX			((*(uint24_t*)0xE30C10))
#define CursorY			((*(uint8_t*)0xE30C12))

#define CrsrClipX		((*(uint8_t*)0xE30C17))
#define CrsrClipY		((*(uint8_t*)0xE30C16))


#endif // MAIN_H