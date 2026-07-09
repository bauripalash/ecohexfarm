// clang-format off

#include "hexbug.h"
#include "config.h"
#include "draw.h"
#include "hexgrid.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include "gameplay.h"
#include <stdbool.h>

// clang-format on

static void setRandomBugTarget(HexBug *bug) {
    do {
        bug->target = GetRandomValue(0, NavTileCount - 1);
    } while (bug->target == bug->tile);
}

int GeneColorRGBA(HexGene gene) { return 1; }

HexBug NewGenesisBug(bool primary, int tile) {
    HexBug bug =
        primary
            ? NewHexBug(ToHex(0xff, 0xff, 0xff))
            : NewHexBug(ToHex(BUG_BASE_HEALTH, BUG_BASE_SPEED, BUG_BASE_RANGE));
    setRandomBugTarget(&bug);
    bug.tile = tile;
    bug.pos = NavTiles[tile].pos;
    return bug;
}

HexBug NewHexBug(int color) {
    Color geneticColor = GetColor(RGBtoRGBA(color));
    HexBug bug = {
        .target = -1,
        .id = HexBugCount,
        .pos = (Vector2){0, 0},
        .tile = 0,
        .nextTile = -1,
        .size = DEFAULT_BUG_SIZE,
        .moving = false,
        .gene = (HexGene){
            .red = geneticColor.r,
            .green = geneticColor.g,
            .blue = geneticColor.b,
            .hGene = color,
            .hColor = geneticColor
        },

    };

    return bug;
}

void BugWalkToTarget(HexBug *bug) {
    if (bug->target == -1) {
        setRandomBugTarget(bug);
    }

    if (bug->tile == bug->target) {
        bug->target = -1;
        bug->moving = false;
        return;
    }

    if (!bug->moving) {
        bug->nextTile = GetBestNeighbor(bug->tile, bug->target);
        if (bug->nextTile == -1) {
            bug->target = -1;
            return;
        }

        bug->moving = true;
    }

    HexNavTile *next = &NavTiles[bug->nextTile];
    Vector2 targetPos = next->pos;

    float speed = Remap(bug->gene.green, 0, 255, 30.0f, 140.0f);

    bug->pos = Vector2MoveTowards(bug->pos, targetPos, speed * GetFrameTime());

    if (Vector2DistanceSqr(bug->pos, targetPos) < 0.25f * 0.25f) {
        TraceLog(LOG_WARNING, "Bug#%d reached %d", bug->id, bug->nextTile);
        bug->pos = targetPos;
        bug->tile = bug->nextTile;
        bug->nextTile = -1;
        bug->moving = false;
    }
}

void DrawHexBug(HexBug *bug) {
    /*
    PBDrawHexagon(
        bug->pos, bug->size, DEFAULT_BUG_THICK, bug->gene.hColor,
        bug->gene.hColor
    );
    */
    PBDrawHexagon(
        bug->pos, bug->size, 1, bug->gene.hColor, (Color){0xff, 0xff, 0xff, 50}
    );

    float eyeSize = bug->size * 0.18f;
    float eyeOffsetX = bug->size * 0.28f;
    Color eyeColor = GetOppositeColor(bug->gene.hColor);
    Vector2 leftEye = {bug->pos.x - eyeOffsetX, bug->pos.y};
    Vector2 rightEye = {bug->pos.x + eyeOffsetX, bug->pos.y};

    // PBDrawHexagonFilled(leftEye, eyeSize, eyeColor);
    // PBDrawHexagonFilled(rightEye, eyeSize, eyeColor);
    DrawCircleV(leftEye, eyeSize, eyeColor);
    DrawCircleV(rightEye, eyeSize, eyeColor);
    float range = Remap(
        bug->gene.blue, 0, BUG_MAX_RANGE, BUG_MIN_RANGE_VALUE,
        BUG_MAX_RANGE_VALUE
    );
    DrawCircleLines(bug->pos.x, bug->pos.y, range, BLUE);

    int target = bug->target;
    if (target != -1) {
        HexNavTile *targetTile = &NavTiles[target];
        Color tGray = GRAY;
        tGray.a = 10;
        PBDrawLine(bug->pos, targetTile->pos, 1, tGray);
    }
}
