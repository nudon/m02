//also, ideally want to write a in engine map editor
//basically just highlight a tile, and specify field values
//might be a good use of learning how to do interfaces
//then, could write a function to output map to text or other format

//also, sdl has it's own file io libraries

#include "myMap.h"
extern SDL_Renderer* gRan;
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
    debugTiles[i] = (tile_t*) malloc(sizeof(tile_t) * 16);
    for (int j = 0; j < 16; j++) {
      debugTiles[i][j] = (tile_t) {.tilePath = blackTile,
				   .tileContents = NULL,
				   .isWall = 0,};
      if (i * j % 2 == 0) {
	debugTiles[i][j].tilePath = whiteTile;
      }
    }
  }
  return debugTiles;
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

SDL_Texture* cinterTiles(tile_map_t* tiles, SDL_Renderer* gRan) {
  SDL_Texture* megaTexture = NULL;
  SDL_Surface* megaSurface;

  //I stole this from https://wiki.libsdl.org/SDL_CreateRGBSurface
  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  megaSurface = SDL_CreateRGBSurface(0, tiles->cols * TILED, tiles->rows * TILED,
				     32,rmask, gmask,  bmask, amask);
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
    megaTexture = loadSurfaceToTexture(megaSurface, gRan);
    SDL_FreeSurface(megaSurface);
  
  }
  else {
    fprintf(stderr, "Tile had one or more demension equal to zero\n");
  }
  return megaTexture;
}


tile_t* getTileFromMapPos(tile_map_t* map, tile_pos_t* pos) {
  return getTileFromMapCord(map, pos->posY, pos->posX);
}

tile_t* getTileFromMapCord(tile_map_t* map, int y, int x) {
  return &(map->cells[x][y]);
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
  SDL_RenderFillRect(gRan, &rect);
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
    if (SDL_GetRenderDrawColor(gRan, &old.r, &old.g, &old.b, &old.a) == 0) {
      int r;
      int g;
      int b;
      int scrmble = layers;
      int dx, dy;
      dx = ((x2 - x1) / 2) / layers;
      dy = ((y2 - y1) / 2) / layers;
      for (int i = 0; i <  layers; i++) {
	r = 255 * mSq(sin(i * M_PI/scrmble));
	g = 255 * mSq(cos(i * M_PI/scrmble));
	b = 255 * mSq(tan(i * M_PI/scrmble));
	SDL_SetRenderDrawColor( gRan, r, g, b, 0xFF );
	myDrawRect( x1 + dx * i, y1 + dy * i, x2 - dx * i, y2 - dy * i);
      }
      SDL_SetRenderDrawColor(gRan, old.r, old.g, old.b, old.a);
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
      //odd thing here, constraning sprite to a single tile
      //would be kind of odd to implement
      //width/height would be same as srcs
      //if things are just tall, y dimension would be different
      //if things are wide, would either have to change things, or do the classic
      //clobber wide monsters into 4 sub monsters that move in unison. 
      
      SDL_RenderCopy(gRan,
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
	SDL_SetRenderDrawColor( gRan, 0x00, 0x00, 0x00, 0xFF );
	myDrawCirc(destRect.x + (TILED / 2), destRect.y + (TILED / 2), TILED / 2);
      }
    }
  }
}


int isAWall(tile_t* tile) {
  if (tile->isWall == 0) {
    return 0;
  }
  else {
    return 1;
  }
}
