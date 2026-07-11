#ifndef PB_COLORS_H
#define PB_COLORS_H

// Convert RGB value to Color with Alpha of 0xff
#define PbSolidColor(r, g, b) (Color){r, g, b, 0xff}

// Convert RGB Color to RGBA Color with Alpha of 0xff
#define PbHexColor(x) (int)0x##x##ff

/*
 * Twilight 5 Palette by Star
 * https://lospec.com/palette-list/twilight-5
 */

#define PbHexColorTPinkLight    PbHexColor(fbbbad)
#define PbHexColorTPink         PbHexColor(ee8695)
#define PbHexColorTGrayLightest PbHexColor(4a7a96)
#define PbHexColorTGrayLighter  PbHexColor(333f58)
#define PbHexColorTGrayDarker   PbHexColor(292831)

#define PbColorTPinkLight       PbSolidColor(0xfb, 0xbb, 0xad)
#define PbColorTPink            PbSolidColor(0xee, 0x86, 0x95)
#define PbColorTGrayLightest    PbSolidColor(0x4a, 0x7a, 0x96)
#define PbColorTGrayLighter     PbSolidColor(0x33, 0x3f, 0x58)
#define PbColorTGrayDarker      PbSolidColor(0x29, 0x28, 0x31)

// Velstad-4-by-mainzero Palette by Arkzero87
// https://lospec.com/palette-list/velstad-4-by-mainzero

#define PbHexColorVGray      PbHexColor(131726)
#define PbHexColorVGrayLight PbHexColor(59608e)
#define PbHexColorVGreen     PbHexColor(61b8ae)
#define PbHexColorVWhite     PbHexColor(dde0bd)

#define PbColorVGray         PbSolidColor(0x13, 0x17, 0x26)
#define PbColorVGrayLight    PbSolidColor(0x59, 0x60, 0x8e)
#define PbColorVGreen        PbSolidColor(0x61, 0xb8, 0xae)
#define PbColorVWhite        PbSolidColor(0xdd, 0xe0, 0xbd)

#endif
