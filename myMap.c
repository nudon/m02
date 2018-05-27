//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myImage.h"
#include "myMenu.h"
#include "systemLimits.h"
#include "myInput.h"
#include "myEnv.h"
//drawn map, essentially a src rect on bg texure to draw
//drawn screen, a dst rect on window to draw to

static SDL_Rect drawnMap, drawnScreen;
static pixPos* cameraPos;
static int min(int f, int s);

map* debugMap() {
  map* debugMap = malloc(sizeof(map));
  debugMap->tileMap = debugTilesInit();
  debugMap->npcMap = initNpcMap(16, 16);
  return debugMap;
}

tileMap* debugTilesInit() {
  return initTiles(16, 16);
}

gen_matrix* initNpcMap(int cols, int rows) {
  gen_matrix* new = newGen_matrix(cols, rows);
  new->type = TYPE_NPCLIST;
  for(int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for(int colIndex = 0; colIndex < cols; colIndex++) {
      setDataAtIndex(new, colIndex, rowIndex, (void*)createNpcList());
    }
  }
  return new;
}

tileMap* initTiles(int cols, int rows) {
  char* blackTile = "black.png";
  char* whiteTile = "white.png";
  int actualRows = rows ;
  int actualCols = cols ;
  tileMap* newTileMap = malloc(sizeof(tileMap));
  tile*** newTiles = malloc(sizeof(tile*) * actualCols);
  newTileMap->rows = rows;
  newTileMap->cols = cols;
  newTileMap->tiles = newTiles;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    newTiles[colIndex] =  malloc(sizeof(tile*) * actualRows );
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      newTiles[colIndex][rowIndex] = createTile(colIndex,rowIndex);
      writeFilePath(getTileFromTileMapCord(newTileMap, colIndex, rowIndex)->tilePath, blackTile);
      if ((rowIndex + (colIndex % 2)) % 2 == 0) {
	writeFilePath(getTileFromTileMapCord(newTileMap, colIndex, rowIndex)->tilePath, whiteTile);
      }
    }
  }
  return newTileMap;
}


tileMap* changeMapDim(map* map, int newRows, int newCols) {
  //takes tile map. extends/shrinks dimenions of map, and copies over old tiles when applicable
  tile* newTile,* oldTile;
  npcList* newList, *oldList;
  tileMap* oldTileMap = map->tileMap;
  gen_matrix* oldNpcMap = map->npcMap;
  int oldRows = oldTileMap->rows;
  int oldCols = oldTileMap->cols;
  int colBound = min(newCols, oldCols);
  int rowBound = min(newRows, oldRows);
  tileMap* newTileMap = initTiles(newCols, newRows);
  gen_matrix* newNpcMap = initNpcMap(newCols, newRows);
  for(int colIndex = 0; colIndex < colBound; colIndex++) {
    for (int rowIndex = 0; rowIndex < rowBound; rowIndex++) {
      newTile = getTileFromTileMapCord(newTileMap, colIndex, rowIndex);
      oldTile = getTileFromTileMapCord(oldTileMap, colIndex, rowIndex);
      cloneTile(newTile, oldTile);
      newList = getNpcListAtIndex(newNpcMap, colIndex, rowIndex);
      oldList = getNpcListAtIndex(oldNpcMap, colIndex, rowIndex);
      moveNodesToNewList(newList, oldList);
    }
  }
  map->tileMap = newTileMap;
  map->npcMap = newNpcMap;
  freeTileMap(oldTileMap);
  freeNpcMatrix(oldNpcMap);
  return newTileMap;
  
}
SDL_Texture* cinterTiles(tileMap* tiles) {
  SDL_Texture* megaTexture = NULL;
  SDL_Surface* megaSurface;

  megaSurface = createSurfaceFromDim(tiles->cols * TILED, tiles->rows * TILED);
  if (tiles->rows * tiles->cols >= 0) {
    SDL_Rect subTexture;
    SDL_Surface* loadedSurface;
    subTexture.w = TILED;
    subTexture.h = TILED;


    subTexture.x = 0;
    //swapped these for lines up to not fuck up may cache hit/miss ratio
    //revert if anything breaks
    for (int j = 0; j < tiles->cols; j++) {
      subTexture.y = 0;
      for (int i = 0; i < tiles->rows; i++) {
	tile* tile = getTileFromTileMapCord(tiles, j, i);
	char* thingy = tile->tilePath;
	loadedSurface = loadToSurface(thingy);
	SDL_BlitSurface(loadedSurface,
			NULL,
			megaSurface,
			&subTexture);
	SDL_FreeSurface(loadedSurface);
	subTexture.y += TILED;
      }
      subTexture.x += TILED;
    }
    megaTexture = loadSurfaceToTexture(megaSurface);
    SDL_FreeSurface(megaSurface);
  
  }
  else {
    fprintf(stderr, "Tile had one or more demension equal to zero\n");
  }
  return megaTexture;
}

void startDebugPopulate() {
    npc* mc = createNpc();
    makeMC(mc);
    setTilePosByCord(mc->tilePos, 1 , 1);
    allignPixPosToTilePos(mc->pixelPos, mc->tilePos);
    addNpc(mc);

    if (1) {
    npc* circ = createNpc();
    makeNpc(circ);
    setTilePosByCord(circ->tilePos, 7, 7);
    allignPixPosToTilePos(circ->pixelPos, circ->tilePos);
    addNpc(circ);

    circ = createNpc();
    makeNpc(circ);
    setTilePosByCord(circ->tilePos, 8, 8);
    allignPixPosToTilePos(circ->pixelPos, circ->tilePos);
    addNpc(circ);
    
    circ = createNpc();
    makeNpc(circ);
    setTilePosByCord(circ->tilePos, 9, 9);
    allignPixPosToTilePos(circ->pixelPos, circ->tilePos);
    addNpc(circ);
    }

    initDebugNpc();
}

void setDrawnMap(map * actualMap, pixPos* currentPos) {
  tileMap* map = actualMap->tileMap;
  if (map->rows * TILED < SCREEN_HEIGHT) {
    drawnMap.y = 0;
    drawnMap.h = map->rows * TILED;
    drawnScreen.y =  (SCREEN_HEIGHT - map->rows * TILED) / 2;
  }
  else {
    drawnMap.y = currentPos->y - (SCREEN_HEIGHT / 2) + (TILED / 2);
    drawnMap.h = SCREEN_HEIGHT;
    drawnScreen.y = 0;
  }
  drawnScreen.h = drawnMap.h;
  
  if (map->cols * TILED < SCREEN_WIDTH) {
    drawnMap.x = 0;
    drawnMap.w = map->cols * TILED;
    drawnScreen.x =  (SCREEN_WIDTH - map->cols * TILED) / 2;
  }
  else {
    drawnMap.x = currentPos->x - (SCREEN_WIDTH / 2) + (TILED / 2);
    drawnMap.w = SCREEN_WIDTH;
    drawnScreen.x = 0;
  }
  drawnScreen.w = drawnMap.w;
  
  if (drawnMap.y < 0) {
    drawnMap.y = 0;
  }
  if (drawnMap.x < 0) {
    drawnMap.x = 0;
  }
  if (drawnMap.y + drawnMap.h > map->rows * TILED) {
    drawnMap.y = map->rows * TILED - drawnMap.h;
  }
  if (drawnMap.x + drawnMap.w > map->cols * TILED) {
    drawnMap.x = map->cols * TILED - drawnMap.w;
  }
  cameraPos->x = drawnScreen.x + (drawnScreen.w / 2);
  cameraPos->y = drawnScreen.y + (drawnScreen.h / 2);
}

tile * createTile(int x, int y) {
  tile* theTile = malloc(sizeof(tile));
  theTile->tilePath = malloc(sizeof(char) * MAXPATHLEN );
  theTile->tileContents = malloc(sizeof(item*) * 12);
  theTile->tilePosition = createTilePos(x,y);
  theTile->isWall = 0;
  return theTile;
}


tilePos* createTilePos(int x, int y) {
  tilePos* tile = malloc(sizeof(tilePos));
  tile->x = x;
  tile->y = y;
  return tile;
}

void freeTilePos(tilePos* tilePos) {
  free(tilePos);
}

void freeTile(tile* tile) {
  if (tile != NULL) {
    freeTilePos(tile->tilePosition);
    free(tile->tileContents);
    free(tile);
  }
}

void freeTileMap(tileMap* tMap) {
  tile* freeT;
  if (tMap != NULL) {
    for (int col = 0; col < tMap->cols; col++) {
      for (int row = 0; row < tMap->rows; row++) {
	freeT = getTileFromTileMapCord(tMap, col, row);
	freeTile(freeT);
      }
      free(tMap->tiles[col]);
    }
  }
  free(tMap->tiles);
  free(tMap);
}

void freeMap(map* map) {
  if (map != NULL) {
    SDL_DestroyTexture(map->mapBG);
    freeTileMap(map->tileMap);
    map->tileMap = NULL;
  }
}

tile* getTileFromMapPos(map* map, tilePos* pos) {
  return getTileFromTileMapPos(map->tileMap, pos);
}

tile* getTileFromMapCord(map* map, int x, int y) {
  return getTileFromTileMapCord(map->tileMap, x, y);
}

tile* getTileFromTileMapPos(tileMap* tileMap, tilePos* pos) {
  return getTileFromTileMapCord(tileMap, pos->x, pos->y);
}

tile*  getTileFromTileMapCord(tileMap* tileMap, int x, int y) {
  if (x < 0 || y < 0 || x >= tileMap->cols || y >= tileMap->rows) {
    fprintf(stderr, "attempting to get a tile out of bounds\n");
    return NULL;
  }
  return tileMap->tiles[x][y];
}



SDL_Rect* getDrawMap() {
  return &drawnMap;
}

SDL_Rect* getDrawScreen() {
  return &drawnScreen;
}

pixPos* getCameraPos() {
  return cameraPos;
}
void setCameraPos(pixPos* new) {
  cameraPos = new;
}

static int min(int f, int s) {
  if (f > s) {
    return s;
  }
  else {
    return f;
  }
}

int getMapDimY(map* theMap) {
  return theMap->tileMap->rows;
}

int getMapDimX(map* theMap) {
  return theMap->tileMap->cols;
}

void setMapDimX(map* theMap, int newX){
  theMap->tileMap->cols = newX;
}

void setMapDimY(map* theMap, int newY){
  theMap->tileMap->rows = newY;
}

void cloneTile(tile* clone, tile* orig) {
  clone->isWall = orig->isWall;
  
  memcpy(clone->tilePath, orig->tilePath, strlen(orig->tilePath));
  clone->tilePath[strlen(orig->tilePath)] = '\0';
  tilePos* pos = orig->tilePosition;
  clone->tilePosition = createTilePos(pos->x, pos->y);

  //not going to bother with item contents for now
}

int isAWall(tile* tile) {
  if (tile == NULL || tile->isWall != 0) {
    return 1;
  }
  else {
    return 0;
  }
}
