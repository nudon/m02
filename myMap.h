#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
  int posX;
  int posY;
} tile_pos_t;

typedef
struct {
  char* tilePath;
  item_t tileContents[12];
  tile_pos_t tilePosiiton;
  int isWall;  
} tile_t;

typedef
struct {
  int rows;
  int cols;
  tile_t** cells;
} tile_map_t;


//stuff to shove into map libraries
tile_map_t* debugMap();
tile_t** debugTilesInit();
tile_t** readMap(char* mapPath);
SDL_Texture* cinterTiles(tile_map_t* tiles, SDL_Renderer* gRan);
//instead of worrying about reading things
//consider merely hardcoding levels at first
//can deal with the intricities of file io later
//or parse the file character by character

#endif 
