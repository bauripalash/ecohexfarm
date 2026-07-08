#ifndef PB_HEXGRID_H
#define PB_HEXGRID_H

#include "raylib.h"

#define DEFAULT_HEX_SIZE 20.0f

typedef struct HexVec {
    int q;
    int r;
} HexVec;

typedef struct HexMapTile {
    HexVec cord;
    int s;
    int size;
    int colsnSize;
    Vector2 pos;
    Color color;
    Color border;
} HexMapTile;

#define VecToHexVec(vec)  ((HexVec){.q = vec.x, .r = vec.y})
#define HexVecToVec(hvec) ((Vector2){.x = hvec.q, .y = hvec.r})

Vector2 HexCordToPx(HexVec cord, float size);
void DrawHexShape(Vector2 pos, float size, Color color);
void DrawHexShapeLine(Vector2 pos, float size, float thickness, Color color);
void DrawHexShapeBordered(
    Vector2 pos, float size, float thickness, Color bg, Color border
);
void FillHexGrid(
    Vector2 center,
    HexMapTile *tiles,
    int *count,
    int gridRadius,
    float hexSize,
    Color bg,
    Color brdr
);

void DrawHexGrid(HexMapTile *tiles, int count, float thickness);

#endif
