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
#include <stdlib.h>
#include <string.h>

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

void BugSyncGeneColor(HexBug *bug) {
    bug->gene.hColor.r = bug->gene.red;
    bug->gene.hColor.g = bug->gene.green;
    bug->gene.hColor.b = bug->gene.blue;
    bug->gene.hGene = ColorToInt(bug->gene.hColor);
    bug->colsnRadius = Remap(
        bug->gene.blue, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT, BUG_MIN_RANGE,
        BUG_MAX_RANGE
    );
}

static int mutateGeneElem(int base, int range) {
    int d = GetRandomValue(-range, range);
    return ClampInt(base + d, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);
}

HexBug NewSuperBug(int tile) {
    HexBug bug = NewHexBug(
        ToHex(BUG_MAX_GENE_LIMIT, BUG_MAX_GENE_LIMIT, BUG_MAX_GENE_LIMIT)
    );
    bug.tile = tile;
    bug.pos = NavTiles[tile].pos;
    bug.colsnRadius = getRange(&bug);
    bug.isSuper = true;
    setRandomBugTarget(&bug);
    return bug;
}

bool PlaceShopSuperBugOrder(void) {
    if (HexBugCount >= MAX_BUGS) {
        return false;
    }
    int tile = GetRandomValue(0, NavTileCount - 1);
    arrput(HexBugs, NewSuperBug(tile));
    HexBugCount++;

    return true;
}

HexBug NewMutatedBug(int tile, int mutation) {
    int red = mutateGeneElem(BUG_BASE_HEALTH, mutation);
    int green = mutateGeneElem(BUG_BASE_SPEED, mutation);
    int blue = mutateGeneElem(BUG_BASE_RANGE / DEFAULT_BUG_SIZE * 20, mutation);

    HexBug bug = NewHexBug(ToHex(red, green, blue));
    bug.tile = tile;
    bug.pos = NavTiles[tile].pos;
    bug.colsnRadius = getRange(&bug);
    setRandomBugTarget(&bug);
    return bug;
}

void MaintainMinimumBugs(void) {
    if (HexBugCount >= MIN_CUR_BUG || HexBugCount >= MAX_BUGS) {
        return;
    }

    int tile = GetRandomValue(0, NavTileCount - 1);
    arrput(HexBugs, NewMutatedBug(tile, BUG_GENE_MUTATION_RANGE));
    HexBugCount++;
    refreshBugIndexes();
}

HexBug NewGenesisBug(bool primary, int tile) {
    HexBug bug =
        NewHexBug(ToHex(BUG_BASE_HEALTH, BUG_BASE_SPEED, BUG_BASE_RANGE));
    bug.tile = tile;
    bug.pos = NavTiles[tile].pos;
    setRandomBugTarget(&bug);
    return bug;
}

int getGeneWanted(const HexBug *bug) {
    switch (OrderDemand) {
        case DEMAND_WANT_RED: return bug->gene.red;
        case DEMAND_WANT_GREEN: return bug->gene.green;
        case DEMAND_WANT_BLUE: return bug->gene.blue;
    }
}

bool SellPennedBugs(void) {
    int earning = 0;
    for (int i = HexBugCount - 1; i >= 0; i--) {
        HexBug *bug = &HexBugs[i];
        if (!bug->isPenned || bug->isSuper) {
            continue;
        }

        int geneElem = getGeneWanted(bug);
        float match = (float)geneElem / (float)BUG_MAX_GENE_LIMIT;
        int price = (int)(DemandPrice * match);
        earning += price;
        arrdel(HexBugs, i);
        HexBugCount--;
        // TraceLog(LOG_WARNING, "Selling -> %d | %d out %d", bug->id, price,
        // DemandPrice);
    }

    refreshBugIndexes();
    Money += earning;

    return earning > 0;
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
        .hasPartner = false,
        .matingCooldown = BUG_MATE_COOLDOWN,
        .faceAngle = 0,
        .txtRect = (Rectangle){0, 0, 0, 0},
        .hunger = 1.0f,
        .isPenned = false,
        .dragging = false,
        .isSuper = false,
        .partnerId = -1,
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

static HexGene crossGene(const HexGene *a, const HexGene *b) {
    HexGene r;
    r.red = ((a->red + b->red) / 2) +
            GetRandomValue(-BUG_NEW_CHILD_MUTATION, BUG_NEW_CHILD_MUTATION);
    r.green = ((a->green + b->green) / 2) +
              GetRandomValue(-BUG_NEW_CHILD_MUTATION, BUG_NEW_CHILD_MUTATION);
    r.blue = ((a->blue + b->blue) / 2) +
             GetRandomValue(-BUG_NEW_CHILD_MUTATION, BUG_NEW_CHILD_MUTATION);

    r.red = ClampInt(r.red, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);
    r.green = ClampInt(r.green, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);
    r.blue = ClampInt(r.blue, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);

    r.hColor = (Color){r.red, r.green, r.blue, 0xff};
    r.hGene = ColorToInt(r.hColor);
    return r;
}

void TryMerge(int bugA, int bugB) {
    int aIndex = FindBugByID(bugA);
    int bIndex = FindBugByID(bugB);

    if (aIndex == -1 || bIndex == -1) {
        return;
    }

    HexBug a = HexBugs[aIndex];
    HexBug b = HexBugs[bIndex];

    HexGene childGene = crossGene(&a.gene, &b.gene);
    HexBug child = NewHexBug(childGene.hGene);
    child.gene = childGene;
    child.tile = a.tile;
    child.pos = a.pos;
    setRandomBugTarget(&child);

    int hiIndex = aIndex > bIndex ? aIndex : bIndex;
    int loIndex = aIndex > bIndex ? bIndex : aIndex;

    arrdel(HexBugs, hiIndex);
    arrdel(HexBugs, loIndex);
    HexBugCount -= 2;
    refreshBugIndexes();
    arrput(HexBugs, child);
    HexBugCount++;
    refreshBugIndexes();
}

static bool canMate(const HexBug *bug) {
    return !bug->isPenned && !bug->dragging && bug->matingCooldown <= 0 &&
           !bug->hasPartner && bug->gene.red >= MIN_BUG_MATE_HEALTH &&
           bug->hunger >= 0.5f;
}

static int detectMate(HexBug *bug) {
    if (!canMate(bug)) return -1;

    for (int i = 0; i < HexBugCount; i++) {
        HexBug *other = &HexBugs[i];
        if (other->id == bug->id || other->hasPartner) {
            continue;
        }

        if (!canMate(other)) {
            continue;
        }

        if (CheckCollisionCircles(
                bug->pos, bug->colsnRadius, other->pos, other->colsnRadius
            )) {
            return i;
        }
    }

    return -1;
}

void BugSeekMate(HexBug *bug, int fc) {
    if (bug->isPenned || bug->dragging) return;
    if (bug->matingCooldown > 0) {
        bug->matingCooldown--;
    }

    if (bug->hasPartner) {
        int pidx = FindBugByID(bug->partnerId);
        if (pidx == -1) {
            bug->hasPartner = false;
            bug->partnerId = -1;
            bug->state = HEX_BUG_IDLE;
            return;
        }

        HexBug *partner = &HexBugs[pidx];
        float speed = getSpeed(bug) * 0.5f;
        bug->pos = Vector2MoveTowards(bug->pos, partner->pos, speed * GetFrameTime());
        Vector2 delta = Vector2Subtract(bug->pos, partner->pos);
        float angleRad = atan2f(delta.y, delta.x);
        bug->faceAngle = (RAD2DEG * angleRad) + 270.0f;
        bug->txtRect = (Rectangle){
            bug->pos.x, bug->pos.y, (float)bugBodyTxt.width * BUG_TXT_SCALE,
            (float)bugBodyTxt.height * BUG_TXT_SCALE
        };
        if (Vector2Distance(bug->pos, partner->pos) <= 0.5f) {

            if (bug->id < partner->id) {
                TryMerge(bug->id, partner->id);
            }
        }

        return;
    }

    int mate = detectMate(bug);
    if (mate != -1) {
        HexBug *partner = &HexBugs[mate];
        bug->hasPartner = true;
        bug->partnerId = partner->id;
        bug->state = HEX_BUG_MATING;

        partner->hasPartner = true;
        partner->partnerId = bug->id;
        partner->state = HEX_BUG_MATING;
    }
}

static void updateBugPos(HexBug *bug, Vector2 newPos) {
    bug->pos = newPos;
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

bool IsBugInPen(HexBug *bug) {
    if (CheckCollisionCircles(
            bug->pos, bug->size, SellingPenPosition, SELLING_PEN_RADIUS
        )) {
        return true;
    }

    return false;
}

void UpdateBugDragging(void) {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < HexBugCount; i++) {
            HexBug *bug = &HexBugs[i];
            if (CheckCollisionPointCircle(mouse, bug->pos, bug->size)) {
                DraggingBugID = bug->id;
                bug->dragging = true;
                break;
            }
        }
    }

    if (DraggingBugID == -1) {
        return;
    }

    HexBug *bug = NULL;
    int bugIndex = FindBugByID(DraggingBugID);
    if (bugIndex == -1) {
        DraggingBugID = -1;
        return;
    }
    bug = &HexBugs[bugIndex];
    bug->pos = mouse;
    bug->txtRect.x = bug->pos.x;
    bug->txtRect.y = bug->pos.y;

    if (!IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        return;
    }

    // Mouse released
    bug->dragging = false;
    DraggingBugID = -1;
    bug->target = -1;
    bug->nextTile = -1;
    bug->state = HEX_BUG_IDLE;

    if (IsBugInPen(bug)) {
        if (bug->foundFood) {
            ReleaseClaimFood(bug->foodId, bug->id);
            bug->foundFood = false;
            bug->foodTile = -1;
            bug->foodId = -1;
        }

        bug->isPenned = true;
    } else {
        int nearestTile = FindNearestNavTile(bug->pos);
        if (nearestTile != -1) {
            bug->tile = nearestTile;
            bug->pos = NavTiles[nearestTile].pos;
        }

        bug->isPenned = false;
    }
}

void UpdateHexBug(HexBug *bug, int fc) {
    if (bug->dragging || bug->isPenned) {
        return;
    }
    if (!bug->foundFood) {
        int foodIndex = detectFood(bug);
        if (foodIndex > -1) {
            bug->foundFood = true;
            bug->foodTile = HexFoods[foodIndex].navTile;
            bug->foodId = HexFoods[foodIndex].id;
            setBugTargetTile(bug, bug->foodTile);
            bug->nextTile = -1;
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
                EatFood(foodIndex, bug);
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
            ReleaseClaimFood(bug->foodId, bug->id);
            arrdel(HexBugs, bug->listIndex);
            HexBugCount--;
            refreshBugIndexes();
        }

        bug->hunger += (fc % 3 == 0) ? -0.5f : 0;
        if (bug->hunger <= 0.0f) {
            bug->gene.red -= (fc % 60 == 0) ? 1.0f : 0;
            bug->gene.red =
                ClampInt(bug->gene.red, BUG_MIN_GENE_LIMIT, BUG_MAX_GENE_LIMIT);
            BugSyncGeneColor(bug);
            bug->hunger = 1.0f;
        }
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
            bug->pos.x, bug->pos.y, range, bug->hasPartner ? RED : BLUE
        );
    }

    if (DEBUG_BUG_HEALTH) {
        DrawTextEx(
            font, TextFormat("%d", bug->gene.red),
            (Vector2){bug->pos.x - 8, bug->pos.y - DEFAULT_BUG_SIZE - 16}, 8, 1,
            RED
        );
    }

    int icon = ICON_NONE;
    if (bug->dragging) {
        icon = ICON_HELP;
    } else if (bug->isPenned) {
        icon = ICON_STAR;
    }
    if (bug->hasPartner) {
        icon = ICON_HEART;
    }
    GuiDrawIcon(
        icon, bug->pos.x - 8, bug->pos.y - DEFAULT_BUG_SIZE - 16, 1,
        bug->gene.hColor
    );

    // PBDrawHexagon(bug->pos, bug->size, 1, BLANK, PINK);
}
