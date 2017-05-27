#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>

void return_main(void);
void main_screen(void);
void pre_name_registration(void);
void name_registration_screen(void);
void pre_round_screen(void);
void draw_rankings(void);

void reset_game(void);
void load_progress(void);
void save_progress(void);

extern unsigned high_score_table[5];

#endif