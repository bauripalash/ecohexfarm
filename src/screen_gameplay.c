#include "colors.h"
#include "config.h"
#include "draw.h"
#include "gameplay.h"
#include "raylib.h"
#include "screens.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#include <stdbool.h>

#define BG_COLOR        PbColorVGray
#define TILE_BG_COLOR   PbColorVGray
#define TILE_BRDR_COLOR PbColorVGrayLight

static int framesCounter = 0;
static int finishScreen = 0;

HexTerrainTile TerrainTiles[TERRAIN_MAX_TILES];
int TerrainTileCount = 0;

HexNavTile NavTiles[NAV_MAX_TILES];
int NavTileCount = 0;

HexBug *HexBugs = NULL;
int HexBugID = 0;
int HexBugCount = 0;

HexFood *HexFoods = NULL;
int HexFoodCount = 0;
int HexFoodID = 0;

Vector2 SellingPenPosition = {0, 0};

int DraggingBugID = -1;

int Money = INIT_MONEY;

bool HasOrder = false;
BuyerDemand OrderDemand;
int DemandPrice = 0;

static void drawBackground(void) {
    DrawTerrainTiles(TerrainTiles, TerrainTileCount, 1);
    if (DEBUG_NAV_TILES) {
        DrawNavTiles(NavTiles, NavTileCount, 1);
    }
}

static void initFirstBugs(void) {
    for (int i = 0; i < INIT_BUGS; i++) {
        arrput(HexBugs, NewGenesisBug(i == 0, i));
        HexBugCount++;
    }
}

static int getRandomNavTile(void) {
    int result = GetRandomValue(0, NavTileCount - 1);
    return result;
}

static void initFirstFoods(void) {
    for (int i = 0; i < INIT_FOODS; i++) {
        SpawnRandomFoodAtRandom();
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
    framesCounter = 0;
    finishScreen = 0;
    TerrainTileCount = GenerateTerrainTiles(
        (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, TerrainTiles,
        TERRAIN_MAP_RADIUS, TERRAIN_TILE_SIZE, TILE_BG_COLOR, TILE_BRDR_COLOR
    );
    NavTileCount = GenerateNavTiles(
        (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, NavTiles,
        NAV_MAP_RADIUS, NAV_TILE_SIZE
    );
    BuildNavNeighbors(NavTiles, NavTileCount);
    initFirstBugs();
    initFirstFoods();
    SellingPenPosition = (Vector2){
        SCREEN_SIZE - SELLING_PEN_MARGIN - SELLING_PEN_RADIUS,
        SCREEN_SIZE - SELLING_PEN_MARGIN - SELLING_PEN_RADIUS
    };
}

static bool showBuyMenu = false;

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {

    /*
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    if (!showBuyMenu) {
        UpdateDemand();
        MaintainMinimumBugs();
        UpdateBugDragging();
        for (int i = 0; i < HexBugCount; i++) {
            UpdateHexBug(&HexBugs[i], framesCounter);
        }
        for (int i = 0; i < HexBugCount; i++) {
            BugSeekMate(&HexBugs[i], framesCounter);
        }
        if (framesCounter % 120 == 0) {
            SpawnRandomFoodAtRandom();
        }
    }

    framesCounter++;
}
static bool msgShow = false;
static int msgTimeStart = 0;
static int msgTimeCur = 0;
static char msgText[512];
static int msgBoxWidth = SCREEN_SIZE - 50;
static int msgBoxHeight = 90;

static void drawMessage(void) {
    if (!msgShow) {
        return;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsKeyPressed(KEY_ENTER)) {
        msgShow = false;
        msgTimeStart = 0;
        msgTimeCur = 0;
        sprintf(msgText, "%s", "");
    }
    Rectangle msgRect = {
        (SCREEN_SIZE / 2.0f) - (msgBoxWidth / 2.0f),
        (SCREEN_SIZE / 2.0f) - (msgBoxHeight / 2.0f), msgBoxWidth, msgBoxHeight
    };
    Color bg = PbColorVGray;
    bg.a = 10;
    DrawRectangleRec((Rectangle){0, 0, SCREEN_SIZE, SCREEN_SIZE}, bg);
    int textWidth = strlen(msgText) * 16;
    DrawTextEx(
        font, msgText,
        (Vector2){SCREEN_SIZE / 2.0f - (textWidth / 2.0f), msgRect.y + 8}, 16,
        0, PbColorVWhite
    );
    if (msgTimeCur - msgTimeStart >= 60) {
        msgShow = false;
        msgTimeStart = 0;
        msgTimeCur = 0;
        sprintf(msgText, "%s", "");
    }

    msgTimeCur = framesCounter;
}
static void showMessage(const char *msg) {
    msgShow = true;
    msgTimeStart = framesCounter;
    sprintf(msgText, "%s", msg);
}

static int getUsedPrice(int price, int count, int max) {
    if (count > 0 && count < max) { // shouldn't happen but just in case
        return (floorf(max / 2.0f));
    }

    return price;
}

void UpdateDemand(void) {
    if (HasOrder) {
        return;
    }

    BuyerDemand demand =
        (BuyerDemand)GetRandomValue(DEMAND_WANT_RED, DEMAND_WANT_BLUE);
    OrderDemand = demand;
    DemandPrice = GetRandomValue(5, 30);
    HasOrder = true;
}

const char *getDemandName(void) {
    switch (OrderDemand) {
        case DEMAND_WANT_RED: return "Healthy";
        case DEMAND_WANT_GREEN: return "Fast";
        case DEMAND_WANT_BLUE: return "Scout";
    }
}

void handleShopBuying(HexShopItemType item) {
    int price = HexShopPrices[item].price;
    if (price > Money) {
        showMessage("You don't have enough money!");
        return;
    }
    switch (item) {
        case SHOP_FOOD_REDDISH: {
            HexFoodType ftype = HEX_FOOD_REDDISH;
            int placed = PlaceShopFoodOrder(ftype, SHOP_BUY_COUNT);
            Money -= getUsedPrice(price, placed, SHOP_BUY_COUNT);
            showMessage("Thank you for buying Reddish Food");
            break;
        }

        case SHOP_FOOD_GREENISH: {
            HexFoodType ftype = HEX_FOOD_GREENISH;
            int placed = PlaceShopFoodOrder(ftype, SHOP_BUY_COUNT);
            Money -= getUsedPrice(price, placed, SHOP_BUY_COUNT);
            showMessage("Thank you for buying Greenish Food");
            break;
        }

        case SHOP_FOOD_BLUEISH: {
            HexFoodType ftype = HEX_FOOD_BLUEISH;
            int placed = PlaceShopFoodOrder(ftype, SHOP_BUY_COUNT);
            Money -= getUsedPrice(price, placed, SHOP_BUY_COUNT);
            showMessage("Thank you for buying Blueish Food");
            break;
        }

        case SHOP_FOOD_SUPER: {
            HexFoodType ftype = HEX_FOOD_SUPER;
            int placed = PlaceShopFoodOrder(ftype, SHOP_BUY_COUNT);
            Money -= getUsedPrice(price, placed, SHOP_BUY_COUNT);
            showMessage("Thank you for buying Super Food");
            break;
        }

        case SHOP_BUG_SUPER: {
            if (PlaceShopSuperBugOrder()) {
                Money -= HexShopPrices[SHOP_BUG_SUPER].price;
                showMessage("Thank you for buying Super Bug");
            } else {
                showMessage("X Too many bugs present! X");
            }
            break;
        }
        default: break;
    }
}

static HexShopItemType openShop(void) {

    int buttonSize = 50;
    int buttonIcon = ICON_OK_TICK;
    int buttonMargin = 0;

    const char *textLabel = TextFormat("<Items Shop>");
    int textWidth = GuiGetTextWidth(textLabel);

    Vector2 screenCenter = (Vector2){SCREEN_SIZE / 2.0f, SCREEN_SIZE / 2.0f};
    Vector2 textPos = (Vector2){screenCenter.x - textWidth / 2.0f, 30};
    DrawTextEx(font, textLabel, textPos, 16, 0, PbColorVWhite);

    textLabel = TextFormat("Money: %d coins", Money);
    textWidth = GuiGetTextWidth(textLabel);
    textPos.x = screenCenter.x - textWidth / 2.0f;
    textPos.y += 20;

    DrawTextEx(font, textLabel, textPos, 16, 0, PbColorVWhite);

    // Healthy Food Buy
    textPos.y += 50;
    int itemRectWidth = 500;
    int itemRectHeight = 80;
    buttonMargin = (itemRectHeight - buttonSize) / 2.0f;
    Rectangle itemRect = {
        screenCenter.x - itemRectWidth / 2.0f, textPos.y, itemRectWidth,
        itemRectHeight
    };
    DrawRectangleRoundedLinesEx(itemRect, 0.6, 0, 2, PbColorVGrayLight);

    DrawPoly(
        (Vector2){itemRect.x + 50, itemRect.y + itemRectHeight / 2.0f}, 6,
        DEFAULT_BUG_SIZE, 30.0f, RED
    );

    DrawTextEx(
        font,
        TextFormat("Healthy Food: [%d]", HexShopPrices[SHOP_FOOD_REDDISH]),
        (Vector2){itemRect.x + 100, itemRect.y + itemRectHeight / 2.0f - 8}, 16,
        1, PbColorVWhite
    );

    if (PbIconButton(
            (Rectangle){
                itemRect.x + itemRect.width - (buttonSize + buttonMargin),
                itemRect.y + buttonMargin, buttonSize, buttonSize
            },
            buttonIcon
        ))
        return SHOP_FOOD_REDDISH;

    // Speedy Food Buy
    textPos.y += itemRect.height + 30;
    itemRect = (Rectangle){
        screenCenter.x - itemRectWidth / 2.0f, textPos.y, itemRectWidth,
        itemRectHeight
    };
    DrawRectangleRoundedLinesEx(itemRect, 0.6, 0, 2, PbColorVGrayLight);

    DrawPoly(
        (Vector2){itemRect.x + 50, itemRect.y + itemRectHeight / 2.0f}, 6,
        DEFAULT_BUG_SIZE, 30.0f, GREEN
    );

    DrawTextEx(
        font,
        TextFormat("Speedy Food: [%d]", HexShopPrices[SHOP_FOOD_GREENISH]),
        (Vector2){itemRect.x + 100, itemRect.y + itemRectHeight / 2.0f - 8}, 16,
        1, PbColorVWhite
    );

    if (PbIconButton(
            (Rectangle){
                itemRect.x + itemRect.width - (buttonSize + buttonMargin),
                itemRect.y + buttonMargin, buttonSize, buttonSize
            },
            buttonIcon
        ))
        return SHOP_FOOD_GREENISH;

    // Scouting Food Buy
    textPos.y += itemRect.height + 30;
    itemRect = (Rectangle){
        screenCenter.x - itemRectWidth / 2.0f, textPos.y, itemRectWidth,
        itemRectHeight
    };
    DrawRectangleRoundedLinesEx(itemRect, 0.6, 0, 2, PbColorVGrayLight);

    DrawPoly(
        (Vector2){itemRect.x + 50, itemRect.y + itemRectHeight / 2.0f}, 6,
        DEFAULT_BUG_SIZE, 30.0f, BLUE
    );

    DrawTextEx(
        font,
        TextFormat("Scouting Food: [%d]", HexShopPrices[SHOP_FOOD_BLUEISH]),
        (Vector2){itemRect.x + 100, itemRect.y + itemRectHeight / 2.0f - 8}, 16,
        1, PbColorVWhite
    );

    if (PbIconButton(
            (Rectangle){
                itemRect.x + itemRect.width - (buttonSize + buttonMargin),
                itemRect.y + buttonMargin, buttonSize, buttonSize
            },
            buttonIcon
        ))
        return SHOP_FOOD_BLUEISH;

    // Super Food Buy
    textPos.y += itemRect.height + 30;
    itemRect = (Rectangle){
        screenCenter.x - itemRectWidth / 2.0f, textPos.y, itemRectWidth,
        itemRectHeight
    };
    DrawRectangleRoundedLinesEx(itemRect, 0.6, 0, 2, PbColorVGrayLight);

    DrawPoly(
        (Vector2){itemRect.x + 50, itemRect.y + itemRectHeight / 2.0f}, 6,
        DEFAULT_BUG_SIZE, 30.0f, WHITE
    );

    DrawTextEx(
        font, TextFormat("Super Food: [%d]", HexShopPrices[SHOP_FOOD_SUPER]),
        (Vector2){itemRect.x + 100, itemRect.y + itemRectHeight / 2.0f - 8}, 16,
        1, PbColorVWhite
    );

    if (PbIconButton(
            (Rectangle){
                itemRect.x + itemRect.width - (buttonSize + buttonMargin),
                itemRect.y + buttonMargin, buttonSize, buttonSize
            },
            buttonIcon
        ))
        return SHOP_FOOD_SUPER;

    // Super Bug Buy
    textPos.y += itemRect.height + 30;
    itemRect = (Rectangle){
        screenCenter.x - itemRectWidth / 2.0f, textPos.y, itemRectWidth,
        itemRectHeight
    };
    DrawRectangleRoundedLinesEx(itemRect, 0.6, 0, 2, PbColorVGrayLight);

    DrawTextEx(
        font, TextFormat("Super Bug: [%d]", HexShopPrices[SHOP_BUG_SUPER]),
        (Vector2){itemRect.x + 100, itemRect.y + itemRectHeight / 2.0f - 8}, 16,
        1, PbColorVWhite
    );

    Vector2 texturePos = (Vector2){
        itemRect.x + DEFAULT_BUG_SIZE * 1.5f,
        itemRect.y + itemRectHeight / 2.0f - DEFAULT_BUG_SIZE
    };
    DrawTextureEx(bugBodyTxt, texturePos, 0.0f, 1.2, WHITE);
    DrawTextureEx(bugFaceTxt, texturePos, 0.0f, 1.2, WHITE);

    if (PbIconButton(
            (Rectangle){
                itemRect.x + itemRect.width - (buttonSize + buttonMargin),
                itemRect.y + buttonMargin, buttonSize, buttonSize
            },
            buttonIcon
        ))
        return SHOP_BUG_SUPER;

    buttonSize = 40;
    bool closed = PbIconButton(
        (Rectangle){
            SCREEN_SIZE / 2.0f - buttonSize, SCREEN_SIZE - buttonSize * 2,
            buttonSize * 2, buttonSize
        },
        ICON_CROSS
    );

    if (closed) {
        showBuyMenu = false;
        return SHOP_CLOSE;
    }

    return SHOP_ITEM_NONE;
}

void DrawHUD(void) {
    int ogText = GuiGetStyle(DEFAULT, TEXT_SIZE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

    Rectangle buyBtnRect = {10, 10, 100, 50};
    if (PbTextButton(buyBtnRect, "Shop", 16)) {
        showBuyMenu = true;
    }

    Rectangle moneyRect =
        (Rectangle){buyBtnRect.x + buyBtnRect.width + 10, 25, 200, 16};
    GuiLabel(moneyRect, GuiIconText(ICON_COIN, TextFormat("Coins: %d", Money)));

    Rectangle sellBtnRect = {SCREEN_SIZE - 300, SCREEN_SIZE - 80, 100, 40};
    if (PbTextButton(sellBtnRect, "Sell", 16)) {
        if (SellPennedBugs()) {
            HasOrder = false;
            UpdateDemand();
        }
    }

    Rectangle demandRect = {SCREEN_SIZE - 310, 10, 300, 30};

    DrawRectangleRoundedLinesEx(demandRect, 0.8f, 0, 3, PbColorTPink);
    const char *newsText =
        TextFormat("!%s Bugs for %d coins!", getDemandName(), DemandPrice);
    int newsTextWidth = strlen(newsText) * 8;
    DrawTextEx(
        font, newsText,
        (Vector2){
            demandRect.x + (demandRect.width / 2.0f) - (newsTextWidth / 2.0f),
            demandRect.y + (demandRect.height / 2.0f) - 4
        },
        8, 0, PbColorVGreen
    );

    if (showBuyMenu) {
        DrawRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, BG_COLOR);
        if (IsKeyPressed(KEY_ENTER)) {
            showBuyMenu = false;
        }
        HexShopItemType item = openShop();
        switch (item) {
            case SHOP_CLOSE: showBuyMenu = false; break;
            case SHOP_ITEM_NONE: break;
            case SHOP_ITEM_COUNT: break;

            default: handleShopBuying(item);
        }
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, ogText);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
    DrawRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, BG_COLOR);
    drawBackground();

    DrawHexFoodList();
    for (int i = 0; i < HexBugCount; i++) {
        DrawHexBug(&HexBugs[i]);
    }

    // DrawCircleLinesV(
    //     (Vector2){(float)SCREEN_CENTER, (float)SCREEN_CENTER}, GARDEN_RADIUS,
    //     WHITE
    //);

    DrawSellingPen();

    DrawHUD();
    drawMessage();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
    arrfree(HexBugs);
    arrfree(HexFoods);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
