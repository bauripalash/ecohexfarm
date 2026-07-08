#ifndef PB_UTILS_H
#define PB_UTILS_H

#include "raylib.h"
typedef struct HexVec HexVec;

#define ClampInt(val, min, max)                                                \
    ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

float CircleRadFromHexRad(float rad);
unsigned int ToHex(unsigned int r, unsigned int g, unsigned b);
unsigned int RGBtoRGBA(unsigned int rgb);
int PercentTo255(int p);
int PercentFrom255(int value);
Vector2 HexCordToPx(HexVec cord, float size);
#endif
