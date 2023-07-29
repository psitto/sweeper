#include "Game.h"

int start_game()
{
	time_t t = time(0);
	struct tm tinfo;
	localtime_s(&tinfo, &t);
	snprintf(msg_about, MESSAGE_ABOUT_LENGTH, MESSAGE_ABOUT_FORMAT, tinfo.tm_year + 1900);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	SDL_SetHint(SDL_HINT_WINDOWS_ENABLE_MENU_MNEMONICS, "1");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		printf("SDL2 initialization error: %s\n", SDL_GetError());
		return 0;
	}
	window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("SDL2 window creation error.\n");
		return 0;
	}
	window_handle = get_window_handle();
	make_window_menu();
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("SDL2 renderer creation error.\n");
		return 0;
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		printf("SDL_image initialization error.");
		return 0;
	}
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf initialization error.");
		return 0;
	}
	return 1;
}

void handle_input() {
	SDL_GetMouseState(&mousex, &mousey);
	SDL_Event e;
	SDL_PumpEvents();
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT) on_left_click();
			else if (e.button.button == SDL_BUTTON_RIGHT) on_right_click();
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_r)
			{
				renew_field(field.difficulty);
				draw_field();
			}
			break;
		case SDL_QUIT:
			running = 0;
			break;
		case SDL_SYSWMEVENT:
			if (e.syswm.msg->msg.win.msg != WM_COMMAND) break;
			int menu_id = (int)LOWORD(e.syswm.msg->msg.win.wParam);
			switch (menu_id)
			{
			case MENUID_NEW_EASY:
			case MENUID_NEW_MEDIUM:
			case MENUID_NEW_HARD:
				renew_field(menu_id);
				draw_field();
				break;
			case MENUID_RESET:
				renew_field(field.difficulty);
				break;
			case MENUID_ABOUT:
				SDL_ShowSimpleMessageBox(0,
					"About Sweeper",
					msg_about,
					window);
				break;
			}
			break;
		}
	}
}

void end_game()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	window = NULL;
	renderer = NULL;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	DestroyMenu(hmenu_bar);
	DestroyMenu(hmenu_game);
	DestroyMenu(hmenu_new);
	DestroyMenu(hmenu_help);
}

static HWND get_window_handle()
{
	SDL_SysWMinfo info_win;
	SDL_VERSION(&info_win.version);
	if (!SDL_GetWindowWMInfo(window, &info_win))
	{
		printf("Couldn't get window information.\n");
		return NULL;
	}
	return info_win.info.win.window;
}

void update_title()
{
	char new_title[30];
	snprintf(new_title, 30, "Sweeper: %d remaining", field.guesses_remaining);
	SDL_SetWindowTitle(window, new_title);
}

static void make_window_menu()
{
	hmenu_bar = CreateMenu();
	hmenu_game = CreatePopupMenu();
	hmenu_new = CreatePopupMenu();
	hmenu_help = CreatePopupMenu();

	AppendMenu(hmenu_bar, MF_POPUP, (UINT_PTR)hmenu_game, L"&Game");
	AppendMenu(hmenu_bar, MF_POPUP, (UINT_PTR)hmenu_help, L"&Help");

	AppendMenu(hmenu_game, MF_POPUP, (UINT_PTR)hmenu_new, L"&New");
	AppendMenu(hmenu_new, MF_STRING, MENUID_NEW_EASY, L"Easy");
	AppendMenu(hmenu_new, MF_STRING, MENUID_NEW_MEDIUM, L"Medium");
	AppendMenu(hmenu_new, MF_STRING, MENUID_NEW_HARD, L"Hard");
	AppendMenu(hmenu_game, MF_STRING, MENUID_RESET, L"Reset\tR");

	AppendMenu(hmenu_help, MF_STRING, MENUID_ABOUT, L"About...");

	SetMenu(window_handle, hmenu_bar);
}