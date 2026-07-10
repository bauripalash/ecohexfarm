#ifndef PB_GAMEPLAY
#define PB_GAMEPLAY

#define TERRAIN_MAP_RADIUS 2
#define TERRAIN_MAX_TILES  100
#define TERRAIN_TILE_SIZE  80.0f

#define NAV_MAP_RADIUS     10
#define NAV_MAX_TILES      360 * 3
#define NAV_TILE_SIZE      DEFAULT_BUG_SIZE

#define MAX_BUGS           100
#define INIT_BUGS          5

extern HexTerrainTile TerrainTiles[TERRAIN_MAX_TILES];
extern int TerrainTileCount;

extern HexNavTile NavTiles[NAV_MAX_TILES];
extern int NavTileCount;

extern HexBug HexBugs[MAX_BUGS];
extern int HexBugID;
extern int HexBugCount;

#endif
