#include "config.h"
#include "raylib.h"
#include "screens.h"
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h> // Emscripten library
#endif

#include <stdio.h>  // Required for: printf()
#include <stdlib.h> // Required for:
#include <string.h> // Required for:

// Global States

GameScreen currentScreen = TITLE;
Font font = {0};
Music music = {0};
Sound fxCoin = {0};

static RenderTexture2D screen = {0};

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static GameScreen transToScreen = UNKNOWN;

// Change to screen, no transition effect
static void ChangeToScreen(int screen);

// Request transition to next screen
static void TransitionToScreen(int screen);

// Update transition effect
static void UpdateTransition(void);

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void);

// Update and draw one frame
static void UpdateDrawFrame(void);

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIN_SIZE, WIN_SIZE, "raylib game template");
    SetRandomSeed(time(NULL));
    screen = LoadRenderTexture(SCREEN_SIZE, SCREEN_SIZE);
    SetTextureFilter(screen.texture, TEXTURE_FILTER_POINT);

    InitAudioDevice();
    // font = LoadFont("resources/BIOSfontII.ttf");
    font = LoadFontEx("resources/font/PressStart2P.ttf", 32, NULL, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

    // currentScreen = TITLE;
    // InitTitleScreen();

    currentScreen = GAMEPLAY;
    InitGameplayScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    switch (currentScreen) {
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}

static void ChangeToScreen(int screen) {
    // Unload current screen
    switch (currentScreen) {
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen) {
        case TITLE: InitTitleScreen(); break;
        case OPTIONS: InitOptionsScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case ENDING: InitEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(int screen) {
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect (fade-in, fade-out)
static void UpdateTransition(void) {
    if (!transFadeOut) {
        transAlpha += 0.05f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f
        // instead of 1.05f For that reason we compare against 1.01f, to avoid
        // last frame loading stop
        if (transAlpha > 1.01f) {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen) {
                case TITLE: UnloadTitleScreen(); break;
                case OPTIONS: UnloadOptionsScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }

            // Load next screen
            switch (transToScreen) {
                case TITLE: InitTitleScreen(); break;
                case OPTIONS: InitOptionsScreen(); break;
                case GAMEPLAY: InitGameplayScreen(); break;
                case ENDING: InitEndingScreen(); break;
                default: break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    } else // Transition fade out logic
    {
        transAlpha -= 0.02f;

        if (transAlpha < -0.01f) {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = UNKNOWN;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void) {
    DrawRectangle(
        0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha)
    );
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
    // Update
    //----------------------------------------------------------------------------------
    // UpdateMusicStream(music);       // NOTE: Music keeps playing between
    // screens

    if (!onTransition) {
        switch (currentScreen) {
            case TITLE: {
                UpdateTitleScreen();

                if (FinishTitleScreen() == 1)
                    TransitionToScreen(OPTIONS);
                else if (FinishTitleScreen() == 2)
                    TransitionToScreen(GAMEPLAY);

            } break;
            case OPTIONS: {
                UpdateOptionsScreen();

                if (FinishOptionsScreen()) TransitionToScreen(TITLE);

            } break;
            case GAMEPLAY: {
                UpdateGameplayScreen();

                if (FinishGameplayScreen() == 1) TransitionToScreen(ENDING);
                // else if (FinishGameplayScreen() == 2)
                // TransitionToScreen(TITLE);

            } break;
            case ENDING: {
                UpdateEndingScreen();

                if (FinishEndingScreen() == 1) TransitionToScreen(TITLE);

            } break;
            default: break;
        }
    } else
        UpdateTransition(); // Update transition (fade-in, fade-out)

    BeginTextureMode(screen);
    {
        ClearBackground(RAYWHITE);

        switch (currentScreen) {
            case TITLE: DrawTitleScreen(); break;
            case OPTIONS: DrawOptionsScreen(); break;
            case GAMEPLAY: DrawGameplayScreen(); break;
            case ENDING: DrawEndingScreen(); break;
            default: break;
        }

        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();
    }
    EndTextureMode();

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawTexturePro(
            screen.texture, (Rectangle){0, 0, SCREEN_SIZE, -SCREEN_SIZE},
            (Rectangle){0, 0, WIN_SIZE, WIN_SIZE}, (Vector2){0, 0}, 0.0f, WHITE
        );

        // DrawTextEx(font, TextFormat("Bugs: %d", HexBugCount), (Vector2){5,
        // 5}, 8, 0, PbColorVWhite);
        DrawFPS(10, 10);
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
}
