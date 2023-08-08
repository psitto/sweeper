#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include "Game.h"
#include "Field.h"
#include "Assets.h"

int main() {
	srand((unsigned int)time(0));

	running = start_game();
	load_assets();
	field = make_field(1);
	update_title();
	Uint64 t_last = SDL_GetTicks64(), t_current, dt;
	while (running)
	{
		t_current = SDL_GetTicks64();
		dt = t_current - t_last;
		handle_input();
		for (int i = 0; i < field.tile_count; i++)
		{
			for (int j = 0; j < TILE_TWEEN_COUNT; j++)
			{
				twn_update(field.tiles[i].tweens[j], dt);
			}
		}
		draw_field();
		t_last = t_current;
	}

	destroy_field();
	unload_assets();
	end_game();
};

wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	main();
}