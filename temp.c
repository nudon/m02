#include <stdlib.h>
#include "temp.h"


tile_pos_t* createTilePos(int x, int y) {
  tile_pos_t* tile = malloc(sizeof(tile_pos_t));
  tile->posX = x;
  tile->posY = y;
  tile->section = 0;
  return tile;
}

void freeTilePos(tile_pos_t* tilePos) {
  free(tilePos);
}
