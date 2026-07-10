#ifndef PB_GAMEPLAY
#define PB_GAMEPLAY

#include "raylib.h"
#include "utils.h"
#include <stdlib.h>

#define TERRAIN_MAP_RADIUS 2
#define TERRAIN_MAX_TILES  100
#define TERRAIN_TILE_SIZE  80.0f

#define NAV_MAP_RADIUS     10
#define NAV_MAX_TILES      360 * 3
#define NAV_TILE_SIZE      DEFAULT_BUG_SIZE

#define MAX_BUGS           100
#define INIT_BUGS          5

#define MAX_FOODS          10
#define INIT_FOODS         2

#define GARDEN_RADIUS      TERRAIN_TILE_SIZE * 4

#define SCREEN_CENTER      SCREEN_SIZE / 2

// -----------------------------------------------------------------------------
//                              HEX_BUG
// -----------------------------------------------------------------------------

typedef struct HexGene {
    // Health
    int red;
    // Speed
    int green;
    // Range
    int blue;
    // hex color
    int hGene;
    Color hColor;
} HexGene;

typedef struct HexBug {
    int id;
    int listIndex;
    int tile;
    int nextTile;
    int target;
    int size;
    Vector2 pos;
    HexGene gene;
    bool moving;
    bool hasFellow;
    float colsnRadius;
    float faceAngle;
    Rectangle txtRect;
    Vector2 txtOrigin;
} HexBug;

// -----------------------------------------------------------------------------
//                              HEX_GRID
// -----------------------------------------------------------------------------

typedef struct HexVec {
    int q;
    int r;
    int s;
} HexVec;

typedef struct HexTerrainTile {
    HexVec cord;
    int size;
    int colsnSize;
    Vector2 pos;
    Color color;
    Color border;
} HexTerrainTile;

typedef struct HexNavTile {
    HexVec cord;
    int size;
    Vector2 pos;
    bool walkable;
    bool hasFood;
    int bugs;
    int neighbor[6];
} HexNavTile;

// -----------------------------------------------------------------------------
//                              HEX_FOOD
// -----------------------------------------------------------------------------
typedef enum HexFoodType {
    HEX_FOOD_REDDISH,
    HEX_FOOD_GREENING,
    HEX_FOOD_BLUEISH,
    HEX_FOOD_SUPER,
    HEX_FOOD_TYPE_COUNT
} HexFoodType;

typedef struct HexFood {
    HexFoodType foodType;
    int navTile;
    int terrainTile;
    Vector2 pos;
} HexFood;

// -----------------------------------------------------------------------------
//                              Global Gameplay Data
// -----------------------------------------------------------------------------

extern HexTerrainTile TerrainTiles[TERRAIN_MAX_TILES];
extern int TerrainTileCount;

extern HexNavTile NavTiles[NAV_MAX_TILES];
extern int NavTileCount;

extern HexBug *HexBugs;
extern int HexBugID;
extern int HexBugCount;

extern HexFood HexFoods[MAX_FOODS];
extern int HexFoodCount;

// -----------------------------------------------------------------------------
//                              HEX_BUG
// -----------------------------------------------------------------------------

#define DEFAULT_BUG_SIZE   18
#define DEFAULT_BUG_THICK  1

#define BUG_MAX_GENE_LIMIT 255
#define BUG_MIN_GENE_LIMIT 0

#define BUG_MAX_HEALTH     100
#define BUG_MIN_HEALTH     1

#define BUG_MAX_SPEED      100
#define BUG_MIN_SPEED      1

#define BUG_MIN_RANGE      DEFAULT_BUG_SIZE
#define BUG_MAX_RANGE      DEFAULT_BUG_SIZE * 10

#define BUG_BASE_SPEED     127
#define BUG_BASE_HEALTH    127
#define BUG_BASE_RANGE     DEFAULT_BUG_SIZE * 3

#define BUG_TXT_SCALE      1.2f

HexBug NewGenesisBug(bool primary, int tile);
HexBug NewHexBug(int color);
void DrawHexBug(HexBug *bug);
void BugWalkToTarget(HexBug *bug, int fc);

// -----------------------------------------------------------------------------
//                              HEX_FOOD
// -----------------------------------------------------------------------------

HexFood NewHexFood(Vector2 position);
void UpdateHexFoodList(void);
void DrawHexFoodList(void);

// -----------------------------------------------------------------------------
//                              HEX_GRID
// -----------------------------------------------------------------------------

static const HexVec HexDirs[6] = {{1, 0, 0},  {1, -1, 0}, {0, -1, 0},
                                  {-1, 0, 0}, {-1, 1, 0}, {0, 1, 0}};

static inline int HexVecDistance(HexVec a, HexVec b) {
    return Max3(abs(a.q - b.q), abs(a.r - b.r), abs(a.s - b.s));
}

#define VecToHexVec(vec)  ((HexVec){.q = vec.x, .r = vec.y})
#define HexVecToVec(hvec) ((Vector2){.x = hvec.q, .y = hvec.r})

int GenerateTerrainTiles(
    Vector2 center,
    HexTerrainTile *tiles,
    int gridRadius,
    float hexSize,
    Color bg,
    Color brdr
);

int GenerateNavTiles(
    Vector2 center, HexNavTile *tiles, int gridRadius, float hexSize
);

int GetBestNeighbor(int tile, int target);
void BuildNavNeighbors(HexNavTile *tiles, int count);
void DrawTerrainTiles(HexTerrainTile *tiles, int count, float thickness);
void DrawNavTiles(HexNavTile *tiles, int count, float thickness);

#endif
