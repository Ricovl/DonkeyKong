#ifndef JUMPMAN_H
#define JUMPMAN_H

#include <stdint.h>

void init_jumpman(uint24_t x, uint8_t y);

void move_jumpman(void);

bool girder_collision(void);
void climb_ladder(void);
bool ladder_in_range(void);
void check_jump_over(void);
uint8_t check_collision(uint8_t loop, uint8_t *structp, uint8_t width, uint8_t height, uint8_t offsetx, uint8_t offsety, uint8_t size);

void check_jumpman_falling(void);
void handle_jumpman_falling(void);

void animate_jumpman_dead(void);
void bonus_item_picked_up(void);
void hammer_stuff(void);


extern const uint8_t jumpman_walking_sprite_table[4];
extern uint8_t bonus_item_height[3];

#endif