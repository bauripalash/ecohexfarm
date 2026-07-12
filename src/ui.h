#ifndef PB_UI_H
#define PB_UI_H

#include "colors.h"
#include "raylib.h"
#include <stdbool.h>

#define BUTTON_FG_COLOR

bool PbIconButton(Rectangle rect, int icon);
bool PbTextButton(Rectangle rect, const char *text, int fontSize);
#endif
