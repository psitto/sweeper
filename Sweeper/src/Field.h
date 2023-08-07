#ifndef __FIELD_H__
#define __FIELD_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Tween.h"
#include "Easing.h"
#include <SDL_rect.h>

typedef char TileData;
typedef struct Tile
{
	TileData data;
	float tile_scale;
	float flag_scale;
	twn_Player* tween;
} Tile;
typedef struct Field
{
	Tile* tiles;
	unsigned int size;
	unsigned int tile_count;
	unsigned int tile_size_px;
	unsigned int bomb_quantity;
	unsigned int difficulty;
	unsigned int tiles_remaining;
	unsigned int* bomb_indexes;
	int guesses_remaining;
	int over, lost;
	long long starting_time;
} Field;
typedef struct TileNeighbors
{
	unsigned int ul, uc, ur, cl, cr, ll, lc, lr;
} TileNeighbors;

#include "Assets.h"
#include "Game.h"

#define IS_BOMB 0x20
#define IS_SHOWN 0x10
#define IS_FLAGGED 0x40
#define HINT_MASK 0xF

const int DIFFICULTY_SIZE[3];
const int DIFFICULTY_BOMB_QUANTITY[3];
const static twn_Motion MOTION_FLAG_PLACEMENT = { 1.3, 1.0, ease_out_back };
const static time_t MOTION_FLAG_PLACEMENT_DURATION = 200;

Field make_field(unsigned int difficulty);
void destroy_field();
void draw_tile(unsigned int index);
void draw_field();
int get_tile_index(unsigned int row, unsigned int col);
int get_hovered_tile_index(int x, int y);
TileNeighbors get_tile_neighbours(unsigned int index);
void reveal_tile(unsigned int index);
void flag_tile(unsigned int index);
void win();
void lose();
void on_left_click();
void on_right_click();
void renew_field(unsigned int difficulty);
static int make_bombs(unsigned int quantity, unsigned int start_index);
static void calc_hints();

#endif