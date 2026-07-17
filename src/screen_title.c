#include "colors.h"
#include "config.h"
#include "raylib.h"
#include "screens.h"
#include <string.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void) {
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Title Screen Update logic
void UpdateTitleScreen(void) {
    // TODO: Update TITLE screen variables here!

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        // finishScreen = 1;   // OPTIONS
        finishScreen = GAMEPLAY; // GAMEPLAY
        PlaySound(fxCoin);
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void) {
    DrawRectangleRec((Rectangle){0, 0, SCREEN_SIZE, SCREEN_SIZE}, PbColorVGray);
    const char *text = "< ecoHexFarm >";
    int textWidth = strlen(text) * 32;
    Vector2 textVec = {(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, 100};
    DrawTextEx(font, text, textVec, 32, 1.0f, PbColorVWhite);
    text = "by Palash Bauri";
    textWidth = strlen(text) * 32;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 32 + 30};

    DrawTextEx(font, text, textVec, 32, 1.0f, PbColorVGrayLight);

    text = "How to play:";
    textWidth = strlen(text) * 32;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 32 + 30};
    DrawTextEx(font, text, textVec, 32, 1.0f, PbColorVGrayLight);

    text = "> Bugs evolve themselved";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 32 + 30};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> Redish bugs are healthy";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> Greenish bugs are fast";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> Blueish bugs are scout";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> drag them into the pen & click sell";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> according to demand you will get paid";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> depending on matching demand";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "> use money to buy upgrades";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 16 + 10};
    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);

    text = "[enter to start]";
    textWidth = strlen(text) * 16;
    textVec =
        (Vector2){(SCREEN_SIZE / 2.0f) - textWidth / 2.0f, textVec.y + 32 + 50};

    DrawTextEx(font, text, textVec, 16, 1.0f, PbColorVGrayLight);
}

// Title Screen Unload logic
void UnloadTitleScreen(void) {}

// Title Screen should finish?
int FinishTitleScreen(void) { return finishScreen; }
