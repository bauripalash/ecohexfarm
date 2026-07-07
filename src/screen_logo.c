#include "config.h"
#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int logoPositionX = 0;
static int logoPositionY = 0;

//----------------------------------------------------------------------------------
// Logo Screen Functions Definition
//----------------------------------------------------------------------------------

// Logo Screen Initialization logic
void InitLogoScreen(void) {
    finishScreen = 0;
    framesCounter = 0;

    logoPositionX = GetScreenWidth() / 2 - 128;
    logoPositionY = GetScreenHeight() / 2 - 128;
}

// Logo Screen Update logic
void UpdateLogoScreen(void) {
    if (IsKeyPressed(KEY_ENTER)) {
        finishScreen = true;
    }
}

// Logo Screen Draw logic
void DrawLogoScreen(void) {
    DrawText(GAME_NAME, logoPositionX, logoPositionY, 82, RED);
}

// Logo Screen Unload logic
void UnloadLogoScreen(void) {
    // Unload LOGO screen variables here!
}

// Logo Screen should finish?
int FinishLogoScreen(void) { return finishScreen; }
