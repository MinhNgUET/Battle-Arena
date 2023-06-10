#include <SDL_image.h>
#include <SDL_mixer.h>

#include "common.h"

#include "bullets.h"
#include "effects.h"
#include "enemies.h"
#include "highscores.h"
#include "init.h"
#include "items.h"
#include "player.h"
#include "sound.h"
#include "text.h"

extern App   app;
extern Stage stage;

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Couldn't initialize SDL Mixer\n");
		exit(1);
	}

	Mix_AllocateChannels(MAX_SND_CHANNELS);

	app.window = SDL_CreateWindow("Battle Arena", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_ShowCursor(0);
}

void initGame(void)
{
	initFonts();

	initPlayer();

	initEnemies();

	initBullets();

	initItems();

	initEffects();

	initSounds();

	initHighscoreTable();

	memset(&stage, 0, sizeof(Stage));

	loadMusic("music/fight_looped.ogg");

	playMusic(1);
}

void cleanup(void)
{
	Mix_Quit();

	IMG_Quit();

	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}
