#ifndef PB_HEXBUG_H
#define PB_HEXBUG_H

#include "raylib.h"

#define DEFAULT_BUG_SIZE  20
#define DEFAULT_BUG_THICK 5

typedef struct HexBug {
    Vector2 pos;
    int size;
    int health;
    int speed;
    int range;
    int hxColor;
    Color color;
} HexBug;

HexBug NewHexBug(int color);
void DrawHexBug(HexBug *bug);

#endif
