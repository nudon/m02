//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myImage.h"
#include "myMenu.h"
#include "systemLimits.h"
#include "myInput.h"

//drawn map, essentially a src rect on bg texure to draw
//drawn screen, a dst rect on window to draw to

//so, got map editing mostly worked out, growing shrinking works fine
//just have an issue with the camera I guess. doesn't like big maps.
//got some issue resolved, still getting some odd things with scrolling maps.
//have case where npc starts not getting drawn too early
//got some weird scalling issues down, the "drawDistance" cap is still off
//fixed that, now only issue is that camera is slightly off center for actual mc
//actually centered on upperleft hand corner of tile. thought adding a contant tiled/2 offset would fix, it didn't
//trying to fuck/fine tune camera positions is actually kind of hard. added a multiple const to the tiled offset
//didn't do anything except make some draw distance stuff behave oddly
//maybe camera position isn't used like I think it is?
//yeah, made it just a constant thing. didn't seem to affect much
//looking at drawNpc then
//solution was in setDrawnMap, had to add the constant to the drawnMap cords 
static SDL_Rect drawnMap, drawnScreen;
static pixPos* cameraPos;
static int min(int f, int s);
//lenOfNullTerminatedThing
static int lontt(void* thing);

map* debugMap() {
  map* debugMap = malloc(sizeof(map));
  debugMap->tileMap = debugTilesInit();

  return debugMap;
}

tileMap* debugTilesInit() {
  return initTiles(16, 16);
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
    //newTiles[i][actualRows - 1] = NULL;
  }
  //newTiles[actualCols - 1] = NULL;


  return newTileMap;
}
//would like some ability to srhink/extend map limits without wiping out existing map
//also kind of difficult, since I would have to do this after I get an int to commit and before I replace the old int.
//also more difficult because I don't know which dimension I replaced,
//well I do, or I could, if I compare map dim before and after change
tileMap* changeMapDim(map* map, int newRows, int newCols) {
  tile* newTile,* oldTile;
  
  tileMap* oldTileMap = map->tileMap;
  int oldRows = oldTileMap->rows;
  int oldCols = oldTileMap->cols;
  int colBound = min(newCols, oldCols);
  int rowBound = min(newRows, oldRows);
  tileMap* newTileMap = initTiles(newCols, newRows);
  for(int colIndex = 0; colIndex < colBound; colIndex++) {
    for (int rowIndex = 0; rowIndex < rowBound; rowIndex++) {
      //hopefully this works.
      //doesn't, because it copies pointer too tilePos, which gets double freed
      newTile = getTileFromTileMapCord(newTileMap, colIndex, rowIndex);
      oldTile = getTileFromTileMapCord(oldTileMap, colIndex, rowIndex);
      cloneTile(newTile, oldTile);
    }
  }
  map->tileMap = newTileMap;
  freeTileMap(oldTileMap);
  return newTileMap;
  
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

SDL_Texture* cinterTiles(tileMap* tiles) {
  SDL_Texture* megaTexture = NULL;
  SDL_Surface* megaSurface;

  megaSurface = createSurfaceFromDim(tiles->cols * TILED, tiles->rows * TILED);
  if (tiles->rows * tiles->cols >= 0) {
    SDL_Rect subTexture;
    SDL_Surface* loadedSurface;
    subTexture.w = TILED;
    subTexture.h = TILED;

    subTexture.y = 0;
    for (int i = 0; i < tiles->rows; i++) {
      subTexture.x = 0;
      for (int j = 0; j < tiles->cols; j++) {
	tile* tile = getTileFromTileMapCord(tiles, j, i);
	char* thingy = tile->tilePath;
	loadedSurface = loadToSurface(thingy);
	SDL_BlitSurface(loadedSurface,
			NULL,
			megaSurface,
			&subTexture);
	SDL_FreeSurface(loadedSurface);
	subTexture.x += TILED;
      }
      subTexture.y += TILED;
    }
    megaTexture = loadSurfaceToTexture(megaSurface);
    SDL_FreeSurface(megaSurface);
  
  }
  else {
    fprintf(stderr, "Tile had one or more demension equal to zero\n");
  }
  return megaTexture;
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

//lenOfNullTerminatedThing
//only works with things that are void**'s
static int lontt(void* thing) {
  void** newThing = thing;
  int len = 0;
  if (newThing != NULL) {
    while(newThing[len] != NULL) {
      len++;
    }
  }
  return len;  
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

