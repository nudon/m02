#include <stdio.h>
#include <pthread.h>

//#include "dirNav.h"
#include "myMap.h"
#include "myImage.h"
#include "myNPC.h"
#include "myMapSections.h"
#include "gameState.h"
#include "myInput.h"

//prototypes 
static int init();

static void myClose();

static int loadMedia();

static void setDrawnMap();

static int  startDebug();

static void gameLoop();

void pauseMenu();

void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


//enum GameState state;



//Globals
SDL_Window* gWin = NULL;
SDL_Renderer* gRan = NULL;
int quit = 0;
npc_pos_t* cameraPos;
SDL_Color backgroundColor;
static int updateWait = 15;

SDL_Rect drawnScreen;
SDL_Rect drawnMap;
SDL_Texture* currMapBG = NULL;
tile_map_t* activeMap = NULL;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 360;
const int SLEEP_TIME_MS = 25;
  
//so, m02. Goal is to somehow get a basic topdown view going
//basic controls, have some image as a background
//additionally, having some other animate creatures

//done with that, next thing that would be nice is get a map editor going

//also, thought about how I would implement traveling between zones.
//Essentially I'd have some additional fields on tiles, usually set to null
//would contain another tile_map_t, which it would travel to, and also a tile positio to load to
//on input, or maybe automatically, would load the map and set position as specified

//other things to do, get basic animation working. This means setting up sprite sheets
//and also updating draw functions to iterate over the correct sprite cycle. might need more flags

//beyond that, get my map section code worked out, or not.
//going to try this actually. have idea of getting it to compile first, then setting sections
//then changing color of tile based on sections, just to get a visual indication of stuff

//also, look into storing/reading maps to/from files. Or if not maps, at least specific information, for stuff like saves

//also maybe start playing around with what items should be. 

int main(int argc, char** args) {
  quit = 0;
  if (init() == 0) {
    if (loadMedia() == 0) {
      SDL_Texture* logo = loadTexture("/home/nudon/prg/gam/sdl/pocus.png", gRan);
      if (logo != NULL) {
	SDL_RenderClear(gRan);
	SDL_RenderCopy(gRan, logo, NULL, NULL);
	SDL_RenderPresent(gRan);
	SDL_Delay(100);
      }
      if (startDebug() == 0) {
      	gameLoop();
      }
      else {
	fprintf(stderr, "startDeubg failed, exitting\n");
      }
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

static void startDebugPopulate() {
    NPC_t* mc = createNPC();
    makeMC(mc);
    setNPCPositionByCord(mc, 1 , 1);
    addNPC(mc);
    
    NPC_t* tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 8, 8);
    addNPC(tori);

    /*
    tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 8, 8);
    addNPC(tori);
    */

    tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 7, 7);
    addNPC(tori);
    
    tori = createNPC();
    makeNPC(tori);
    setNPCPositionByCord(tori, 9, 9);
    addNPC(tori);
}

static int startDebug() {
  int fail = 0;
  activeMap = debugMap();
  //makeSections(activeMap);
  if (activeMap != NULL) {
    currMapBG = cinterTiles(activeMap, gRan);
    if (currMapBG != NULL) {
      startDebugPopulate();
    }
    else {
      fprintf(stderr, "Failed to cinter background tiles\n");
      fail = 2;
    }
  }
  else {
    fprintf(stderr, "Failed to load debugMap\n");
    fail = 1;
  }
  return fail;
}


void setDrawnMap() {
  tile_map_t * map = activeMap;
  npc_pos_t* currentPos = cameraPos;
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


void gameLoop() {
  setDrawnMap();
  while(!quit) {
    setBGColor(0,0,0,0xff);
    SDL_RenderClear(gRan);


    //gameRun functions
    if(state == GAMERUN) {
      pickDestLoop(activeMap->allNPCS);
      moveDestLoop(activeMap->allNPCS);
      setDrawnMap();
    }
    SDL_RenderCopy(gRan, currMapBG, &drawnMap, &drawnScreen);
    drawAllNPCS(activeMap->allNPCS);
    //gamePause functions
    if(state == GAMEPAUSE) {
      pauseMenu();
    }
    
    SDL_Delay(updateWait);
    SDL_RenderPresent(gRan);
  }
} 

int init() {
  drawnMap.x = 0;
  drawnMap.y = 0;
  drawnMap.w = SCREEN_WIDTH;
  drawnMap.h = SCREEN_HEIGHT;
  cameraPos = malloc(sizeof(npc_pos_t));
  cameraPos->pixPosX = 0;
  cameraPos->pixPosY = 0;
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
	if ((IMG_Init(imgFlags) & imgFlags)) {
	  state = GAMERUN;
	}
	else {
	  fail = 4;
	  fprintf(stderr, "Error in loading img loading librarys: %s \n",
		  IMG_GetError());
	}
      }
      else {
	fail = 3;
	fprintf(stderr, "Error in creating renderer for gWin: %s \n",
		SDL_GetError());
      }      
    }
    else {
      fail = 2;
      fprintf(stderr, "Window Could not be created: %s\n",
	      SDL_GetError());
    }
  }
  else {
    fprintf(stderr, "Error in init video %s\n",
	    SDL_GetError() );
    fail =1;
  }
  return fail;
}


int loadMedia() {
  int fail = 0;
  return fail;
}

void myClose() {
  freeMap(activeMap);
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


void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  SDL_SetRenderDrawColor(gRan, r, g, b, a);
}
