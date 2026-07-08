#include "hexgrid.h"
#include "draw.h"
#include "raylib.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

int FillHexGrid(
    Vector2 center,
    HexMapTile *tiles,
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
                HexMapTile *tile = &tiles[hexIdx];
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

void DrawHexGrid(HexMapTile *tiles, int count, float thickness) {
    for (int i = 0; i < count; i++) {
        HexMapTile tile = tiles[i];
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
