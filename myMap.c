//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myImage.h"
#include "myMenu.h"
#include "systemLimits.h"

//drawn map, essentially a src rect on bg texure to draw
//drawn screen, a dst rect on window to draw to
static SDL_Rect drawnMap, drawnScreen;
static pixPos* cameraPos;

map* debugMap() {
  map* debugMap = malloc(sizeof(map));
  debugMap->tiles = debugTilesInit();
  debugMap->rows = 16;
  debugMap->cols = 16;
  
  //debugMap->allNpcs = createNpcSet();
  return debugMap;
}

tile*** debugTilesInit() {
  char* blackTile = "/home/nudon/prg/gam/media/black.png";
  char* whiteTile = "/home/nudon/prg/gam/media/white.png";
  tile*** debugTiles = malloc(sizeof(tile*) * 16);
  for (int i = 0; i < 16; i++) {
    debugTiles[i] =  malloc(sizeof(tile*) * 16);
    for (int j = 0; j < 16; j++) {
      debugTiles[i][j] = createTile(j,i);
      writeFilePath(debugTiles[i][j]->tilePath, blackTile);
      if ((j + (i % 2)) % 2 == 0) {
	writeFilePath(debugTiles[i][j]->tilePath, whiteTile);
      }
    }
  }
  return debugTiles;
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

void setDrawnMap(map * map, pixPos* currentPos) {
  if (map->rows * TILED < SCREEN_HEIGHT) {
    drawnMap.y = 0;
    drawnMap.h = map->rows * TILED;
    drawnScreen.y =  (SCREEN_HEIGHT - map->rows * TILED) / 2;
  }
  else {
    drawnMap.y = currentPos->y - (SCREEN_HEIGHT / 2);
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
    drawnMap.x = currentPos->x - (SCREEN_WIDTH / 2);
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
  if (drawnMap.y > map->rows * TILED) {
    drawnMap.y = map->rows * TILED;
  }
  if (drawnMap.x > map->cols * TILED) {
    drawnMap.x = map->cols * TILED;
  }
  cameraPos->x = drawnMap.x + (drawnMap.w / 2);
  cameraPos->y = drawnMap.y + (drawnMap.h / 2);
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
  freeTilePos(tile->tilePosition);
  free(tile->tileContents);
  free(tile);
}

void freeMap(map* map) {
  SDL_DestroyTexture(map->mapBG);
  for (int col = 0; col < map->cols; col++) {
    for (int row = 0; row < map->rows; row++) {
      free(map->tiles[col][row]->tilePosition);
      free(map->tiles[col][row]->tileContents);
      free(map->tiles[col][row]);
    }
    free(map->tiles[col]);
  }
}


tile** readMap(char* mapPath) {
  return NULL;
}

SDL_Texture* cinterTiles(map* tiles) {
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
	tile* tile = getTileFromMapCord(tiles, j, i);
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
  return getTileFromMapCord(map, pos->x, pos->y);
}

tile* getTileFromMapCord(map* map, int x, int y) {
  if (x >= map->rows || x < 0 || y >= map->cols || y < 0) {
    return NULL;
  }
  else {
    return map->tiles[y][x];
  }
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



int isAWall(tile* tile) {
  if (tile == NULL || tile->isWall != 0) {
    return 1;
  }
  else {
    return 0;
  }
}
