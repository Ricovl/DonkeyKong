#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>

void main_screen(void);
void name_registration(uint8_t ranking);
void pre_round_screen(void);
void draw_rankings(void);

void reset_game_data(void);
void load_progress(void);
void save_progress(void);

extern char *rank_num[5];
extern unsigned high_score_table[5];

#endif