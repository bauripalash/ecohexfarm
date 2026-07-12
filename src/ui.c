#include "ui.h"
#include "colors.h"
#include "external/raygui.h"
#include "raylib.h"
#include "screens.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

Vector2 getIconCenter(Rectangle rect, int scale) {
    float x = rect.x + (rect.width / 2.0f) - ((16 * scale) / 2.0f);
    float y = rect.y + (rect.height / 2.0f) - ((16 * scale) / 2.0f);

    return (Vector2){x, y};
}

bool PbIconButton(Rectangle rect, int icon) {
    Vector2 mpos = GetMousePosition();
    bool hover = CheckCollisionPointRec(mpos, rect);
    DrawRectangleRounded(
        rect, 0.5, 0, hover ? PbColorTGrayDarker : PbColorVGray
    );
    DrawRectangleRoundedLinesEx(rect, 0.5, 0, 2, PbColorVGrayLight);
    int scale = floorf(rect.height / 16.0f);
    Vector2 iconPosition = getIconCenter(rect, scale);
    GuiDrawIcon(
        icon, iconPosition.x, iconPosition.y, scale,
        hover ? PbColorVGreen : PbColorVWhite
    );
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool PbTextButton(Rectangle rect, const char *text, int fontSize) {
    Vector2 mpos = GetMousePosition();
    bool hover = CheckCollisionPointRec(mpos, rect);
    DrawRectangleRounded(
        rect, 0.5, 0, hover ? PbColorTGrayDarker : PbColorVGray
    );
    DrawRectangleRoundedLinesEx(rect, 0.5, 0, 2, PbColorVGrayLight);

    int textWidth = strlen(text) * fontSize;
    DrawTextEx(
        font, text,
        (Vector2){
            (rect.x + (rect.width / 2.0f)) - (textWidth / 2.0f),
            rect.y + (rect.height / 2.0f) - (fontSize / 2.0f)
        },
        fontSize, 0, hover ? PbColorVGreen : PbColorVWhite
    );
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
