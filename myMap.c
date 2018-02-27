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


//so, map editing. This is going to be fairly complicated, mostly because they're going to probably have their own menues
//basic idea, have some active tile. maybe make it a red rectangle. have it move around, probably reuse the single input code
//either for userinput or setting the drawn Map.
//could actually save off old movelist and idlelist, and just create a new one for just the camera thing
//could then set speed to some specific value, to allow for 1-frame tile traversal. Then just restore lists at end
//anyways, base case, have that move around using arrow keys. That part is easy.
//on pushing some special key, maybe enter or e, pull up some menu
//basically, will show the current values of the highlighted tile. That might require some reworking of menus, but simple
//the annoying part will be getting input through the game window. Will need to learn how to do that first
//then have to deal with sanitizing it. after that, maybe have a call to cinterTiles, to update map? at least when tile path changes
