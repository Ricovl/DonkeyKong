#ifndef BONUS_SCORE_H
#define BONUS_SCORE_H

#include <stdint.h>

void spawn_bonus_score(uint8_t score, uint24_t x, uint8_t y);
void update_bonus_scores(void);

// Bonus scores
#define MAX_BONUS_SCORES 4

typedef struct {
	uint8_t y;
	uint24_t x;
	uint8_t counter;
	gfx_image_t *sprite;
	uint8_t background_data[15 * 7 + 2];
} bonus_score_t;

extern bonus_score_t bonus_score[MAX_BONUS_SCORES];
extern uint8_t num_bonus_scores;

#endif