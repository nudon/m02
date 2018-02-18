#ifndef FILE_MYMAP1_SEEN
#define FILE_MYMAP1_SEEN
#define TILED 16
//#define MAX_CELLS  1024

typedef
struct {
  int posX;
  int posY;
  int section;
} tile_pos_t;


tile_pos_t* createTilePos(int x, int y);
void freeTilePos(tile_pos_t* tilePos);
#endif
