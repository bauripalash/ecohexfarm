// clang-format off
#include "colors.h"
#include "config.h"
#include "hexbug.h"
#include "hexgrid.h"
#include "raylib.h"
#include "screens.h"
#include "gameplay.h"
#include "ui.h"

#include <stdbool.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
// clang-format on

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

HexBug HexBugs[MAX_BUGS];
int HexBugID = 0;
int HexBugCount = 0;

static void drawBackground(void) {
    DrawTerrainTiles(TerrainTiles, TerrainTileCount, 1);
    if (DEBUG_NAV_TILES) {
        DrawNavTiles(NavTiles, NavTileCount, 1);
    }
}

static void initFirstBugs(void) {
    HexBugs[HexBugCount++] = NewGenesisBug(true, 0);
    for (int i = 1; i < INIT_BUGS; i++) {
        HexBugs[HexBugCount++] = NewGenesisBug(false, i);
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
    TerrainTileCount = GenerateTerrainTiles(
        (Vector2){SCREEN_SIZE / 2.0f, SCREEN_SIZE / 2.0f}, TerrainTiles,
        TERRAIN_MAP_RADIUS, TERRAIN_TILE_SIZE, TILE_BG_COLOR, TILE_BRDR_COLOR
    );
    NavTileCount = GenerateNavTiles(
        (Vector2){SCREEN_SIZE / 2.0f, SCREEN_SIZE / 2.0f}, NavTiles,
        NAV_MAP_RADIUS, NAV_TILE_SIZE
    );
    BuildNavNeighbors(NavTiles, NavTileCount);
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
    for (int i = 0; i < HexBugCount; i++) {
        BugWalkToTarget(&HexBugs[i]);
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

    Rectangle buyBtnRect = {100, 5, 50, 50};
    if (PbIconButton(buyBtnRect, ICON_COIN)) {
        TraceLog(LOG_WARNING, "Buy");
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
