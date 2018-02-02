//here's my rough idea for implementing tiles and whatnot
//have some struct, contains all info on tile
//that means picture/texture for tile, as well as if it's a wall/whatnot
//as well as anything on that tile, so essentially some fileds are sets of items

//also, ideally want to write a in engine map editor
//basically just highlight a tile, and specify field values
//might be a good use of learning how to do interfaces
//then, could write a function to output map to text or other format

//also, sdl has it's own file io libraries

#include "myMap.h"

//globals

tile_map_t* debugMap() {
  tile_map_t* debugMap = malloc(sizeof(tile_map_t));
  debugMap->cells = debugTilesInit();
  debugMap->rows = 16;
  debugMap->cols = 16;
  return debugMap;
}

tile_t** debugTilesInit() {
  char* blackTile = "bba";
  char* whiteTile = "abb";
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
  //suddenly suck at string processing
  //want to grab things delimited by spaces
  //but also 
  //then, malloc a tile_t[rows][cells]
  //then, parse file again, initializing each cell
  //then create file_map_t, 
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
  //passes through tileMap of tiles, draws all tiles to 1 SDL_Texture
  //also need to change this to use Renderer/textures instead of Surfaces
  SDL_Texture* megaTexture = NULL;
  SDL_Surface* megaSurface;
  if (tiles->rows * tiles->cols >= 0) {
    //SDL_Surface* megaTexture = malloc(sizeof(SDL_Surface*));
    //unsure how to declare a SDL_Surface of unknown size
    //assuming for now that megaTexture is a blank surface of appropriate size
    SDL_Rect subTexture;
    subTexture.w = TILED;
    subTexture.h = TILED;
    subTexture.y = 0;
    subTexture.x = 0;
    SDL_Surface* loadedSurface;
    for (int i = 0; i < tiles->rows; i++) {
      for (int j = 0; j < tiles->cols; j++) {
	tile_t* tile = getTileFromMapCord(tiles, j, i);
	char* thingy = tile->tilePath;
	loadedSurface = loadToSurface(thingy);
	SDL_BlitSurface(loadedSurface,
			NULL,
			megaSurface,
			&subTexture);
	SDL_FreeSurface(loadedSurface);
	//load thingy, then add to some megatexture
	subTexture.x += TILED;
      }
      subTexture.y += TILED;
    }
    megaTexture = loadSurfaceToTexture(megaSurface, gRan);
  
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
