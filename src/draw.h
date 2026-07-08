#ifndef PB_DRAW_H
#define PB_DRAW_H

#include "raylib.h"

// Bresenham's line algorithm
// based on https://zingl.github.io/bresenham.html
void PBDrawLine(Vector2 a, Vector2 b, int thickness, Color clr);

// Draw custom outlined filled hexagon
void PBDrawHexagon(
    Vector2 center, float r, int thickness, Color fill, Color outline
);

// Draw custom line only hexagon
void PBDrawHexagonLine(Vector2 center, float r, float thickness, Color color);

// Draw custom filled hexagon
void PBDrawHexagonFilled(Vector2 center, float r, Color color);
#endif
