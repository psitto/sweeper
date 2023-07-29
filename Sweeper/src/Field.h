#ifndef __FIELD_H__
#define __FIELD_H__

typedef char Tile;
typedef struct Field
{
	Tile* tiles;
	unsigned int size;
	unsigned int tile_count;
	unsigned int tile_size_px;
	unsigned int bomb_quantity;
	unsigned int difficulty;
	unsigned int tiles_remaining;
	unsigned int over;
	unsigned int* bomb_indexes;
	int guesses_remaining;
	long long starting_time;
} Field;
typedef struct TileNeighbors
{
	unsigned int ul, uc, ur, cl, cr, ll, lc, lr;
} TileNeighbors;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Assets.h"
#include "Game.h"

#define IS_BOMB 0x20
#define IS_SHOWN 0x10
#define IS_FLAGGED 0x40
#define HINT_MASK 0xF

const int DIFFICULTY_SIZE[3];
const int DIFFICULTY_BOMB_QUANTITY[3];

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

#endif