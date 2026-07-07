#include "colors.h"
#include "config.h"
#include "hexbug.h"
#include "raylib.h"
#include "screens.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>

#define MAP_RADIUS      2
#define TILE_RADIUS     82.0f
#define MAX_TILES       100
#define BG_COLOR        PbColorTGrayDarker
#define TILE_FG_COLOR   PbColorTGrayDarker
#define TILE_BRDR_COLOR PbColorTGrayLightest
#define NAV_TILE_SIZE   30

static int framesCounter = 0;
static int finishScreen = 0;
static Color navTileBorder = {255, 100, 100, 20};

typedef struct MapTile {
    int q, r, s;
    int circleRad;
    Vector2 screenPos;
    Color color;
} MapTile;

MapTile tiles[MAX_TILES];
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

static Vector2 hexToPx(int q, int r) {
    float x = TILE_RADIUS * (3.0f / 2.0f * q);
    float y = TILE_RADIUS * (sqrtf(3.0f) / 2.0f * q + sqrtf(3.0f) * r);
    return (Vector2){x + WIN_SIZE / 2.0f, y + WIN_SIZE / 2.0f};
}

static void drawHexTiles(void) {
    for (int i = 0; i < tileCount; i++) {
        MapTile tile = tiles[i];
        DrawPoly(tile.screenPos, 6, TILE_RADIUS, 0.0f, tile.color);
        Color brdr = TILE_BRDR_COLOR;
        float brdrThickness = 1.0f;
        if (i == 5) {
            brdr = PbColorTPinkLight;
            brdrThickness = 2.0f;
        }
        DrawPolyLinesEx(
            tile.screenPos, 6, TILE_RADIUS, 0.0f, brdrThickness, brdr
        );
        // DrawCircleLines(tile.screenPos.x, tile.screenPos.y, tile.circleRad,
        // WHITE);
        DrawTextEx(font, TextFormat("%d", i), tile.screenPos, 16, 1, brdr);
    }
}

static void drawNavTiles(void) {
    for (int i = 0; i < navTileCount; i++) {
        NavTile tile = navTiles[i];
        DrawRectangleLinesEx(tile.bounds, 1, navTileBorder);
    }
}

static void drawBackground(void) {
    drawHexTiles();

    drawNavTiles();
}

static void initNavTileGrid(void) {
    int nTileCount = 0;
    Vector2 pos = {0, 0};
    while (pos.y <= WIN_SIZE) {
        while (pos.x <= WIN_SIZE) {

            TraceLog(LOG_WARNING, "->%d", nTileCount);
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

static void initTileGrid(void) {
    tileCount = 0;
    for (int q = -MAP_RADIUS; q <= MAP_RADIUS; q++) {
        for (int r = -MAP_RADIUS; r <= MAP_RADIUS; r++) {
            int s = -q - r;
            if (abs(s) <= MAP_RADIUS) {
                tiles[tileCount].q = q;
                tiles[tileCount].r = r;
                tiles[tileCount].s = s;
                tiles[tileCount].screenPos = hexToPx(q, r);
                tiles[tileCount].color = TILE_FG_COLOR;
                tiles[tileCount].circleRad = CircleRadFromHexRad(TILE_RADIUS);
                tileCount++;
            }
        }
    }
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
    initTileGrid();
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
