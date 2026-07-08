#include "hexgrid.h"
#include "raylib.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>

Vector2 HexCordToPx(HexVec cord, float size) {
    float x = size * (3.0f / 2.0f * cord.q);
    float y = size * (sqrtf(3.0f) / 2.0f * cord.q + sqrtf(3.0f) * cord.r);
    return (Vector2){x, y};
}

void DrawHexShape(Vector2 pos, float size, Color color) {
    DrawPoly(pos, 6, size, 0.0f, color);
}

void DrawHexShapeLine(Vector2 pos, float size, float thickness, Color color) {
    DrawPolyLinesEx(pos, 6, size, 0.0f, thickness, color);
}

void DrawHexShapeBordered(
    Vector2 pos, float size, float thickness, Color bg, Color border
) {
    DrawHexShape(pos, size, bg);
    DrawHexShapeLine(pos, size, thickness, border);
}

void FillHexGrid(
    Vector2 center,
    HexMapTile *tiles,
    int *count,
    int gridRadius,
    float hexSize,
    Color bg,
    Color brdr
) {
    int hexIdx = 0;
    Vector2 og = {center.x, center.y};
    for (int q = -gridRadius; q <= gridRadius; q++) {
        for (int r = -gridRadius; r <= gridRadius; r++) {
            int s = -q - r;
            if (abs(s) <= gridRadius) {
                HexMapTile *tile = &tiles[hexIdx];
                tile->cord = (HexVec){q, r};
                Vector2 pxl = HexCordToPx(tile->cord, hexSize);
                tile->size = hexSize;
                tile->pos = (Vector2){pxl.x + center.x, pxl.y + center.y};
                tile->color = bg;
                tile->border = brdr;
                tile->s = s;
                tile->colsnSize = CircleRadFromHexRad(hexSize);
                hexIdx++;
            }
        }
    }
    *count = hexIdx + 1;
}

void DrawHexGrid(HexMapTile *tiles, int count, float thickness) {
    for (int i = 0; i < count; i++) {
        HexMapTile tile = tiles[i];
        DrawHexShapeBordered(
            tile.pos, tile.size, thickness, tile.color, tile.border
        );
    }
}
