#include "external/stb_ds.h"
#include "gameplay.h"
#include "raylib.h"
#include "screens.h"
#include <stdbool.h>

HexFood NewHexFood(Vector2 position, int tile) {
    HexFood food = {
        .id = HexFoodID++,
        .index = HexFoodCount,
        .pos = position,
        .foodType = HEX_FOOD_REDDISH,
        .navTile = tile,
        .terrainTile = 0,
        .claimed = false,
        .claimedBy = -1
    };

    return food;
}

bool SpawnRandomFoodAtRandom(void) {
    if (HexFoodCount >= MAX_FOODS) {
        return false;
    }
    int tileIndex = GetRandomValue(0, NavTileCount - 1);
    HexNavTile *tile = &NavTiles[tileIndex];
    if (!tile->hasFood) {
        arrput(HexFoods, NewHexFood(tile->pos, tileIndex));
        HexFoodCount++;
        return true;
    }

    return true;
}

void RemoveHexFood(int index) {
    if (index < 0 || index >= HexFoodCount) {
        return;
    }

    arrdel(HexFoods, index);
    HexFoodCount--;
    UpdateHexFoodList();
}

int FindFoodByID(int id) {
    for (int i = 0; i < HexFoodCount; i++) {
        if (HexFoods[i].id == id) {
            return i;
        }
    }

    return -1;
}

void ReleaseClaimFood(int id, int bugId) {
    int index = FindFoodByID(id);
    if (index == -1) {
        return;
    }
    HexFood *food = &HexFoods[index];
    if (food->claimedBy == bugId) {
        food->claimedBy = -1;
        food->claimed = false;
    }
}

bool EatFood(int index) {
    if (index < 0 || index >= HexFoodCount) return false;
    RemoveHexFood(index);
    return true;
}

void UpdateHexFoodList(void) {
    for (int i = 0; i < HexFoodCount; i++) {
        HexFood *food = &HexFoods[i];
        food->index = i;
    }
}

static Color getFoodColor(HexFood *food) {
    switch (food->foodType) {
        case HEX_FOOD_REDDISH: return RED;
        case HEX_FOOD_GREENISH: return GREEN;
        case HEX_FOOD_BLUEISH: return BLUE;
        case HEX_FOOD_SUPER: return WHITE;
        default: return BLACK;
    }
}

void DrawHexFoodList(void) {
    for (int i = 0; i < HexFoodCount; i++) {
        HexFood *food = &HexFoods[i];
        Color foodColor = getFoodColor(food);
        DrawPoly(food->pos, 6, DEFAULT_BUG_SIZE, 30.0f, foodColor);
        DrawCircleLinesV(food->pos, FOOD_RADIUS, foodColor);
        DrawTextEx(
            font, TextFormat("#%d : >[%d]", food->index, food->claimedBy),
            food->pos, 8, 0, WHITE
        );
    }
}
