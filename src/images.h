#ifndef IMAGES_H
#define IMAGES_H

#include <stdint.h>
#include <graphx.h>
#include "stdlib.h"
#include "gfx\sprites_gfx.h"

void decompress_images(void);

// Jumpman
extern gfx_image_t *jumpman_sprite[2][18];

extern gfx_image_t *jumpman_dead[6];

// Barrels
extern gfx_image_t *rolling_barrel[2][6];

// Firefoxes
extern gfx_image_t *firefox_sprite[2][4];

// Kong
extern gfx_image_t *kong_goofy;
extern gfx_image_t *kong_sprite[13];

extern gfx_image_t *kong_crazy_eye[2];
extern gfx_image_t *kong_knockedout_sprite[2];

// Bonus sprites
extern gfx_image_t *bonus_score_sprite[5];

// Hammer sprites
extern gfx_image_t *hammer_sprite[2][4];

// Flame on top of oilcan
extern gfx_image_t *fire[4];

// Pauline
extern gfx_image_t *pauline_sprite[2][3];

// Bouncers
extern gfx_image_t *bouncer_sprite[2];

// Pauline's items
extern gfx_image_t *pauline_item[3];

// Pulleys
extern gfx_image_t *pulley_right[3];
extern gfx_image_t *pulley_left[3];

#endif
