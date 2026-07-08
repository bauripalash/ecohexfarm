#include "draw.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

void PBDrawLine(Vector2 a, Vector2 b, int thickness, Color clr) {
    int x0 = (int)a.x;
    int y0 = (int)a.y;

    int x1 = (int)b.x;
    int y1 = (int)b.y;

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2 = 0;

    while (true) {
        DrawRectangle(x0, y0, thickness, thickness, clr);
        e2 = 2 * err;

        if (x0 == x1 && y0 == y1) {
            break;
        }

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void PBDrawHexagonEx(
    Vector2 center,
    float r,
    bool filled,
    bool outlined,
    int thickness,
    Color fill,
    Color outline
) {
    Vector2 corners[6];
    for (int i = 0; i < 6; i++) {
        float angle = 60.0f * i;
        float angleRad = DEG2RAD * -angle;
        corners[i] = (Vector2){
            center.x + r * cosf(angleRad), center.y + r * sinf(angleRad)
        };
    }

    if (filled) {
        for (int i = 0; i < 6; i++) {
            DrawTriangle(center, corners[i], corners[(i + 1) % 6], fill);
        }
    }

    if (outlined) {
        for (int i = 0; i < 6; i++) {
            PBDrawLine(corners[i], corners[(i + 1) % 6], thickness, outline);
        }
    }
}

void PBDrawHexagon(
    Vector2 center, float r, int thickness, Color fill, Color outline
) {
    PBDrawHexagonEx(center, r, true, true, thickness, fill, outline);
}

void PBDrawHexagonFilled(Vector2 center, float r, Color color) {
    PBDrawHexagonEx(center, r, true, false, 1, color, BLANK);
}

void PBDrawHexagonLine(Vector2 center, float r, float thickness, Color color) {
    PBDrawHexagonEx(center, r, false, true, thickness, BLANK, color);
}
