#include "colors.h"
#include "config.h"
#include "hexbug.h"
#include "hexgrid.h"
#include "raylib.h"
#include "screens.h"

#define MAP_RADIUS      2
#define TILE_RADIUS     82.0f
#define MAX_TILES       100
#define BG_COLOR        PbColorTGrayDarker
#define TILE_BG_COLOR   PbColorTGrayDarker
#define TILE_BRDR_COLOR PbColorTGrayLightest
#define NAV_TILE_SIZE   30

static int framesCounter = 0;
static int finishScreen = 0;
static Color navTileBorder = {255, 100, 100, 20};

HexMapTile tiles[MAX_TILES];
static int tileCount = 0;

typedef struct NavTile {
    Rectangle bounds;
} NavTile;

#define MAX_NAV_TILES WIN_SIZE
NavTile navTiles[MAX_NAV_TILES + 20];
static int navTileCount = 0;

#define MAX_BUGS 50
HexBug bugs[MAX_BUGS];
static int bugCount = 0;

static void drawNavTiles(void) {
    for (int i = 0; i < navTileCount; i++) {
        NavTile tile = navTiles[i];
        DrawRectangleLinesEx(tile.bounds, 1, navTileBorder);
    }
}

static void drawBackground(void) { DrawHexGrid(tiles, tileCount, 1); }

static void initNavTileGrid(void) {
    int nTileCount = 0;
    Vector2 pos = {0, 0};
    while (pos.y <= WIN_SIZE) {
        while (pos.x <= WIN_SIZE) {
            navTiles[nTileCount].bounds =
                (Rectangle){pos.x, pos.y, NAV_TILE_SIZE, NAV_TILE_SIZE};
            nTileCount++;
            pos.x += NAV_TILE_SIZE;
        }
        pos.x = 0;
        pos.y += NAV_TILE_SIZE;
    }
    navTileCount = nTileCount;
}

static void initFirstBugs(void) {
    bugCount = 0;
    for (int i = 0; i < 4; i++) {
        bugs[i] = NewHexBug(PbHexColorTPink);
        bugCount++;
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
    FillHexGrid(
        (Vector2){WIN_SIZE / 2.0f, WIN_SIZE / 2.0f}, tiles, &tileCount,
        MAP_RADIUS, DEFAULT_HEX_SIZE, TILE_BG_COLOR, TILE_BRDR_COLOR
    );
    initNavTileGrid();
    initFirstBugs();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {

    /*
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    framesCounter++;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
    DrawRectangle(0, 0, WIN_SIZE, WIN_SIZE, BG_COLOR);
    drawBackground();

    for (int i = 0; i < bugCount; i++) {
        DrawHexBug(&bugs[i]);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
