#include "colors.h"
#include "draw.h"
#include "external/raygui.h"
#include "gameplay.h"
#include "raylib.h"

void DrawSellingPen(void) {
    int og = GuiGetStyle(LABEL, TEXT_COLOR_NORMAL);

    PBDrawHexagon(
        SellingPenPosition, SELLING_PEN_RADIUS, SELLING_PEN_THICKNESS, BLANK,
        PbColorVWhite
    );
    const char *sellingBugLabel = GuiIconText(ICON_HOUSE, "Selling Hex");

    Rectangle labelRect = {
        SellingPenPosition.x - SELLING_PEN_RADIUS + 7,
        SellingPenPosition.y + SELLING_PEN_RADIUS, 200, 10
    };

    GuiLabel(labelRect, sellingBugLabel);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, og);
}
