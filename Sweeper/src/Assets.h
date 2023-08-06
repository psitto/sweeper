#ifndef __ASSETS_H__
#define __ASSETS_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "Game.h"

typedef struct Text
{
	SDL_Texture* texture;
	int w, h;
} Text;

SDL_Texture* tex_tile_hidden;
SDL_Texture* tex_tile_flagged;
SDL_Texture* tex_tile_bomb;

TTF_Font* text_font;

Text hint_text[8];
const SDL_Color hint_color[8];

static void make_hint_textures();
void load_assets();
void unload_assets();

#endif