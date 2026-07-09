#ifndef PB_HEXGRID_H
#define PB_HEXGRID_H

#include "raylib.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_HEX_SIZE 20.0f

typedef struct HexVec {
    int q;
    int r;
    int s;
} HexVec;

static const HexVec HexDirs[6] = {{1, 0, 0},  {1, -1, 0}, {0, -1, 0},
                                  {-1, 0, 0}, {-1, 1, 0}, {0, 1, 0}};

static inline int HexVecDistance(HexVec a, HexVec b) {
    return Max3(abs(a.q - b.q), abs(a.r - b.r), abs(a.s - b.s));
}

typedef struct HexMapTile {
    HexVec cord;
    int size;
    int colsnSize;
    Vector2 pos;
    Color color;
    Color border;
} HexMapTile;

typedef struct HexNavTile {
    HexVec cord;
    int size;
    Vector2 pos;
    bool walkable;
    bool hasFood;
    int bugs;
    int neighbor[6];
} HexNavTile;

#define VecToHexVec(vec)  ((HexVec){.q = vec.x, .r = vec.y})
#define HexVecToVec(hvec) ((Vector2){.x = hvec.q, .y = hvec.r})

int FillHexGrid(
    Vector2 center,
    HexMapTile *tiles,
    int gridRadius,
    float hexSize,
    Color bg,
    Color brdr
);

int GenerateNavTiles(
    Vector2 center, HexNavTile *tiles, int gridRadius, float hexSize
);

int GetBestNeighbor(int tile, int target);
void BuildNavNeighbors(HexNavTile *tiles, int count);
void DrawHexGrid(HexMapTile *tiles, int count, float thickness);
void DrawNavTiles(HexNavTile *tiles, int count, float thickness);
#endif
