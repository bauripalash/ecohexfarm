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
    return (HexBug){
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
    PBDrawHexagonLine(bug->pos, bug->size, DEFAULT_BUG_THICK, bug->gene.hColor);
    int target = bug->target;
    if (target != -1) {
        HexNavTile *targetTile = &NavTiles[target];
        PBDrawLine(bug->pos, targetTile->pos, 1, GRAY);
        // DrawText(
        //     TextFormat("%d->%d->%d", bug->tile, bug->nextTile, bug->target),
        //     bug->pos.x, bug->pos.y, 16, WHITE
        //);

        // DrawCircle(targetTile->pos.x, targetTile->pos.y, 5,
        // bug->gene.hColor);
    }
}
