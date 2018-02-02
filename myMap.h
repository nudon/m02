#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "myImage.h"
#include "temp.h"
#include "myNPC.h"
//considering a nother struct, which would just be a tile** and row/col info
//would be used in tile_map_t
//also, considering a new struct to hold tile properties, such as wall, ice, etc
//would be used in tile_t



typedef
struct {
  char* itemPath;
} item_t;


typedef
struct {
  char* tilePath;
  item_t tileContents[12];
  tile_pos_t* tilePosition;
  int isWall;  
} tile_t;

typedef
struct {
  int rows;
  int cols;
  tile_t** cells;
  NPC_move_list allNPCS;
} tile_map_t;

tile_map_t* debugMap();
tile_t** debugTilesInit();
tile_t** readMap(char* mapPath);
SDL_Texture* cinterTiles(tile_map_t* tiles, SDL_Renderer* gRan);
tile_t* getTileFromMapPos(tile_map_t* map, tile_pos_t* pos);
tile_t* getTileFromMapCord(tile_map_t* map, int y, int x);

#endif 
