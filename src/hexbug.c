#include "hexbug.h"
#include "config.h"
#include "draw.h"
#include "raylib.h"
#include "utils.h"
#include <stdbool.h>

int GeneColorRGBA(HexGene gene) { return 1; }

HexBug NewGenesisBug(bool primary) {
    return primary ? NewHexBug(ToHex(0xff, 0xff, 0xff))
                   : NewHexBug(
                         ToHex(BUG_BASE_HEALTH, BUG_BASE_SPEED, BUG_BASE_RANGE)
                     );
}

HexBug NewHexBug(int color) {
    Color geneticColor = GetColor(RGBtoRGBA(color));
    return (HexBug){
        .pos = (Vector2){SCREEN_SIZE / 2.0f, SCREEN_SIZE / 2.0f},
        .size = DEFAULT_BUG_SIZE,
        .gene = (HexGene){
            .red = geneticColor.r,
            .green = geneticColor.g,
            .blue = geneticColor.b,
            .hGene = color,
            .hColor = geneticColor
        },
    };
}

void DrawHexBug(HexBug *bug) {
    PBDrawHexagon(
        bug->pos, bug->size, DEFAULT_BUG_THICK, bug->gene.hColor,
        bug->gene.hColor
    );
}
