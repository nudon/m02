#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN

#include <SDL2/SDL_image.h>
//#include "temp.h"
//#include "myNPC.h"



typedef
struct {
  char* itemPath;
} item;

typedef
struct {
  int x;
  int y;
} tilePos;

typedef
struct {
  int x;
  int y;
} pixPos;

//thinking I'll do a similar thing to how I handled npcs
//declare some uint8, have it store flags for properties of wall
typedef
struct {
  int isWall;  
  char* tilePath;
  item* tileContents;
  tilePos* tilePosition;
} tile;

typedef
struct {
  int rows;
  int cols;
  tile*** tiles;
}tileMap;

typedef
struct {
  tileMap* tileMap;
  //npcSet* allNpcs;
  SDL_Texture* mapBG;
  //propigating name changes will be fun
} map;




tilePos* createTilePos(int x, int y);
void freeTilePos(tilePos* tilePos);

SDL_Rect* getDrawMap();
SDL_Rect* getDrawScreen();
pixPos* getCameraPos();
void setCameraPos(pixPos* new);
tile* createTile(int x, int y);

void freeTile(tile* tile);
void freeTileMap(tileMap* tMap);
void freeMap(map* map);

map* debugMap();
tileMap* debugTilesInit();
tileMap* initTiles(int rows, int cols);
tileMap* changeMapDim(map* map, int newRows, int newCols);
void startDebugPopulate();
void setDrawnMap( map * map, pixPos* currentPos);
SDL_Texture* cinterTiles(tileMap* tiles);
tile* getTileFromMapPos(map* map, tilePos* pos);
tile* getTileFromMapCord(map* map, int y, int x);
tile* getTileFromTileMapPos(tileMap* tiles, tilePos* pos);
tile* getTileFromTileMapCord(tileMap* tiles, int x, int y);
int isAWall(tile* tile);
int getMapDimX(map* theMap);
int getMapDimY(map* theMap);
void setMapDimX(map* theMap, int newX);
void setMapDimY(map* theMap, int newY);
void cloneTile(tile* clone, tile* orig);

#endif 
