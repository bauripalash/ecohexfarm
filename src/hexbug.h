#ifndef PB_HEXBUG_H
#define PB_HEXBUG_H

#include "raylib.h"
#include <stdbool.h>

#define DEFAULT_BUG_SIZE   18
#define DEFAULT_BUG_THICK  1

#define BUG_MAX_GENE_LIMIT 255
#define BUG_MIN_GENE_LIMIT 0

#define BUG_MAX_HEALTH     100
#define BUG_MIN_HEALTH     1

#define BUG_MAX_SPEED      100
#define BUG_MIN_SPEED      1

#define BUG_MIN_RANGE      DEFAULT_BUG_SIZE
#define BUG_MAX_RANGE      DEFAULT_BUG_SIZE * 10

#define BUG_BASE_SPEED     127
#define BUG_BASE_HEALTH    127
#define BUG_BASE_RANGE     DEFAULT_BUG_SIZE * 2

#define BUG_TXT_SCALE      1.2f

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
    int tile;
    int nextTile;
    int target;
    int size;
    Vector2 pos;
    HexGene gene;
    bool moving;
    bool hasFellow;
    float colsnRadius;
    float faceAngle;
    Rectangle txtRect;
    Vector2 txtOrigin;
} HexBug;

HexBug NewGenesisBug(bool primary, int tile);
HexBug NewHexBug(int color);
void DrawHexBug(HexBug *bug);
void BugWalkToTarget(HexBug *bug);
#endif
