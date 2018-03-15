#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN

#include <SDL2/SDL_image.h>
#include "temp.h"
#include "myNPC.h"



typedef
struct {
  char* itemPath;
} item_t;


//thinking I'll do a similar thing to how I handled npcs
//declare some uint8, have it store flags for properties of wall
typedef
struct {
  char* tilePath;
  item_t* tileContents;
  tile_pos_t* tilePosition;
  int isWall;  
} tile_t;

typedef
struct {
  int rows;
  int cols;
  tile_t** cells;
  NPC_move_list* allNPCS;
  SDL_Texture* mapBG;
} tile_map_t;

void setActiveMap(tile_map_t* newMap);
tile_map_t* getActiveMap();
SDL_Rect* getDrawMap();
SDL_Rect* getDrawScreen();
SDL_Texture* getMapBG();
void setMapBG(SDL_Texture* newBG);
npc_pos_t* getCameraPos();
void setCameraPos(npc_pos_t* new);
tile_t* createTile(int x, int y);

void freeTile(tile_t* tile);

void freeMap(tile_map_t* map);

tile_map_t* debugMap();
tile_t** debugTilesInit();
void startDebugPopulate();
void setDrawnMap( tile_map_t * map, npc_pos_t* currentPos);
tile_t** readMap(char* mapPath);
SDL_Texture* cinterTiles(tile_map_t* tiles);
tile_t* getTileFromMapPos(tile_map_t* map, tile_pos_t* pos);
tile_t* getTileFromMapCord(tile_map_t* map, int y, int x);
int isAWall(tile_t* tile);

#endif 
