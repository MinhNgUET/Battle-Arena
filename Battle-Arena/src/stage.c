#include "common.h"

#include "bullets.h"
#include "camera.h"
#include "draw.h"
#include "effects.h"
#include "enemies.h"
#include "entities.h"
#include "highscores.h"
#include "items.h"
#include "player.h"
#include "stage.h"
#include "text.h"
#include "util.h"

extern App     app;
extern Entity *player;
extern Stage   stage;

static void logic(void);
static void draw(void);
static void drawGrid(void);
static void drawHud(void);
static void drawWeaponInfo(char *name, int type, int x, int y);
static void spawnEnemy(void);
static void spawnStronger(void);
//static void pause(void);
static void spawnPointsPowerup(void);
static void resetStage(void);

static SDL_Texture *targetterTexture;
static SDL_Texture *gridTexture[4];
static int          enemySpawnTimer;
static int          pointsSpawnTimer;
static int          gameOverTimer;
static int          scoreDisplay;

//enum GameState {
//	PLAYING,
//	PAUSED
//};

//enum GameState gameState;

void initStage(void)
{
	//gameState = PLAYING;
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	targetterTexture = loadTexture("gfx/targetter.png");
	gridTexture[0] = loadTexture("gfx/grid01.png");
	gridTexture[1] = loadTexture("gfx/grid02.png");
	gridTexture[2] = loadTexture("gfx/grid03.png");
	gridTexture[3] = loadTexture("gfx/grid04.png");

	resetStage();

	addPlayer();

	//pause();

	enemySpawnTimer = 0;
	pointsSpawnTimer = 0;
	scoreDisplay = 0;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(&app.mouse, 0, sizeof(Mouse));
}

static void resetStage(void)
{
	Entity *e;
	Effect *ef;

	while (stage.entityHead.next)
	{
		e = stage.entityHead.next;
		stage.entityHead.next = e->next;
		free(e);
	}

	while (stage.bulletHead.next)
	{
		e = stage.bulletHead.next;
		stage.bulletHead.next = e->next;
		free(e);
	}

	while (stage.effectHead.next)
	{
		ef = stage.effectHead.next;
		stage.effectHead.next = ef->next;
		free(ef);
	}

	memset(&stage, 0, sizeof(Stage));

	stage.entityTail = &stage.entityHead;
	stage.bulletTail = &stage.bulletHead;
	stage.effectTail = &stage.effectHead;

	gameOverTimer = FPS * 2;
}

//static void pause(void)
//{
//	int quit = 0;
//	while (!quit)
//	{
//		SDL_Event event;
//		while (SDL_PollEvent(&event)) {
//			switch (event.type) {
//			case SDL_QUIT:
//				quit = 1;
//				break;
//			case SDL_KEYDOWN:
//				if (event.key.keysym.sym == SDLK_SPACE) {
//					if (gameState == PLAYING)
//						gameState = PAUSED;
//					else
//						gameState = PLAYING;
//				}
//				break;
//			}
//		}
//	}
//}

static void logic(void)
{
	doPlayer();

	doEntities();

	doBullets();

	spawnEnemy();

	spawnStronger();

	spawnPointsPowerup();

	doEffects();

	doCamera();

	if (scoreDisplay < stage.score)
	{
		scoreDisplay++;
	}

	if (player == NULL && --gameOverTimer <= 0)
	{
		addHighscore(stage.score);

		initHighscores();
	}
}

static void spawnEnemy(void)
{
	int x, y;

	if (player != NULL && --enemySpawnTimer <= 0 && stage.score < 1000)
	{
		x = player->x + (rand() % SCREEN_WIDTH) - (rand() % SCREEN_WIDTH);
		y = player->y + (rand() % SCREEN_HEIGHT) - (rand() % SCREEN_HEIGHT);

		if (getDistance(x, y, player->x, player->y) > SCREEN_WIDTH / 2)
		{
			addEnemy(x, y);

			enemySpawnTimer = (FPS * 4) + rand() % FPS;
		}
	}
}

static void spawnStronger(void)
{
	int x, y;

	if (player != NULL && --enemySpawnTimer <= 0 && stage.score > 1000)
	{
		x = player->x + (rand() % SCREEN_WIDTH) - (rand() % SCREEN_WIDTH);
		y = player->y + (rand() % SCREEN_HEIGHT) - (rand() % SCREEN_HEIGHT);

		if (getDistance(x, y, player->x, player->y) > SCREEN_WIDTH / 2)
		{
			harder(x, y);

			enemySpawnTimer = (FPS * 2) + rand() % FPS;
		}
	}
}

static void spawnPointsPowerup(void)
{
	int x, y;

	if (--pointsSpawnTimer <= 0)
	{
		x = rand() % ARENA_WIDTH;
		y = rand() % ARENA_HEIGHT;

		addPointsPowerup(x, y);

		pointsSpawnTimer = (FPS * 3) + rand() % (FPS * 2);
	}
}

static void draw(void)
{
	drawGrid();

	drawEntities();

	drawBullets();

	drawEffects();

	drawHud();

	blit(targetterTexture, app.mouse.x, app.mouse.y, 1);
}

static void drawHud(void)
{
	drawText(10, 10, 255, 255, 255, TEXT_LEFT, "HEALTH:%d", player != NULL ? player->health : 0);

	drawText(250, 10, 255, 255, 255, TEXT_LEFT, "SCORE:%05d", scoreDisplay);

	drawWeaponInfo("PISTOL", WPN_PISTOL, 550, 10);

	drawWeaponInfo("UZI", WPN_UZI, 800, 10);

	drawWeaponInfo("SHOTGUN", WPN_SHOTGUN, 1050, 10);
}

static void drawWeaponInfo(char *name, int type, int x, int y)
{
	int r, g, b;

	if (player != NULL && player->weaponType == type)
	{
		r = b = 0;
		g = 255;
	}
	else
	{
		r = g = b = 255;
	}

	drawText(x, y, r, g, b, TEXT_LEFT, "%s:%03d", name, stage.ammo[type]);
}

static void drawGrid(void)
{
	int x1, x2, y1, y2, x, y, mx, my, n;

	x1 = (stage.camera.x % GRID_SIZE) * -1;
	x2 = x1 + GRID_RENDER_WIDTH * GRID_SIZE + (x1 == 0 ? 0 : GRID_SIZE);

	y1 = (stage.camera.y % GRID_SIZE) * -1;
	y2 = y1 + GRID_RENDER_HEIGHT * GRID_SIZE + (y1 == 0 ? 0 : GRID_SIZE);

	mx = stage.camera.x / GRID_SIZE;
	my = stage.camera.y / GRID_SIZE;

	n = 0;

	for (x = x1; x < x2; x += GRID_SIZE)
	{
		for (y = y1; y < y2; y += GRID_SIZE)
		{
			if (mx >= 0 && my >= 0 && mx <= (ARENA_WIDTH / GRID_SIZE) - 1 && my <= (ARENA_HEIGHT / GRID_SIZE) - 1)
			{
				n = ((mx ^ my) / 5) % 4;

				blit(gridTexture[n], x, y, 0);
			}

			my++;
		}

		my = stage.camera.y / GRID_SIZE;
		mx++;
	}
}
