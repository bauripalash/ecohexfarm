#ifndef PB_HEXBUG_H
#define PB_HEXBUG_H

#include "raylib.h"
#include <stdbool.h>

#define DEFAULT_BUG_SIZE  20
#define DEFAULT_BUG_THICK 5

#define BUG_MAX_HEALTH    100
#define BUG_MIN_HEALTH    1

#define BUG_MIN_RANGE     DEFAULT_BUG_SIZE

#define BUG_MAX_SPEED     100
#define BUG_MIN_SPEED     1

#define BUG_BASE_RANGE    DEFAULT_BUG_SIZE * 2
#define BUG_BASE_HEALTH   50
#define BUG_BASE_SPEED    50

typedef struct HexGene {
    // Health
    int red;
    // Speed
    int green;
    // Range
    int blue;
    // hex color
    int hGene;
    Color hColor;
} HexGene;

typedef struct HexBug {
    int id;
    int size;
    Vector2 pos;
    HexGene gene;
} HexBug;

HexBug NewGenesisBug(bool primary);
HexBug NewHexBug(int color);
void DrawHexBug(HexBug *bug);

#endif
