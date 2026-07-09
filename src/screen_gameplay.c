// clang-format off
#include "colors.h"
#include "config.h"
#include "hexbug.h"
#include "hexgrid.h"
#include "raylib.h"
#include "screens.h"

#include "gameplay.h"
#include <stdbool.h>
// clang-format on

#define BG_COLOR        PbColorVGray
#define TILE_BG_COLOR   PbColorVGray
#define TILE_BRDR_COLOR PbColorVGrayLight

static int framesCounter = 0;
static int finishScreen = 0;
static Color navTileBorder = {255, 100, 100, 20};

HexMapTile MapTiles[MAX_TILES];
int MapTileCount = 0;

HexNavTile NavTiles[MAX_NAV_TILES];
int NavTileCount = 0;

HexBug HexBugs[MAX_BUGS];
int HexBugCount = 0;

static void drawBackground(void) {
    DrawHexGrid(MapTiles, MapTileCount, 1);
    DrawNavTiles(NavTiles, NavTileCount, 1);
}

static void initFirstBugs(void) {
    HexBugs[0] = NewGenesisBug(true, 0);

    HexBugs[1] = NewGenesisBug(false, 1);

    HexBugCount = 2;
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
    MapTileCount = FillHexGrid(
        (Vector2){SCREEN_SIZE / 2.0f, SCREEN_SIZE / 2.0f}, MapTiles, MAP_RADIUS,
        DEFAULT_HEX_SIZE, TILE_BG_COLOR, TILE_BRDR_COLOR
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
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
