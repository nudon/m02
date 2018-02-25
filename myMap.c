//also, ideally want to write a in engine map editor
//basically just highlight a tile, and specify field values
//might be a good use of learning how to do interfaces
//then, could write a function to output map to text or other format

//also, sdl has it's own file io libraries

#include "myMap.h"
extern npc_pos_t* cameraPos;
extern SDL_Rect drawnMap, drawnScreen;
extern int SCREEN_WIDTH, SCREEN_HEIGHT;

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
      debugTiles[i][j] = *(createTile(blackTile, j,i));
      if ((j + (i % 2)) % 2 == 0) {
	debugTiles[i][j].tilePath = whiteTile;
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
    
    NPC_t* tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 8, 8);
    addNPC(tori);

    tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 7, 7);
    addNPC(tori);
    
    tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 9, 9);
    addNPC(tori);
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

tile_t * createTile(char* textPath, int x, int y) {
  tile_t* tile = malloc(sizeof(tile_t));
  tile->tilePath = textPath;
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
  //my crimes end here. 
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

void drawAllNPCS(NPC_move_list* list) {
  drawNPCList(list->idleList);
  drawNPCList(list->moveList);
}

void drawNPCList(NPC_list_t* list) {
  NPC_node_t* current = list->start;
  while(current != NULL) {
    drawNPC(current->storedNPC);
    current = current->next;
  }
}


//temporary stuff for draw primitives
#include <math.h>
void myDrawRect(int x1, int y1, int x2, int y2) {
  SDL_Rect rect;
  rect.x = x1;
  rect.y = y1;
  rect.w = x2 - x1;
  rect.h = y2 - y1;
  SDL_RenderFillRect(getRenderer(), &rect);
}


void myDrawCirc(int x, int y, int rad) {
  double theta = 0;
  double halfACircle = M_PI;
  int quality = 100;
  double dTheta = M_PI / quality;
  int c1x, c2x, c1y, c2y;
  while(theta < halfACircle) {
    c1x = x + rad * cos(theta);
    c1y = y + rad * sin(theta);
    c2x = x;
    c2y = y - rad * sin(theta);
    myDrawRect(c1x, c1y, c2x, c2y);
    theta += dTheta;
  }

}

double mSq(double b) {
  return b*b;
}

void myDrawFunRect(int x1 , int y1, int x2, int y2, int layers) {
  if (layers > 0) {
    SDL_Color old;
    SDL_Renderer* rend = getRenderer();
    if (SDL_GetRenderDrawColor(rend, &old.r, &old.g, &old.b, &old.a) == 0) {
      int r;
      int g;
      int b;
      int scrmble = layers;
      int dx, dy;
      dx = ((x2 - x1) / 2) / layers;
      dy = ((y2 - y1) / 2) / layers;
      for (int i = 0; i <  layers; i++) {
	r = 255 * mSq(sin(i * M_PI/scrmble));
	r = r << (sizeof(int) / 2);
	g = 255 * mSq(cos(i * M_PI/scrmble));
	g = g << (sizeof(int) / 2);
	b = 255 * mSq(tan(i * M_PI/scrmble));
	b = b << (sizeof(int) / 2);
	SDL_SetRenderDrawColor( rend, r, g, b, 0xFF );
	myDrawRect( x1 + dx * i, y1 + dy * i, x2 - dx * i, y2 - dy * i);
      }
      SDL_SetRenderDrawColor(rend, old.r, old.g, old.b, old.a);
    }
    else {
      fprintf(stderr, "Unable to backup previous drawRender settings: %s  \n", SDL_GetError());
    }
  }
  else {
    fprintf(stderr, "FunRect passed negative or zero layers \n");
  }
}


void drawNPC(NPC_t* npc) {
  SDL_Rect srcRect, destRect;
  SDL_Renderer* rend = getRenderer();
  destRect.x = npc->pixelPos->pixPosX + drawnScreen.x;
  destRect.y = npc->pixelPos->pixPosY + drawnScreen.y;
  destRect.w = TILED;
  destRect.h = TILED;
  if (destRect.x + TILED < cameraPos->pixPosX - (SCREEN_WIDTH / 2) ||
      destRect.x - TILED > cameraPos->pixPosX + (SCREEN_WIDTH / 2)) {
  }
  else if (destRect.y + TILED < cameraPos->pixPosY - (SCREEN_HEIGHT / 2) ||
	   destRect.y - TILED > cameraPos->pixPosY + (SCREEN_HEIGHT / 2)) {
  }
  else {
    if (npc->animState->holder->sprite_sheet != NULL && 0) {
      
      srcRect.x = npc->animState->spriteCol * npc->animState->holder->sprite_width;
      srcRect.y = npc->animState->spriteRow * npc->animState->holder->sprite_height;
      srcRect.w = npc->animState->holder->sprite_width;
      srcRect.h = npc->animState->holder->sprite_height;
      
      SDL_RenderCopy(rend,
		     npc->animState->holder->sprite_sheet,
		     &srcRect,
		     &destRect);
    }
    else {
      //use drawing primitives
      if (npc->flags == 1) {
	myDrawFunRect(destRect.x, destRect.y, destRect.x + destRect.w, destRect.y + destRect.h, 5);
      }
      else {
	

	SDL_SetRenderDrawColor( rend, 0x00, 0x00, 0x00, 0xFF );
	myDrawCirc(destRect.x + (TILED / 2), destRect.y + (TILED / 2), TILED / 2);

	SDL_SetRenderDrawColor( rend, 0xff, 0xff, 0xff, 0xFF );
	myDrawCirc(destRect.x + (TILED / 2), destRect.y + (TILED / 2), TILED / 3);
      }
    }
  }
}


int isAWall(tile_t* tile) {
  if (tile == NULL || tile->isWall == 1) {
    return 1;
  }
  else {
    return 0;
  }
}
