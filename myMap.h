#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN

#include <SDL2/SDL_image.h>
#include "temp.h"
#include "myNPC.h"



typedef
struct {
  char* itemPath;
} item;


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
  //npcSet* allNpcs;
  SDL_Texture* mapBG;
  //propigating name changes will be fun
} map;


SDL_Rect* getDrawMap();
SDL_Rect* getDrawScreen();
pixPos* getCameraPos();
void setCameraPos(pixPos* new);
tile* createTile(int x, int y);

void freeTile(tile* tile);

void freeMap(map* map);

map* debugMap();
tile*** debugTilesInit();
void startDebugPopulate();
void setDrawnMap( map * map, pixPos* currentPos);
tile** readMap(char* mapPath);
SDL_Texture* cinterTiles(map* tiles);
tile* getTileFromMapPos(map* map, tilePos* pos);
tile* getTileFromMapCord(map* map, int y, int x);
int isAWall(tile* tile);

#endif 
