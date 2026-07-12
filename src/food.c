#include "external/stb_ds.h"
#include "gameplay.h"
#include "raylib.h"
#include "screens.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

HexFood NewHexFood(Vector2 position, int tile, HexFoodType ftype, int boost) {
    HexFood food = {
        .id = HexFoodID++,
        .index = HexFoodCount,
        .pos = position,
        .foodType = ftype,
        .boost = boost,
        .navTile = tile,
        .terrainTile = 0,
        .claimed = false,
        .claimedBy = -1
    };

    return food;
}

static HexFoodType getRandomFoodType(void) {
    return (HexFoodType)GetRandomValue(HEX_FOOD_REDDISH, HEX_FOOD_SUPER);
}

bool SpawnFoodAtRandom(HexFoodType ftype) {
    if (HexFoodCount >= MAX_FOODS) {
        return false;
    }

    int tileIndex = GetRandomValue(0, NavTileCount - 1);
    HexNavTile *tile = &NavTiles[tileIndex];

    if (tile->hasFood) {
        int i = 0;
        do {
            if (i >= NavTileCount) {
                return false;
            }
            tileIndex = GetRandomValue(0, NavTileCount - 1);
            tile = &NavTiles[tileIndex];
            i++;
        } while (tile->hasFood);
    }

    int boost = ftype == HEX_FOOD_SUPER ? FOOD_SUPER_BOOST : FOOD_MID_BOOST;
    arrput(HexFoods, NewHexFood(tile->pos, tileIndex, ftype, boost));
    HexFoodCount++;
    return true;
}

int PlaceShopFoodOrder(HexFoodType ftype, int count) {
    for (int i = 0; i < count; i++) {
        if (!SpawnFoodAtRandom(ftype)) return i + 1;
    }

    return count;
}

bool SpawnRandomFoodAtRandom(void) {
    if (HexFoodCount >= MAX_FOODS) {
        return false;
    }
    int tileIndex = GetRandomValue(0, NavTileCount - 1);
    HexNavTile *tile = &NavTiles[tileIndex];

    if (tile->hasFood) {
        int i = 0;
        do {
            if (i >= NavTileCount) {
                return false;
            }
            tileIndex = GetRandomValue(0, NavTileCount - 1);
            tile = &NavTiles[tileIndex];
        } while (tile->hasFood);
    }

    HexFoodType ftype;
    int boost = 0;

    if (GetRandomValue(1, 100) <= FOOD_MEDIUM_CHANCE) {
        ftype = getRandomFoodType();
        boost = FOOD_MID_BOOST;
    } else {
        ftype = getRandomFoodType();
        boost = FOOD_WEAK_BOOST;
    }

    arrput(HexFoods, NewHexFood(tile->pos, tileIndex, ftype, boost));
    HexFoodCount++;
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

bool EatFood(int index, HexBug *bug) {
    if (index < 0 || index >= HexFoodCount) return false;
    HexFood *food = &HexFoods[index];
    switch (food->foodType) {
        case HEX_FOOD_REDDISH: {
            bug->gene.red = ClampInt(
                bug->gene.red + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            break;
        }
        case HEX_FOOD_GREENISH: {
            bug->gene.green = ClampInt(
                bug->gene.green + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            break;
        }
        case HEX_FOOD_BLUEISH: {
            bug->gene.blue = ClampInt(
                bug->gene.blue + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            break;
        }
        case HEX_FOOD_SUPER: {
            bug->gene.red = ClampInt(
                bug->gene.red + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            bug->gene.green = ClampInt(
                bug->gene.green + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            bug->gene.blue = ClampInt(
                bug->gene.blue + food->boost, BUG_MIN_GENE_LIMIT,
                BUG_MAX_GENE_LIMIT
            );
            break;
        }

        default: break;
    }
    BugSyncGeneColor(bug);
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
        case HEX_FOOD_REDDISH: return REDDISH_FOOD;
        case HEX_FOOD_GREENISH: return GREENISH_FOOD;
        case HEX_FOOD_BLUEISH: return BLUEISH_FOOD;
        case HEX_FOOD_SUPER: return SUPER_FOOD;
        default: return BLACK;
    }
}

void DrawHexFoodList(void) {
    for (int i = 0; i < HexFoodCount; i++) {
        HexFood *food = &HexFoods[i];
        Color foodColor = getFoodColor(food);
        DrawPoly(food->pos, 6, DEFAULT_BUG_SIZE * 0.5f, 30.0f, foodColor);
        // DrawCircleLinesV(food->pos, FOOD_RADIUS, foodColor);
        // DrawTextEx(
        //     font, TextFormat("#%d : >[%d]", food->index, food->claimedBy),
        //     food->pos, 8, 0, WHITE
        //);
    }
}
