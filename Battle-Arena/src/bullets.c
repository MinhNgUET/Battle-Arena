#include "common.h"

#include "bullets.h"
#include "draw.h"
#include "effects.h"
#include "sound.h"
#include "util.h"

extern App     app;
extern Entity *player;
extern Entity *self;
extern Stage   stage;

static void fireDonkUzi(void);
static void fireDonkShotgun(void);
static void fireDonkPistol(void);
static void bulletHitEntity(Entity *b);

static SDL_Texture *bulletTexture;

void initBullets(void)
{
	bulletTexture = loadTexture("gfx/bullet.png");
}

void doBullets(void)
{
	Entity *b, *prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		b->x += b->dx;
		b->y += b->dy;

		bulletHitEntity(b);

		if (--b->health <= 0)
		{
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}

		prev = b;
	}
}

static void bulletHitEntity(Entity *b)
{
	Entity *e;
	int     distance;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (e->side != SIDE_NONE && e->side != b->side)
		{
			distance = getDistance(e->x, e->y, b->x, b->y);

			if (distance < e->radius + b->radius)
			{
				b->health = 0;
				e->health--;
				addBulletHitEffect(b);

				if (e == player)
				{
					playSound(SND_DONK_HIT, CH_DONK);
				}
				else
				{
					playSound(SND_ENEMY_HIT, CH_HIT);
				}

				return;
			}
		}
	}
}

void drawBullets(void)
{
	Entity *b;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		SDL_SetTextureColorMod(b->texture, b->color.r, b->color.g, b->color.b);

		blitRotated(b->texture, b->x - stage.camera.x, b->y - stage.camera.y, b->angle);
	}
}

void fireDonkBullet(void)
{
	switch (player->weaponType)
	{
		case WPN_UZI:
			fireDonkUzi();
			break;

		case WPN_SHOTGUN:
			fireDonkShotgun();
			break;

		default:
			fireDonkPistol();
			break;
	}
}

static Entity *createDonkBullet(void)
{
	Entity *b;

	b = malloc(sizeof(Entity));
	memset(b, 0, sizeof(Entity));
	stage.bulletTail->next = b;
	stage.bulletTail = b;

	b->x = player->x;
	b->y = player->y;
	b->texture = bulletTexture;
	b->health = FPS * 2;
	b->angle = player->angle;
	b->radius = 16;
	b->side = SIDE_PLAYER;
	SDL_QueryTexture(b->texture, NULL, NULL, &b->w, &b->h);
	b->color.r = 128;
	b->color.g = b->color.b = b->color.a = 255;

	return b;
}

static void fireDonkUzi(void)
{
	Entity *b;

	b = createDonkBullet();

	calcSlope(app.mouse.x, app.mouse.y, b->x - stage.camera.x, b->y - stage.camera.y, &b->dx, &b->dy);

	b->dx *= 16;
	b->dy *= 16;

	player->reload = 4;

	playSound(SND_UZI, CH_DONK);
}

static void fireDonkShotgun(void)
{
	Entity *b;
	int     i, destX, destY;
	float   dx, dy;

	calcSlope(app.mouse.x, app.mouse.y, player->x - stage.camera.x, player->y - stage.camera.y, &dx, &dy);

	dx = player->x + (dx * 128);
	dy = player->y + (dy * 128);

	for (i = 0; i < 5; i++)
	{
		b = createDonkBullet();

		destX = dx + (rand() % 24 - rand() % 24);
		destY = dy + (rand() % 24 - rand() % 24);

		calcSlope(destX, destY, b->x, b->y, &b->dx, &b->dy);

		b->dx *= 16;
		b->dy *= 16;
	}

	player->reload = FPS * 0.75;

	playSound(SND_SHOTGUN, CH_DONK);
}

static void fireDonkPistol(void)
{
	Entity *b;

	b = createDonkBullet();

	calcSlope(app.mouse.x, app.mouse.y, b->x - stage.camera.x, b->y - stage.camera.y, &b->dx, &b->dy);

	b->dx *= 16;
	b->dy *= 16;

	player->reload = 16;

	playSound(SND_PISTOL, CH_DONK);
}

void fireEnemyBullet(void)
{
	Entity *b;

	b = malloc(sizeof(Entity));
	memset(b, 0, sizeof(Entity));
	stage.bulletTail->next = b;
	stage.bulletTail = b;

	b->x = self->x;
	b->y = self->y;
	b->texture = bulletTexture;
	b->health = FPS * 2;
	b->angle = getAngle(self->x, self->y, player->x, player->y);
	b->radius = 16;
	b->side = SIDE_ENEMY;
	SDL_QueryTexture(b->texture, NULL, NULL, &b->w, &b->h);
	b->color.r = 255;
	b->color.g = b->color.b = 0;
	b->color.a = 255;

	calcSlope(player->x, player->y, b->x, b->y, &b->dx, &b->dy);

	b->dx *= 10;
	b->dy *= 10;

	playSound(SND_ENEMY_BULLET, CH_ENEMY_BULLET);
}
