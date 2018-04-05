#include <stdlib.h>
#include "temp.h"


tilePos* createTilePos(int x, int y) {
  tilePos* tile = malloc(sizeof(tilePos));
  tile->x = x;
  tile->y = y;
  return tile;
}

void freeTilePos(tilePos* tilePos) {
  free(tilePos);
}
