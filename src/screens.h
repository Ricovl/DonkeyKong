#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>

void return_main(void);
void main_screen(void);
void pre_name_registration(void);
void name_registration_screen(void);
void pre_round_screen(void);
void draw_rankings(void);
void credits_screen(void);

void reset_game(void);
void load_progress(void);
void save_progress(void);

sk_key_t get_key_fast(void);
void draw_girder_text(uint8_t *text_data, uint24_t x, uint8_t y);

typedef struct {
	uint8_t cursorX;
	uint8_t cursorY;
	uint8_t rankNum;
	uint8_t charNum;
	uint8_t timer;
} HighScore_t;

extern unsigned high_score_table[5];

extern const uint8_t donkey_text[];
extern const uint8_t kong_text[];

#endif