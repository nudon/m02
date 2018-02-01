#ifndef FILE_MYMAP_SEEN
#define FILE_MYMAP_SEEN
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "myImage.h"
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
  tile_pos_t* tilePosition;
  int isWall;  
} tile_t;

typedef
struct {
  int rows;
  int cols;
  tile_t** cells;
} tile_map_t;

struct tile_dist_node_struct {
  double dVal;
  tile_t* storedTile;
  struct tile_dist_node_struct * next;
  struct tile_dist_node_struct * prev;
};

typedef struct tile_dist_node_struct tile_dist_node_t;

typedef
struct {
  double dVal;
  tile_dist_node_t * start;
  tile_dist_node_t * end;
} tile_dist_list_t;


//stuff to shove into map libraries
tile_map_t* debugMap();
tile_t** debugTilesInit();
tile_t** readMap(char* mapPath);
SDL_Texture* cinterTiles(tile_map_t* tiles, SDL_Renderer* gRan);
tile_t* getTileFromMapPos(tile_map_t* map, tile_pos_t* pos);
tile_t* getTileFromMapCord(tile_map_t* map, int y, int x);

//stuff for detecting sections
void setAvgDist(tile_map_t* map, tile_dist_node_t* theNode);
void preMarkOffSect(tile_pos_t* center, tile_pos_t* tile);
void markOffSect(tile_pos_t* c1, tile_pos_t* c2);
int checkSquares(tile_dist_list_t* theList, tile_map_t* map, tile_pos_t* center, int loopCount);
void makeSections(tile_map_t* map);
int isAWall(tile_t* tile);
int makeSectionsCheckTile(tile_map_t* map, tile_dist_list_t* theList,  tile_pos_t* tile, tile_pos_t* center);

//stuff from mergesort
void redoBacklinks(tile_dist_list_t * theList);
void splitList(tile_dist_node_t* start, tile_dist_node_t* firstHalf, tile_dist_node_t* secondHalf);
int compare(tile_dist_node_t* first, tile_dist_node_t* second);
tile_dist_node_t* merge(tile_dist_node_t* first, tile_dist_node_t* second);
void mergeSort(tile_dist_node_t* startRef);

//tile_dist_list functions
tile_dist_list_t* createTile_dist_list();

tile_dist_node_t* createTile_dist_node(tile_t* tile);

void appendTo_tile_dist_list(tile_dist_list_t* list, tile_dist_node_t* new);

void prependToTile_dist_list(tile_dist_list_t* list, tile_dist_node_t* new);

void removeFromTile_dist_list(tile_dist_list_t* list, tile_t * tile);	  

void freeTile_dist_node(tile_dist_node_t* tileNode);

void freeTile_dist_list(tile_dist_list_t* tileList);

#endif 
