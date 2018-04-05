#ifndef FILE_MYMAP1_SEEN
#define FILE_MYMAP1_SEEN
#define TILED 16
//#define MAX_CELLS  1024

typedef
struct {
  int x;
  int y;
} tilePos;


tilePos* createTilePos(int x, int y);
void freeTilePos(tilePos* tilePos);
#endif
