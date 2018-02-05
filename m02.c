#include <stdio.h>
#include <pthread.h>

//#include "dirNav.h"
#include "myMap.h"
#include "myImage.h"
#include "myNPC.h"
//also, some very bizare behavior with renderer, top and bottom boundaries are normal
//but left and right boundaries, approaching them causes screen to stretch out, then reset to original render draw color

//prototypes 
static int init();

static void myClose();

static int loadMedia();

static void setDrawnMap();

static void startDebug();

static void gameLoop();

static void drawLoop(int* quitp);


//Globals
SDL_Window* gWin = NULL;
SDL_Renderer* gRan = NULL;
SDL_Texture* gTex = NULL;
extern int TILE_D;
int quit = 0;
npc_pos_t* cameraPos;

SDL_Rect drawnMap;
SDL_Texture* currMapBG = NULL;
tile_map_t* activeMap = NULL;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 360;
const int SLEEP_TIME_MS = 25;



  
//so, m02. Goal is to somehow get a basic topdown view going
//basic controls, have some image as a background
//additionally, having some other animate creatures



int main(int argc, char** args) {
  quit = 0;
  if (init() == 0) {
    if (loadMedia() == 0) {
      startDebug();
      gameLoop(); 
    }
    else {
      fprintf(stderr,"LoadMediaFailed\n");
    }    
  }
  else {
    fprintf(stderr, "Init failed\n");    
  }
  myClose();
  return 0;
}

static void startDebug() {
  activeMap = debugMap();
  currMapBG = cinterTiles(activeMap, gRan);
  NPC_t* mc = createNPC();
  makeMC(mc);
  mc->position->posX = 0;
  mc->position->posY = 0;
  cameraPos = mc->pixelPos;
  prependToNPC_list(activeMap->allNPCS->idleNPC, createNPC_node(mc));
}


void setDrawnMap() {
  tile_map_t * map = activeMap;
  npc_pos_t* currentPos = cameraPos;
  if (map->rows * TILED < SCREEN_HEIGHT) {
    drawnMap.y = (SCREEN_HEIGHT - map->rows * TILED) / 2;
    drawnMap.h = map->rows * TILED;    
  }
  else {
    drawnMap.y = currentPos->pixPosY - (SCREEN_HEIGHT / 2);
  }
  
  if (map->cols * TILED > SCREEN_WIDTH) {
    drawnMap.x = (SCREEN_WIDTH - map->cols * TILED) / 2;
    drawnMap.w = map->cols * TILED;
  }
  else {
    drawnMap.x = currentPos->pixPosX - (SCREEN_WIDTH / 2);
  }
  
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

  //set cameraPos to middle of drawn map
  cameraPos->pixPosX = drawnMap.x + (drawnMap.w / 2);
  cameraPos->pixPosY = drawnMap.y + (drawnMap.h / 2);
}


void gameLoop() {
  while(!quit) {
    SDL_RenderClear(gRan);
    pickDestLoop(activeMap->allNPCS);
    moveDestLoop(activeMap->allNPCS);
    setDrawnMap();
    SDL_RenderCopy(gRan, currMapBG, &drawnMap, NULL);
    drawAllNPCS(activeMap->allNPCS);
  }
} 

int init() {
  drawnMap.x = 0;
  drawnMap.y = 0;
  drawnMap.w = SCREEN_WIDTH;
  drawnMap.h = SCREEN_HEIGHT;
  int fail = 0;
  if(SDL_Init(SDL_INIT_VIDEO) >= 0) {
    gWin = SDL_CreateWindow("SDL, Now with renderererers",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    SCREEN_WIDTH,
			    SCREEN_HEIGHT,
			    SDL_WINDOW_SHOWN);
    if (gWin != NULL) {
      gRan = SDL_CreateRenderer(gWin, -1, SDL_RENDERER_ACCELERATED);
      if (gRan != NULL) {
	SDL_SetRenderDrawColor(gRan, 0xff, 0xff, 0xff, 0xff);
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
	  fail = 3;
	  fprintf(stderr, "Error in loading img loading librarys: %s \n",
		  IMG_GetError());
	}
      }
      else {
	fail == 2;
	fprintf(stderr, "Error in creating renderer for gWin: %s \n",
		SDL_GetError());
      }      
    }
    else {
      fail == 1;
      fprintf(stderr, "Window Could not be created: %s\n",
	      SDL_GetError());
    }
  }
  else {
    fprintf(stderr, "Error in init video %s\n",
	    SDL_GetError() );
    fail = 4;
  }
  return fail;
}


int loadMedia() {
  int fail = 0;
  currMapBG = loadTexture("/home/nudon/prg/gam/media/p/ran.jpg", gRan);
  if (gTex == NULL) {
    fail == 1;
  }
  return fail;
}

void myClose() {
  SDL_DestroyTexture(currMapBG);
  currMapBG = NULL;
  SDL_DestroyRenderer(gRan);
  gRan = NULL;
  SDL_DestroyWindow(gWin);
  gWin = NULL;
  IMG_Quit();
  free(currMapBG);
  SDL_Quit();
}
