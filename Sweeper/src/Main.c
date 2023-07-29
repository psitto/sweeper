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

	draw_field();

	while (running)
	{
		handle_input();
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