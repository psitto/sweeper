#include "Assets.h"

const SDL_Color hint_color[8] = {
	{ 4, 40, 204 },
	{ 46, 234, 46 },
	{ 255, 28, 28 },
	{ 0, 138, 173 },
	{ 0, 153, 12 },
	{ 255, 82, 0 },
	{ 99, 0, 145 },
	{ 40, 40, 40 }
};

static void make_hint_textures()
{
	for (int i = 1; i <= 8; i++)
	{
		char number_str[2] = { '0' + i, '\0' };
		SDL_Surface* sfc = TTF_RenderText_Blended(text_font, number_str, hint_color[i - 1]);
		Text text = { SDL_CreateTextureFromSurface(renderer, sfc), sfc->w, sfc->h };
		hint_text[i - 1] = text;
		SDL_FreeSurface(sfc);
	}
}

void load_assets()
{
	tex_tile_hidden = IMG_LoadTexture(renderer, "assets\\tile.png");
	tex_tile_bomb = IMG_LoadTexture(renderer, "assets\\tile_bomb.png");
	tex_flag = IMG_LoadTexture(renderer, "assets\\flag.png");
	SDL_SetTextureBlendMode(tex_flag, SDL_BLENDMODE_NONE);
	text_font = TTF_OpenFont("assets\\Poppins-Regular.ttf", 64);
	make_hint_textures();
}

void unload_assets()
{
	SDL_DestroyTexture(tex_tile_hidden);
	SDL_DestroyTexture(tex_tile_bomb);
	SDL_DestroyTexture(tex_flag);
	for (int i = 0; i < 8; i++)
	{
		SDL_DestroyTexture(hint_text[i].texture);
	}
	TTF_CloseFont(text_font);
}