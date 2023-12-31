#ifndef __GAME_H__
#define __GAME_H__

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <time.h>
#include "Field.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 1

#define WINW 800
#define WINH 800

#define MENUID_NEW_EASY 0
#define MENUID_NEW_MEDIUM 1
#define MENUID_NEW_HARD 2
#define MENUID_RESET 3
#define MENUID_ABOUT 4

#define MESSAGE_ABOUT_FORMAT "\
Sweeper v%d.%d.%d\n\n\
Copyright (c) 2023-%04d psitto\n\
The MIT License\n\n\
Built with SDL2, SDL2_image and SDL2_ttf."
#define MESSAGE_ABOUT_LENGTH 115

#define COLOR_BG_CLEAR_R 0x96
#define COLOR_BG_CLEAR_G 0x96
#define COLOR_BG_CLEAR_B 0x96

SDL_Window* window;
SDL_Renderer* renderer;
const SDL_Rect win_rect;
SDL_Texture* tex_bg;
Field field;
int running;
int mousex;
int mousey;
HWND window_handle;
static HMENU hmenu_bar;
static HMENU hmenu_game;
static HMENU hmenu_new;
static HMENU hmenu_help;
static char msg_about[MESSAGE_ABOUT_LENGTH];

int start_game();
void handle_input();
void end_game();
void update_title();
void render_bg(SDL_Texture* texture, const SDL_Rect* dstrect);
void render_fg(SDL_Texture* texture, const SDL_Rect* dstrect);
SDL_Rect get_scaled_rect(SDL_Rect rect, float scale);
static HWND get_window_handle();
static void make_window_menu();
#endif