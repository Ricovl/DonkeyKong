#ifndef FIREFOXES_H
#define FIREFOXES_H

#include <stdint.h>
#include "stdlib.h"

void update_firefoxes(void);
void move_firefoxes(void);
void release_firefox(void);

// Firefoxes
#define MAX_FIREFOXES 5

typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;
	uint8_t actualY;

	uint8_t sprite, dir;

	uint8_t moveCounter, jumpCounter;
	uint8_t reverseTimer, ladderTimer, freezeTimer;
	uint8_t freezerMode;
	
	bool onLadder;
	uint8_t dismountY;
	bool freezeFlag;
	bool isSpawning;

	uint8_t background_data[16 * 16 + 2];
} firefox_t;

extern firefox_t firefox[MAX_FIREFOXES];
extern uint8_t num_firefoxes;
extern bool releaseFirefox;

void reverse_firefox_random(firefox_t *this_firefox);
void handle_freezer(firefox_t *this_firefox);
void mount_dismount_ladder(firefox_t *this_firefox);
void animate_firefox(firefox_t *this_firefox);
void handle_firefox_spawning(firefox_t *this_firefox);

// Oilcan
typedef struct {
	uint8_t y;
	uint8_t sprite;
	uint24_t x;

	bool onFire;

	uint8_t fireRelease;				// 1 when fire on screen(flame is on), 3 when a fire is to be released, else 0
	uint8_t updateTimer, releaseTimer;

	uint8_t background_data[16 * 16 + 2];
} oilcan_t;

extern oilcan_t oilcan;

// Lookup tables
extern const uint8_t firefox_girders_jumpTable[31];
extern const uint8_t firefox_conveyors_jumpTable[23];
extern const uint8_t firefox_rivets_spawnTable[2][8];
extern const uint8_t firefox_bobbingTable[18];

#endif