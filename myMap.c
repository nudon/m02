//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myImage.h"
#include "myMenu.h"
#include "systemLimits.h"
static SDL_Rect drawnMap, drawnScreen;
static npc_pos_t* cameraPos;
static tile_map_t* activeMap = NULL;

tile_map_t* debugMap() {
  tile_map_t* debugMap = malloc(sizeof(tile_map_t));
  debugMap->cells = debugTilesInit();
  debugMap->rows = 16;
  debugMap->cols = 16;
  
  debugMap->allNPCS = createNPC_move_list();
  return debugMap;
}

tile_t** debugTilesInit() {
  char* blackTile = "/home/nudon/prg/gam/media/black.png";
  char* whiteTile = "/home/nudon/prg/gam/media/white.png";
  tile_t** debugTiles = malloc(sizeof(tile_t*) * 16);
  for (int i = 0; i < 16; i++) {
    debugTiles[i] =  (tile_t*)malloc(sizeof(tile_t) * 16);
    for (int j = 0; j < 16; j++) {
      debugTiles[i][j] = *(createTile(j,i));
      writeFilePath(debugTiles[i][j].tilePath, blackTile);
      if ((j + (i % 2)) % 2 == 0) {
	writeFilePath(debugTiles[i][j].tilePath, whiteTile);
      }
    }
  }
  return debugTiles;
}


void startDebugPopulate() {
    NPC_t* mc = createNPC();
    makeMC(mc);
    setNPCPositionByCord(mc, 1 , 1);
    addNPC(mc);
    
    NPC_t* circ = createNPC();
    makeNPC(circ);
    setNPCPositionByCord(circ, 8, 8);
    addNPC(circ);

    circ = createNPC();
    makeNPC(circ);
    setNPCPositionByCord(circ, 7, 7);
    addNPC(circ);
    
    circ = createNPC();
    makeNPC(circ);
    setNPCPositionByCord(circ, 9, 9);
    addNPC(circ);
}

void setDrawnMap( tile_map_t * map, npc_pos_t* currentPos) {
  if (map->rows * TILED < SCREEN_HEIGHT) {
    drawnMap.y = 0;
    drawnMap.h = map->rows * TILED;
    drawnScreen.y =  (SCREEN_HEIGHT - map->rows * TILED) / 2;
  }
  else {
    drawnMap.y = currentPos->pixPosY - (SCREEN_HEIGHT / 2);
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
    drawnMap.x = currentPos->pixPosX - (SCREEN_WIDTH / 2);
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
  cameraPos->pixPosX = drawnMap.x + (drawnMap.w / 2);
  cameraPos->pixPosY = drawnMap.y + (drawnMap.h / 2);
}

tile_t * createTile(int x, int y) {
  tile_t* tile = malloc(sizeof(tile_t));
  tile->tilePath = malloc(sizeof(char) * MAXPATHLEN );
  tile->tileContents = malloc(sizeof(item_t*) * 12);
  tile->tilePosition = createTilePos(x,y);
  return tile;
}


void freeTile(tile_t* tile) {
  freeTilePos(tile->tilePosition);
  free(tile->tileContents);
  free(tile);
}

void freeMap(tile_map_t* map) {
  freeNPC_list(map->allNPCS->idleList);
  freeNPC_list(map->allNPCS->moveList);
  free(map->allNPCS);
  for (int col = 0; col < map->cols; col++) {
    for (int row = 0; row < map->rows; row++) {
      free(map->cells[col][row].tilePosition);
      free(map->cells[col][row].tileContents);
    }
    free(map->cells[col]);
  }
}


tile_t** readMap(char* mapPath) {
  //first, parse file, find dimensions
  //look before leeping, SDL has it's own file io
  //use that instead
  FILE* mapFile = fopen(mapPath, "r");
  tile_t** tiles;
  if (mapFile != NULL) {
    int maxChars = 256;
    char readCell[maxChars];
    int len;
    int rows;
    int cols;
    while(sscanf(mapPath, "%s", readCell) != EOF) {
      if (rows == 0) {
	cols++;
      }
      len = strlen(readCell);
      if (readCell[len - 1] == '\n') {
	rows++;
      }
    }
    //reset the FILE* somehow
    tiles = (tile_t**) malloc(sizeof(tile_t*) * cols);
    for (int i = 0; i < rows; i++) {
      tiles[i] = (tile_t*) malloc(sizeof(tile_t*) * rows);
      for (int j = 0; j < cols; j++) {
	sscanf(mapPath, "%s", readCell);
	tile_t* gdt;
	//parse things, store in gdt
	tiles[cols][rows] = *gdt;
	
      } 
    }
    return tiles;
  }
  else {
    fprintf(stderr, "Error in opening map file %s\n", mapPath);
  }
  return tiles;
}

SDL_Texture* cinterTiles(tile_map_t* tiles) {
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
	tile_t* tile = getTileFromMapCord(tiles, j, i);
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


tile_t* getTileFromMapPos(tile_map_t* map, tile_pos_t* pos) {
  return getTileFromMapCord(map, pos->posX, pos->posY);
}

tile_t* getTileFromMapCord(tile_map_t* map, int x, int y) {
  if (x >= map->rows || x < 0 || y >= map->cols || y < 0) {
    return NULL;
  }
  else {
    return &(map->cells[y][x]);
  }
}


SDL_Rect* getDrawMap() {
  return &drawnMap;
}

SDL_Rect* getDrawScreen() {
  return &drawnScreen;
}

npc_pos_t* getCameraPos() {
  return cameraPos;
}
void setCameraPos(npc_pos_t* new) {
  cameraPos = new;
}

void setActiveMap(tile_map_t* newMap) {
  activeMap = newMap;
}

tile_map_t* getActiveMap() {
  return activeMap;
}

SDL_Texture* getMapBG() {
  return activeMap->mapBG;
}

void setMapBG(SDL_Texture* newBG) {
  activeMap->mapBG = newBG; 
}

int isAWall(tile_t* tile) {
  if (tile == NULL || tile->isWall == 1) {
    return 1;
  }
  else {
    return 0;
  }
}
