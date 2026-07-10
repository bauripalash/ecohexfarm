#include "colors.h"
#include "config.h"
#include "gameplay.h"
#include "raylib.h"
#include "screens.h"
#include "ui.h"

#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#include <stdbool.h>

#define BG_COLOR        PbColorVGray
#define TILE_BG_COLOR   PbColorVGray
#define TILE_BRDR_COLOR PbColorVGrayLight

static int framesCounter = 0;
static int finishScreen = 0;
static Color navTileBorder = {255, 100, 100, 20};

HexTerrainTile TerrainTiles[TERRAIN_MAX_TILES];
int TerrainTileCount = 0;

HexNavTile NavTiles[NAV_MAX_TILES];
int NavTileCount = 0;

HexBug *HexBugs;
int HexBugID = 0;
int HexBugCount = 0;

HexFood HexFoods[MAX_FOODS];
int HexFoodCount = 0;

static void drawBackground(void) {
    DrawTerrainTiles(TerrainTiles, TerrainTileCount, 1);
    if (DEBUG_NAV_TILES) {
        DrawNavTiles(NavTiles, NavTileCount, 1);
    }
}

static void initFirstBugs(void) {
    for (int i = 0; i < INIT_BUGS; i++) {
        arrput(HexBugs, NewGenesisBug(i == 0, i));
        HexBugCount++;
    }
}

static int getRandomNavTile(void) {
    int result = GetRandomValue(0, NavTileCount - 1);
    return result;
}

static void initFirstFoods(void) {
    for (int i = 0; i < INIT_FOODS; i++) {
        HexFoods[HexFoodCount++] = NewHexFood(NavTiles[getRandomNavTile()].pos);
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
    TerrainTileCount = GenerateTerrainTiles(
        (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, TerrainTiles,
        TERRAIN_MAP_RADIUS, TERRAIN_TILE_SIZE, TILE_BG_COLOR, TILE_BRDR_COLOR
    );
    NavTileCount = GenerateNavTiles(
        (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, NavTiles,
        NAV_MAP_RADIUS, NAV_TILE_SIZE
    );
    BuildNavNeighbors(NavTiles, NavTileCount);
    initFirstBugs();
    initFirstFoods();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {

    /*
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    for (int i = 0; i < HexBugCount; i++) {
        BugWalkToTarget(&HexBugs[i], framesCounter);
    }
    framesCounter++;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
    DrawRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, BG_COLOR);
    drawBackground();

    for (int i = 0; i < HexBugCount; i++) {
        DrawHexBug(&HexBugs[i]);
    }

    DrawCircleLinesV(
        (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, GARDEN_RADIUS,
        WHITE
    );

    DrawHexFoodList();
    Rectangle buyBtnRect = {100, 5, 50, 50};
    if (PbIconButton(buyBtnRect, ICON_COIN)) {
        TraceLog(LOG_WARNING, "Buy");
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
    arrfree(HexBugs);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
