#include "Field.h"

const int DIFFICULTY_SIZE[3] = { 8, 16, 20 };
const int DIFFICULTY_BOMB_QUANTITY[3] = { 10, 40, 99 };

Field make_field(unsigned int difficulty)
{
	Field f;
	f.size = DIFFICULTY_SIZE[difficulty];
	f.tile_count = f.size * f.size;
	f.tile_size_px = WINW / f.size;
	f.bomb_quantity = 0;
	f.difficulty = difficulty;
	f.guesses_remaining = DIFFICULTY_BOMB_QUANTITY[difficulty];
	f.tiles_remaining = f.tile_count;
	f.tiles = (Tile*)calloc(f.tile_count, sizeof(Tile));
	for (int i = 0; i < f.tile_count; i++)
	{
		for (int j = 0; j < TILE_TWEEN_COUNT; j++)
		{
			f.tiles[i].tweens[j] = twn_make_player();
		}
	}
	f.over = 0;
	f.lost = 0;
	return f;
}


void destroy_field()
{
	for (int i = 0; i < field.tile_count; i++)
	{
		for (int j = 0; j < TILE_TWEEN_COUNT; j++)
		{
			free(field.tiles[i].tweens[j]);
		}
	}
	free(field.tiles);
	if (field.bomb_quantity != 0)
	{
		free(field.bomb_indexes);
	}
}

void draw_tile(unsigned int index)
{
	SDL_Rect dst_rect_tile = {
		(index % field.size) * field.tile_size_px,
		(index / field.size) * field.tile_size_px,
		field.tile_size_px,
		field.tile_size_px };
	if (field.lost && field.tiles[index].data & IS_BOMB)
	{
		render_bg(tex_tile_bomb, &dst_rect_tile);
	}
	else if (field.tiles[index].data & IS_SHOWN)
	{
		// render nothing
		if ((field.tiles[index].data & 0xF))
		{
			Text* text_hint = &hint_text[(field.tiles[index].data & 0xF) - 1];
			float text_size_coefficient = ((float)field.tile_size_px / text_hint->h);
			SDL_Rect dst_rect_text;
			dst_rect_text.w = (int)(text_hint->w * text_size_coefficient);
			dst_rect_text.h = (int)(text_hint->h * text_size_coefficient);
			dst_rect_text.x = dst_rect_tile.x + (dst_rect_tile.w - dst_rect_text.w) / 2;
			dst_rect_text.y = dst_rect_tile.y + (dst_rect_tile.h - dst_rect_text.h) / 2;
			render_bg(text_hint->texture, &dst_rect_text);
		}
	}
	else 
	{
		render_bg(tex_tile_hidden, &dst_rect_tile);
		if (field.tiles[index].data & IS_FLAGGED)
		{
			SDL_Rect dst_rect_flag = get_scaled_rect(dst_rect_tile, field.tiles[index].flag_scale);
			render_fg(tex_flag, &dst_rect_flag);
			// SDL_RenderCopy(renderer, tex_flag, NULL, &dst_rect_flag);
		}
	}
}

void draw_field()
{
	SDL_SetRenderTarget(renderer, tex_bg);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, COLOR_BG_CLEAR_R, COLOR_BG_CLEAR_G, COLOR_BG_CLEAR_B, 0xFF);
	SDL_RenderClear(renderer);

	for (unsigned int i = 0; i < field.tile_count; i++)
	{
		draw_tile(i);
	}

	SDL_RenderCopy(renderer, tex_bg, 0, &win_rect);
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
	TileData* tile_data = &field.tiles[index].data;
	if (*tile_data & IS_SHOWN | *tile_data & IS_FLAGGED) return;
	if (*tile_data & IS_BOMB)
	{
		lose();
		return;
	}
	*tile_data |= IS_SHOWN;
	field.tiles_remaining--;
	if (field.tiles_remaining == field.bomb_quantity)
	{
		win();
		return;
	}
	if ((*tile_data & 0xF) > 0) return;
	TileNeighbors neighbors = get_tile_neighbours(index);
	for (int i = 0; i < 8; i++)
	{
		unsigned int neighbor = ((unsigned int*)&neighbors)[i];
		if (neighbor == index) continue;
		reveal_tile(neighbor);
	}
}

static void anim_flag_placement(unsigned int tile_index)
{
	twn_Player* tween = field.tiles[tile_index].tweens[0];
	twn_set_target(tween, &field.tiles[tile_index].flag_scale);
	twn_set_motion(tween, &MOTION_FLAG_PLACEMENT);
	twn_set_duration(tween, MOTION_FLAG_PLACEMENT_DURATION);
	twn_play(tween);
}

void flag_tile(unsigned int tile_index)
{
	if (field.tiles[tile_index].data & IS_SHOWN)
	{
		return;
	}
	if (!(field.tiles[tile_index].data & IS_FLAGGED))
	{
		anim_flag_placement(tile_index);
	}
	if (field.tiles[tile_index].data & IS_SHOWN) return;
	field.tiles[tile_index].data ^= IS_FLAGGED;
	field.guesses_remaining += field.tiles[tile_index].data & IS_FLAGGED ? -1 : 1;
}

static void cb_show_win_message(twn_Player* p)
{
	long long playtime = time(0) - field.starting_time;
	char time_msg[29];
	snprintf(time_msg, 29, ":-)\nYour playtime was %02lld:%02lld.", playtime / 60, playtime % 60);
	SDL_ShowSimpleMessageBox(0,
		"You won",
		time_msg,
		window);
}

void win()
{
	bool callback_set = false;
	for (unsigned int i = 0; i < field.bomb_quantity; i++)
	{
		if (field.tiles[field.bomb_indexes[i]].data & IS_FLAGGED) continue;
		flag_tile(field.bomb_indexes[i]);
		if (callback_set) continue;
		twn_set_callback(field.tiles[field.bomb_indexes[i]].tweens[0], cb_show_win_message);
		callback_set = true;
	}
	if (!callback_set)
	{
		cb_show_win_message(NULL);
	}
	field.guesses_remaining = 0;
	update_title();
	field.over = 1;
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
		if (on_protected_tile || field.tiles[chosen_index].data & IS_BOMB) continue;
		field.tiles[chosen_index].data |= IS_BOMB;
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
			field.tiles[neighbor_index].data++;
		}
	}
}