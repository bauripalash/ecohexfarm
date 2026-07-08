#ifndef PB_GAMEPLAY
#define PB_GAMEPLAY

#define MAP_RADIUS     4
#define MAX_TILES      100

#define NAV_MAP_RADIUS 11
#define NAV_TILE_SIZE  DEFAULT_BUG_SIZE + 2
#define MAX_NAV_TILES  360 * 3
#define MAX_BUGS       50

extern HexMapTile MapTiles[MAX_TILES];
extern int MapTileCount;

extern HexNavTile NavTiles[MAX_NAV_TILES];
extern int NavTileCount;

extern HexBug HexBugs[MAX_BUGS];
extern int HexBugCount;

#endif
