#include "config.h"
#include "draw.h"
#include "external/raygui.h"
#include "external/stb_ds.h"
#include "gameplay.h"
#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "utils.h"
#include <math.h>
#include <stdbool.h>

static void setRandomBugTarget(HexBug *bug) {
    do {
        bug->target = GetRandomValue(0, NavTileCount - 1);
    } while (bug->target == bug->tile);
}

static void setBugTargetTile(HexBug *bug, int tile) {
    bug->target = tile;
    bug->nextTile = -1;
}

static int detectFood(HexBug *bug) {
    float radius = bug->colsnRadius;

    for (int i = 0; i < HexFoodCount; i++) {
        HexFood *food = &HexFoods[i];
        if (food->claimed) {
            continue;
        }
        if (CheckCollisionCircles(bug->pos, radius, food->pos, FOOD_RADIUS)) {

            return i;
        }
    }

    return -1;
}

static void refreshBugIndexes(void) {
    for (int i = 0; i < HexBugCount; i++) {
        HexBugs[i].listIndex = i;
    }
}

static float getHealth(const HexBug *bug) {
    float result = Remap(
        bug->gene.red, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT, BUG_MIN_HEALTH,
        BUG_MAX_HEALTH
    );
    return result;
}

static float getSpeed(const HexBug *bug) {
    float result = Remap(
        bug->gene.green, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT, BUG_MIN_SPEED,
        BUG_MAX_SPEED
    );
    return result;
}

static float getRange(const HexBug *bug) {
    float result = Remap(
        bug->gene.blue, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT, BUG_MIN_RANGE,
        BUG_MAX_RANGE
    );
    return result;
}

static void syncGeneColor(HexBug *bug) {
    bug->gene.hColor.r = bug->gene.red;
    bug->gene.hColor.g = bug->gene.green;
    bug->gene.hColor.b = bug->gene.blue;
    bug->gene.hGene = ColorToInt(bug->gene.hColor);
}

HexBug NewGenesisBug(bool primary, int tile) {
    HexBug bug =
        // primary
        //     ? NewHexBug(ToHex(0xff, 0xff, 0xff))
        //:
        NewHexBug(ToHex(BUG_BASE_HEALTH, BUG_BASE_SPEED, BUG_BASE_RANGE));
    setRandomBugTarget(&bug);
    bug.tile = tile;
    bug.pos = NavTiles[tile].pos;
    return bug;
}

HexBug NewHexBug(int color) {
    Color geneticColor = GetColor(RGBtoRGBA(color));
    HexBug bug = {
        .target = -1,
        .listIndex = HexBugCount,
        .id = HexBugID,
        .pos = (Vector2){0, 0},
        .tile = 0,
        .nextTile = -1,
        .size = DEFAULT_BUG_SIZE,
        .state = HEX_BUG_IDLE,
        .hasFellow = false,
        .faceAngle = 0,
        .txtRect = (Rectangle){0, 0, 0, 0},
        .hunger = 1.0f,
        .txtOrigin =
            (Vector2){
                ((float)bugBodyTxt.width * BUG_TXT_SCALE) / 2.0f,
                ((float)bugBodyTxt.height * BUG_TXT_SCALE) / 2.0f
            },
        .gene = (HexGene){
            .red = geneticColor.r,
            .green = geneticColor.g,
            .blue = geneticColor.b,
            .hGene = color,
            .hColor = geneticColor
        },

    };
    bug.colsnRadius = getRange(&bug);
    HexBugID++;
    return bug;
}

int FindBugByID(int id) {
    for (int i = 0; i < HexBugCount; i++) {
        if (HexBugs[i].id == id) {
            return i;
        }
    }

    return -1;
}

static void updateBugPos(HexBug *bug, Vector2 newPos) {
    bug->pos = newPos;
    if (bug->state == HEX_BUG_WANDERING) {
        bug->hasFellow = false;
        Vector2 pos = bug->pos;
        int fellowIndex = -1;
        float colsRadius = getRange(bug);
        for (int i = 0; i < HexBugCount; i++) {
            HexBug *fellow = &HexBugs[i];
            if (fellow->id == bug->id) {
                continue;
            }
            if (CheckCollisionCircles(
                    pos, colsRadius, fellow->pos, getRange(fellow)
                )) {
                bug->hasFellow = true;
                fellowIndex = i;
            }
        }
        if (bug->hasFellow) {
            if (bug->hunger > 0.8f) {
                HexBug *fellow = &HexBugs[fellowIndex];
                if (fellow->hunger > 0.8f) {
                    TraceLog(LOG_WARNING, "Bug#%d and Bug#%d ready to merge!");
                }
            }
        }
    }
    if (bug->target != -1) {
        HexNavTile *targetTile = &NavTiles[bug->target];
        Vector2 delta = Vector2Subtract(bug->pos, targetTile->pos);
        float angleRad = atan2f(delta.y, delta.x);
        bug->faceAngle = (RAD2DEG * angleRad) + 270.0f;
        bug->txtRect = (Rectangle){
            bug->pos.x, bug->pos.y, (float)bugBodyTxt.width * BUG_TXT_SCALE,
            (float)bugBodyTxt.height * BUG_TXT_SCALE
        };
    }
}

void BugWalkToTarget(HexBug *bug, int fc) {
    if (!bug->foundFood) {
        int foodIndex = detectFood(bug);
        if (foodIndex > -1) {
            bug->foundFood = true;
            bug->foodTile = HexFoods[foodIndex].navTile;
            bug->foodId = HexFoods[foodIndex].id;
            setBugTargetTile(bug, bug->foodTile);
            bug->nextTile = -1;
            TraceLog(
                LOG_WARNING, "Bug#%d found food : %d", bug->listIndex,
                bug->foodId
            );
            // return;
        }
    }

    if (bug->state == HEX_BUG_IDLE && fc % 120 == 0) {
        bug->state = HEX_BUG_WANDERING;
    }

    if (bug->target == -1 && bug->state == HEX_BUG_IDLE) {
        setRandomBugTarget(bug);
    }

    if (bug->tile == bug->target) {
        bug->target = -1;
        bug->state = HEX_BUG_IDLE;
        bool reachedFood = bug->foundFood && bug->tile == bug->foodTile;
        if (reachedFood) {
            int foodIndex = FindFoodByID(bug->foodId);
            if (foodIndex != -1) {
                if (EatFood(foodIndex)) {
                    bug->hunger = 1.0f;
                    bug->gene.red += 10;
                    syncGeneColor(bug);
                }
            }
            bug->foundFood = false;
            bug->foodTile = -1;
            bug->foodId = -1;
        }
        return;
    }

    if (bug->state == HEX_BUG_WANDERING) {
        bug->nextTile = GetBestNeighbor(bug->tile, bug->target);
        if (bug->nextTile == -1) {
            bug->target = -1;
            return;
        }
    }

    if (bug->state == HEX_BUG_WANDERING) {

        HexNavTile *next = &NavTiles[bug->nextTile];
        Vector2 targetPos = next->pos;

        float speed = getSpeed(bug);

        Vector2 bugPos =
            Vector2MoveTowards(bug->pos, targetPos, speed * GetFrameTime());

        updateBugPos(bug, bugPos);

        if (Vector2DistanceSqr(bug->pos, targetPos) < 0.25f * 0.25f) {
            updateBugPos(bug, targetPos);
            bug->tile = bug->nextTile;
            bug->nextTile = -1;
        }

        if (bug->gene.red <= 0) {
            arrdel(HexBugs, bug->listIndex);
            HexBugCount--;
            refreshBugIndexes();
        }

        bug->gene.red -= (fc % 60 == 0) ? 1.0f : 0;
        bug->gene.red =
            ClampInt(bug->gene.red, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);
        syncGeneColor(bug);
    }
}

void DrawHexBug(HexBug *bug) {
    Rectangle srcRect = {
        0, 0, (float)bugBodyTxt.width, (float)bugBodyTxt.height
    };

    DrawTexturePro(
        bugBodyTxt, srcRect, bug->txtRect, bug->txtOrigin, bug->faceAngle,
        bug->gene.hColor
    );
    DrawTexturePro(
        bugFaceTxt, srcRect, bug->txtRect, bug->txtOrigin, bug->faceAngle,
        bug->gene.hColor
    );

    if (DEBUG_BUG_TARGET) {
        int target = bug->target;
        if (target != -1) {
            HexNavTile *targetTile = &NavTiles[target];
            Color tGray = GRAY;
            tGray.a = 100;
            PBDrawLine(bug->pos, targetTile->pos, 1, tGray);
        }
    }

    if (DEBUG_BUG_RANGE) {
        float range = getRange(bug);
        DrawCircleLines(
            bug->pos.x, bug->pos.y, range, bug->hasFellow ? RED : BLUE
        );
    }

    if (DEBUG_BUG_HEALTH) {
        GuiDrawIcon(
            ICON_HEART, bug->pos.x - 8, bug->pos.y - DEFAULT_BUG_SIZE - 16, 1,
            RED
        );
    }

    DrawTextEx(
        font, TextFormat("%d", bug->gene.red),
        (Vector2){bug->pos.x - 8, bug->pos.y - DEFAULT_BUG_SIZE - 16}, 8, 1, RED
    );

    // PBDrawHexagon(bug->pos, bug->size, 1, BLANK, PINK);
}
