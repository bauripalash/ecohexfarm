#include "ui.h"
#include "colors.h"
#include "external/raygui.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>

Vector2 getIconCenter(Rectangle rect, int scale) {
    float x = rect.x + (rect.width / 2.0f) - ((16 * scale) / 2.0f);
    float y = rect.y + (rect.height / 2.0f) - ((16 * scale) / 2.0f);

    return (Vector2){x, y};
}

bool PbIconButton(Rectangle rect, int icon) {
    DrawRectangleRounded(rect, 0.5, 0, PbColorVGray);
    DrawRectangleRoundedLinesEx(rect, 0.5, 0, 2, PbColorVGrayLight);
    int scale = floorf(rect.height / 16.0f);
    Vector2 iconPosition = getIconCenter(rect, scale);
    GuiDrawIcon(icon, iconPosition.x, iconPosition.y, scale, PbColorVWhite);
    return CheckCollisionPointRec(GetMousePosition(), rect) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
