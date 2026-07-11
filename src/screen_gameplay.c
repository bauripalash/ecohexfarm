#include "colors.h"
#include "config.h"
#include "draw.h"
#include "gameplay.h"
#include "raylib.h"
#include "screens.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>

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

HexTerrainTile TerrainTiles[TERRAIN_MAX_TILES];
int TerrainTileCount = 0;

HexNavTile NavTiles[NAV_MAX_TILES];
int NavTileCount = 0;

HexBug *HexBugs = NULL;
int HexBugID = 0;
int HexBugCount = 0;

HexFood *HexFoods = NULL;
int HexFoodCount = 0;
int HexFoodID = 0;

Vector2 SellingPenPosition = {0, 0};

int DraggingBugID = -1;

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
        int tile = getRandomNavTile();
        arrput(HexFoods, NewHexFood(NavTiles[tile].pos, tile));
        HexFoodCount++;
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
    SellingPenPosition = (Vector2){
        SCREEN_SIZE - SELLING_PEN_MARGIN - SELLING_PEN_RADIUS,
        SCREEN_SIZE - SELLING_PEN_MARGIN - SELLING_PEN_RADIUS
    };
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {

    /*
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    UpdateBugDragging();
    for (int i = 0; i < HexBugCount; i++) {
        BugWalkToTarget(&HexBugs[i], framesCounter);
    }
    if (framesCounter % 60 == 0) {
        SpawnRandomFoodAtRandom();
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

    // DrawCircleLinesV(
    //     (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, GARDEN_RADIUS,
    //     WHITE
    //);

    DrawHexFoodList();
    Rectangle buyBtnRect = {100, 5, 50, 50};
    if (PbIconButton(buyBtnRect, ICON_COIN)) {
        TraceLog(LOG_WARNING, "Buy");
    }

    DrawSellingPen();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
    arrfree(HexBugs);
    arrfree(HexFoods);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
