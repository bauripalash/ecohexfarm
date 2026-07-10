#include "gameplay.h"
#include "raylib.h"

HexFood NewHexFood(Vector2 position) {
    HexFood food = {
        .pos = position,
        .foodType = HEX_FOOD_REDDISH,
        .navTile = 0,
        .terrainTile = 0,
    };

    return food;
}
void UpdateHexFoodList(void) {}

void DrawHexFoodList(void) {
    for (int i = 0; i < HexFoodCount; i++) {
        HexFood *food = &HexFoods[i];
        DrawCircleV(food->pos, 12, RED);
    }
}
