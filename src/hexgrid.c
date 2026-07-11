#include "draw.h"
#include "gameplay.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

int GenerateTerrainTiles(
    Vector2 center,
    HexTerrainTile *tiles,
    int gridRadius,
    float hexSize,
    Color bg,
    Color brdr
) {
    int hexIdx = 0;
    for (int q = -gridRadius; q <= gridRadius; q++) {
        for (int r = -gridRadius; r <= gridRadius; r++) {
            int s = -q - r;
            if (abs(s) <= gridRadius) {
                HexTerrainTile *tile = &tiles[hexIdx];
                tile->cord = (HexVec){q, r, s};
                Vector2 pxl = HexCordToPx(tile->cord, hexSize);
                tile->size = hexSize;
                tile->pos = (Vector2){pxl.x + center.x, pxl.y + center.y};
                tile->color = bg;
                tile->border = brdr;
                tile->colsnSize = CircleRadFromHexRad(hexSize);
                hexIdx++;
            }
        }
    }
    return hexIdx;
}

int GenerateNavTiles(
    Vector2 center, HexNavTile *tiles, int gridRadius, float hexSize
) {
    int tileCount = 0;
    for (int q = -gridRadius; q <= gridRadius; q++) {
        for (int r = -gridRadius; r <= gridRadius; r++) {
            int s = -q - r;
            if (abs(s) > gridRadius) continue;
            HexNavTile *tile = &tiles[tileCount];
            tile->cord = (HexVec){.q = q, .r = r, .s = s};

            Vector2 pxl = HexCordToPx(tile->cord, hexSize);
            tile->pos = (Vector2){pxl.x + center.x, pxl.y + center.y};
            tile->size = hexSize;
            // todo: colsnSize
            tile->hasFood = false;
            tile->walkable = true;
            tile->bugs = -1;
            for (int i = 0; i < 6; i++) {
                tile->neighbor[i] = -1;
            }

            tileCount++;
        }
    }

    return tileCount;
}

void BuildNavNeighbors(HexNavTile *tiles, int count) {
    for (int i = 0; i < count; i++) {
        for (int d = 0; d < 6; d++) {
            HexNavTile *tile = &tiles[i];
            HexVec target = {
                tile->cord.q + HexDirs[d].q, tile->cord.r + HexDirs[d].r
            };

            for (int j = 0; j < count; j++) {
                if (tiles[j].cord.q == target.q &&
                    tiles[j].cord.r == target.r) {
                    tile->neighbor[d] = j;
                }
            }
        }
    }
}

int GetBestNeighbor(int tile, int target) {
    int best = tile;
    int bestDist = INT_MAX;
    for (int i = 0; i < 6; i++) {
        int tn = NavTiles[tile].neighbor[i];
        if (tn == -1) {
            continue;
        }

        int dist = HexVecDistance(NavTiles[tn].cord, NavTiles[target].cord);

        if (dist < bestDist) {
            best = tn;
            bestDist = dist;
        }
    }
    return best;
}

int FindNearestNavTile(Vector2 pos) {
    int best = -1;
    float bestDist = FLT_MAX;

    for (int i = 0; i < NavTileCount; i++) {
        float dist = Vector2DistanceSqr(pos, NavTiles[i].pos);
        if (dist < bestDist) {
            best = i;
            bestDist = dist;
        }
    }
    return best;
}

void DrawTerrainTiles(HexTerrainTile *tiles, int count, float thickness) {
    for (int i = 0; i < count; i++) {
        HexTerrainTile tile = tiles[i];
        PBDrawHexagon(tile.pos, tile.size, thickness, tile.color, tile.border);
        // DrawTextEx(font, TextFormat("%d", i), tile.pos, 16, 0.0f,
        // tile.border);
    }
}

void DrawNavTiles(HexNavTile *tiles, int count, float thickness) {
    for (int i = 0; i < count; i++) {
        HexNavTile tile = tiles[i];
        PBDrawHexagonLine(
            tile.pos, tile.size, thickness, (Color){0xff, 0xff, 0xff, 10}
        );
        // DrawTextEx(font, TextFormat("%d", i), tile.pos, 16, 0.0f,
        // tile.border);
    }
}
