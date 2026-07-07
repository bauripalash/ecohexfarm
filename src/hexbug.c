#include "hexbug.h"
#include "config.h"
#include "raylib.h"

HexBug NewHexBug(int color) {

    return (HexBug){
        .pos = (Vector2){WIN_SIZE / 2.0f, WIN_SIZE / 2.0f},
        .size = DEFAULT_BUG_SIZE,
        .health = 0,
        .range = 0,
        .speed = 1,
        .hxColor = color,
        .color = GetColor(color)
    };
}

void DrawHexBug(HexBug *bug) {
    DrawPolyLinesEx(bug->pos, 6, bug->size, 0, DEFAULT_BUG_THICK, bug->color);
}
