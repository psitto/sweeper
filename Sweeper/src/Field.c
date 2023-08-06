#include "Field.h"

const int DIFFICULTY_SIZE[3] = { 8, 16, 20 };
const int DIFFICULTY_BOMB_QUANTITY[3] = { 10, 40, 99 };

static int make_bombs(unsigned int quantity, unsigned int start_index);
static void calc_hints();

Field make_field(unsigned int difficulty)
{
	Field f = { NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0 };
	if (difficulty < 0 || difficulty > 2)
	{
		printf("Difficulty %d is invalid. Must be 0, 1 or 2.\n", difficulty);
		return f;
	}
	f.difficulty = difficulty;
	f.size = DIFFICULTY_SIZE[difficulty];
	f.tile_count = f.size * f.size;
	f.tiles_remaining = f.tile_count;
	f.tile_size_px = WINW / f.size;
	f.tiles = (Tile*)calloc(f.tile_count, sizeof(Tile));
	f.guesses_remaining = DIFFICULTY_BOMB_QUANTITY[difficulty];
	return f;
}

void destroy_field()
{
	free(field.tiles);
	free(field.bomb_indexes);
}

void draw_tile(unsigned int index)
{
	SDL_Rect dst_rect_tile = {
		(index % field.size) * field.tile_size_px,
		(index / field.size) * field.tile_size_px,
		field.tile_size_px,
		field.tile_size_px };
	if (field.over && field.tiles[index] & IS_BOMB)
	{
		if (field.lost)
		{
			SDL_RenderCopy(renderer, tex_tile_bomb, NULL, &dst_rect_tile);
		}
		else
		{
			SDL_RenderCopy(renderer, tex_tile_flagged, NULL, &dst_rect_tile);
		}
	}
	else if (field.tiles[index] & IS_SHOWN)
	{
		SDL_RenderCopy(renderer, tex_tile_shown, NULL, &dst_rect_tile);
		if ((field.tiles[index] & 0xF))
		{
			Text* text_hint = &hint_text[(field.tiles[index] & 0xF) - 1];
			float text_size_coefficient = ((float)field.tile_size_px / text_hint->h);
			SDL_Rect dst_rect_text;
			dst_rect_text.w = (int)(text_hint->w * text_size_coefficient);
			dst_rect_text.h = (int)(text_hint->h * text_size_coefficient);
			dst_rect_text.x = dst_rect_tile.x + (dst_rect_tile.w - dst_rect_text.w) / 2;
			dst_rect_text.y = dst_rect_tile.y + (dst_rect_tile.h - dst_rect_text.h) / 2;
			SDL_RenderCopy(renderer, text_hint->texture, NULL, &dst_rect_text);
		}
	}
	else if (field.tiles[index] & IS_FLAGGED)
	{
		SDL_RenderCopy(renderer, tex_tile_flagged, NULL, &dst_rect_tile);
	}
	else
	{
		SDL_RenderCopy(renderer, tex_tile_hidden, NULL, &dst_rect_tile);
	}
}

void draw_field()
{
	SDL_RenderClear(renderer);
	for (unsigned int i = 0; i < field.tile_count; i++)
	{
		draw_tile(i);
	}
	SDL_RenderPresent(renderer);
}

int get_tile_index(unsigned int row, unsigned int col)
{
	return row * field.size + col;
}

int get_hovered_tile_index(int x, int y)
{
	unsigned int tile_index = get_tile_index(y / field.tile_size_px, x / field.tile_size_px);
	return (tile_index >= 0 && tile_index < field.tile_count) ? tile_index : -1;
}

TileNeighbors get_tile_neighbours(unsigned int index)
{
	TileNeighbors n;
	n.ul = n.uc = n.ur = n.cl = n.cr = n.ll = n.lc = n.lr = index;
	if (index >= field.size)
	{ // if not first row
		n.uc -= field.size;
		if (index % field.size != 0) // if not first column
			n.ul -= field.size + 1;
		if (index % field.size != field.size - 1) // if not last column
			n.ur -= field.size - 1;
	}
	if (index % field.size != 0) // if not first column
		n.cl -= 1;
	if (index % field.size != field.size - 1) // if not last column
		n.cr += 1;
	if (index < field.size * (field.size - 1))
	{ // if not last row
		n.lc += field.size;
		if (index % field.size != 0) // if not first column
			n.ll += field.size - 1;
		if (index % field.size != field.size - 1) // if not last column
			n.lr += field.size + 1;
	}
	return n;
}

void reveal_tile(unsigned int index)
{
	Tile* tile = &field.tiles[index];
	if (*tile & IS_SHOWN | *tile & IS_FLAGGED) return;
	if (*tile & IS_BOMB)
	{
		lose();
		return;
	}
	*tile |= IS_SHOWN;
	field.tiles_remaining--;
	if (field.tiles_remaining == field.bomb_quantity)
	{
		win();
		return;
	}
	if ((*tile & 0xF) > 0) return;
	TileNeighbors neighbors = get_tile_neighbours(index);
	for (int i = 0; i < 8; i++)
	{
		unsigned int neighbor = ((unsigned int*)&neighbors)[i];
		if (neighbor == index) continue;
		reveal_tile(neighbor);
	}
}

void flag_tile(unsigned int tile_index)
{
	if (field.tiles[tile_index] & IS_SHOWN) return;
	field.tiles[tile_index] ^= IS_FLAGGED;
	field.guesses_remaining += field.tiles[tile_index] & IS_FLAGGED ? -1 : 1;
}

void win()
{
	for (unsigned int i = 0; i < field.bomb_quantity; i++)
	{
		if (field.tiles[field.bomb_indexes[i]] & IS_FLAGGED) continue;
		field.tiles[field.bomb_indexes[i]] |= IS_FLAGGED;
	}
	field.guesses_remaining = 0;
	update_title();
	field.over = 1;
	draw_field();

	long long playtime = time(0) - field.starting_time;
	char time_msg[29];
	snprintf(time_msg, 29, ":-)\nYour playtime was %02lld:%02lld.", playtime / 60, playtime % 60);
	SDL_ShowSimpleMessageBox(0,
		"You won",
		time_msg,
		window);
}

void lose()
{
	field.over = 1;
	field.lost = 1;
}

void on_left_click()
{
	if (field.over)
	{
		renew_field(field.difficulty);
		return;
	}
	int ht = get_hovered_tile_index(mousex, mousey);
	if (ht == -1) return;
	if (field.bomb_quantity == 0 && make_bombs(DIFFICULTY_BOMB_QUANTITY[field.difficulty], ht))
	{
		calc_hints();
		update_title();
		field.starting_time = time(0);
	}
	reveal_tile(ht);
}

void on_right_click()
{
	if (field.over)
	{
		renew_field(field.difficulty);
		return;
	}
	if (field.bomb_quantity == 0) return;
	int ht = get_hovered_tile_index(mousex, mousey);
	if (ht == -1) return;
	flag_tile(ht);
	update_title();
}

void renew_field(unsigned int difficulty)
{
	destroy_field(field);
	field = make_field(difficulty);
	update_title();
}

static int make_bombs(unsigned int quantity, unsigned int start_index)
{
	if (quantity > field.tile_count - 9) // tile_count minus protected tiles
	{
		printf("%d bombs are too many! Field only has %d tiles available.\n", quantity, field.tile_count - 9);
		return 0;
	}
	field.bomb_quantity = field.guesses_remaining = quantity;
	field.bomb_indexes = (unsigned int*)malloc(quantity * sizeof(unsigned int));
	if (field.bomb_indexes == NULL)
	{
		printf("Memory allocation for bomb indexes failed.\n");
		return 0;
	}
	unsigned int chosen_index;
	TileNeighbors start_neighbors = get_tile_neighbours(start_index);
	unsigned int* protected_tiles = (unsigned int*)&start_neighbors;
	do
	{
		chosen_index = rand() % field.tile_count;
		int on_protected_tile = chosen_index == start_index;
		if (on_protected_tile) continue;
		for (int i = 0; i < 8; i++)
		{
			if (chosen_index == protected_tiles[i])
			{
				on_protected_tile = 1;
				break;
			};
		}
		if (on_protected_tile || field.tiles[chosen_index] & IS_BOMB) continue;
		field.tiles[chosen_index] |= IS_BOMB;
		quantity--;
		field.bomb_indexes[quantity] = chosen_index;
	} while (quantity > 0);
	return 1;
}

static void calc_hints()
{
	for (unsigned int i = 0; i < field.bomb_quantity; i++)
	{
		unsigned int bomb_index = field.bomb_indexes[i];
		TileNeighbors neighbors = get_tile_neighbours(bomb_index);
		for (int j = 0; j < 8; j++)
		{
			int neighbor_index = ((unsigned int*)&neighbors)[j];
			if (neighbor_index == bomb_index) continue;
			field.tiles[neighbor_index]++;
		}
	}
}