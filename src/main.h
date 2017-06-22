#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

sk_key_t get_key_fast(void);
void game_loop(void);
void check_end_stage(void);
void handle_bonus_timer(void);
void handle_time_ran_out(void);
void flash_1up(void);
void increase_difficulty(void);

#endif