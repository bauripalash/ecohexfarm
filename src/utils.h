#ifndef PB_UTILS_H
#define PB_UTILS_H

#include "raylib.h"
typedef struct HexVec HexVec;

#define ClampInt(val, min, max)                                                \
    ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

static inline int Max3(int a, int b, int c) {
    int r = a;
    if (b > r) r = b;
    if (c > r) r = c;
    return r;
}

static inline Color GetOppositeColor(Color c) {
    float brightness = (c.r * 0.299f) + (c.g * 0.587f) + (c.b * 0.114f);

    if (brightness > 128.0f) {
        return BLACK;
    } else {
        return WHITE;
    }
}

float CircleRadFromHexRad(float rad);
unsigned int ToHex(unsigned int r, unsigned int g, unsigned b);
unsigned int RGBtoRGBA(unsigned int rgb);
int PercentTo255(int p);
int PercentFrom255(int value);
Vector2 HexCordToPx(HexVec cord, float size);
#endif
