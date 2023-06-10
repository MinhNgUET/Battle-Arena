#include "common.h"

#include "bullets.h"
#include "draw.h"
#include "effects.h"
#include "player.h"
#include "sound.h"
#include "util.h"

extern App     app;
extern Entity *player;
extern Stage   stage;

static void die(void);

static SDL_Texture *playerTexture;

void initPlayer(void)
{
	playerTexture = loadTexture("gfx/donk.png");
}

void addPlayer(void)
{
	player = malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity));
	stage.entityTail->next = player;
	stage.entityTail = player;

	player->texture = playerTexture;
	player->health = 5;
	player->x = SCREEN_WIDTH*7.5;
	player->y = SCREEN_HEIGHT*7.5;
	player->radius = 24;
	player->side = SIDE_PLAYER;
	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
	player->color.r = player->color.g = player->color.b = player->color.a = 255;

	stage.ammo[WPN_PISTOL] = 25;
	stage.ammo[WPN_UZI] = 100;
	stage.ammo[WPN_SHOTGUN] = 12;

	player->die = die;
}

void doPlayer(void)
{
	if (player != NULL)
	{
		player->dx *= 0.85;
		player->dy *= 0.85;

		if (app.keyboard[SDL_SCANCODE_W])
		{
			player->dy = -PLAYER_SPEED;
		}

		if (app.keyboard[SDL_SCANCODE_S])
		{
			player->dy = PLAYER_SPEED;
		}

		if (app.keyboard[SDL_SCANCODE_A])
		{
			player->dx = -PLAYER_SPEED;
		}

		if (app.keyboard[SDL_SCANCODE_D])
		{
			player->dx = PLAYER_SPEED;
		}

		player->angle = getAngle(player->x - stage.camera.x, player->y - stage.camera.y, app.mouse.x, app.mouse.y);

		if (player->reload == 0 && stage.ammo[player->weaponType] > 0 && app.mouse.button[SDL_BUTTON_LEFT])
		{
			fireDonkBullet();

			stage.ammo[player->weaponType]--;
		}

		if (app.mouse.wheel < 0)
		{
			if (--player->weaponType < WPN_PISTOL)
			{
				player->weaponType = WPN_MAX - 1;
			}

			app.mouse.wheel = 0;
		}

		if (app.mouse.wheel > 0)
		{
			if (++player->weaponType >= WPN_MAX)
			{
				player->weaponType = WPN_PISTOL;
			}

			app.mouse.wheel = 0;
		}

		if (app.keyboard[SDL_SCANCODE_R])
		{
			if (player->weaponType == WPN_PISTOL && stage.ammo[WPN_PISTOL] == 0)
			{
				stage.ammo[WPN_PISTOL] = 25;
			}

			app.keyboard[SDL_SCANCODE_R] = 0;
		}
	}
}

static void die(void)
{
	addDonkDeathEffect();

	player = NULL;

	playSound(SND_DONK_DIE, CH_DONK);
}
