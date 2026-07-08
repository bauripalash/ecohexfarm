#ifndef PB_UTILS_H
#define PB_UTILS_H

#define ClampInt(val, min, max)                                                \
    ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

float CircleRadFromHexRad(float rad);
unsigned int ToHex(unsigned int r, unsigned int g, unsigned b);
unsigned int RGBtoRGBA(unsigned int rgb);
int PercentTo255(int p);
int PercentFrom255(int value);
#endif
