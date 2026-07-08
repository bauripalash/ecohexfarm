#include "utils.h"
#include <math.h>

float CircleRadFromHexRad(float rad) { return (sqrtf(3.0f) / 2.0f) * rad; }
unsigned int ToHex(unsigned int r, unsigned int g, unsigned b) {
    unsigned int result = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    return result;
}

unsigned int RGBtoRGBA(unsigned int rgb) {
    unsigned int result = (rgb << 8) | 0xff;
    return result;
}

int PercentTo255(int p) {
    int cp = ClampInt(p, 0, 100);
    return ((cp * 255) + 50) / 100;
}

int PercentFrom255(int value) {
    int cvalue = ClampInt(value, 0, 255);
    return ((cvalue * 100) + 127) / 255;
}
