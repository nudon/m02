#include <stdio.h>
#include <pthread.h>

//#include "dirNav.h"
#include "myMap.h"
#include "myImage.h"
#include "myNPC.h"
//so, switched to renderers.
//also, some very bizare behavior with renderer, top and bottom boundaries are normal
//but left and right boundaries, approaching them causes screen to stretch out, then reset to original render draw color


//going to just dump a bunch of design things I jotted down.
/*
  So for implementing npc's
in each map struct, have a list of npc's
npc struct itself, store position, x/y for now, some fields/flags for npc behavior
also speed, an animation struct *, and an anim index, keeps track of how far in cycle npc is

might want a similar approach to objects like boxes and crates, maybe doors. 
some struct, with some sprite sheet, posiiton, tracked of by map

for sprite struct, was thinking of having on filesystem, 1 sprite sheet per npc
haven't thought much beyond organizing sprite beyond seperate anim cycles get seperate rows/columns
then, in program, have a struct, containing the sprite sheet, and x/y dimensions of single sprite


now, issue of simultaneous character movement.
approach to this is have 2 functions, each having their own list or circular buffer
 1 that, for each npc in buffer, takes info like flags and pos, and spits out a direction to move
then, if a dir was chosen, removes from buffer, add's to 2's
2 that, for each npc in buffer, moves the npc in an appropriate vector. 
then, if npc has reached destination, remove from 2's buffer, add to 1
want destination to be a tile, to allow things to slide on ice or charge in a direction
not really sure how to store that. easiest would be store as a pair in cust struct. 
also, would want to set an npc's anim index from both functions
//might be too advanced, but idea of certain interactions interupting movement would be nice
//like getting knocked around, have dest and speed changed, 

then, for actually drawing everything, npcs stationary, moving, boxes, 
call some specialized functions that goes through relevant lists on map
//also, had an interesting idea. If I'm parsing npc list for things in a subset of positions
//might be worthwhile to have list be parrrallel to map, or in some way store in 2d array npcs
//So I can just search through subset of 2d array, and not whole list
//somewhat applies to non-tile based placement as well, just keep int array, and round npc pos
 */
//there she blows
//next time, I actually have to code. scary.

//prototypes 
static int init();

static void myClose();

static int loadMedia();

static void gameLoop();

static void drawLoop(int* quitp);

static int  startInputLoop(int* quitp);

static void* inputLoop(void* arg);

static void movePosition(int dx, int dy);

static void handleInput(SDL_Event e);

static void updateScreen();

//Globals
SDL_Window* gWin = NULL;
SDL_Renderer* gRan = NULL;
SDL_Texture* gTex = NULL;
extern int TILE_D;


SDL_Rect drawnMap;
SDL_Texture* currMap = NULL;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 360;
const int SLEEP_TIME_MS = 25;


enum KeyPress {
  KEY_PRESS_DEFAULT,
  KEY_PRESS_UP,
  KEY_PRESS_DOWN,
  KEY_PRESS_LEFT,
  KEY_PRESS_RIGHT,
  KEY_PRESS_TOTAL
};
  
//so, m02. Goal is to somehow get a basic topdown view going
//basic controls, have some image as a background
//additionally, having some other animate creatures



int main(int argc, char** args) {
  if (init() == 0) {
    if (loadMedia() == 0) {
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

//just handling interthread communications through global variable globQuit
//since acessing the return of input loop causes drawLoop to block
//wasn't able to modify contentes of any pointer args I sent to input loop
//compiler didn't like me touching contents of void*'s.

void gameLoop() {
  int quitp = 0;
  tile_map_t* werk =  debugMap();
  if ( startInputLoop(&quitp) == 0) {
    drawLoop(&quitp);
  }
} 
void drawLoop(int* quitp) {
  while(!(*quitp)) {
      updateScreen();      
      SDL_Delay(SLEEP_TIME_MS);
  }
  return;
}

int  startInputLoop(int* quitp) {
  int fail = 0;
  pthread_t inputThreadID;
  int threadStat =  pthread_create(&inputThreadID, NULL, inputLoop, quitp);
  if (threadStat != 0) {
    fprintf(stderr, "Error in creating thread for input processing\n");
    fail = 1;
  }
  return fail; 
}

void* inputLoop(void* arg) {
  int* quit = arg;
  SDL_Event e;
  while(!(*quit)) {
    while(SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
	*quit = 1;
      }
      else if (e.type == SDL_KEYDOWN) {
	handleInput(e);
      }
    }
    SDL_Delay(SLEEP_TIME_MS);
  }
  return NULL;
}

void handleInput(SDL_Event e) {
  //unsure how to implement multi-frame wakling
  //basic idea would be, walking 1 grid takes x frames
  //might be a good idea to tie this to some ingame speed stat
  
  //thinking how to write this  to handle seperate characters
  //would basically this, but instead of moving drawnmap values
  //change some things coord value
  //and determine correct map offset somewhere else
  int dist = 10;
  //int dist = floor((double)TILE_D / 4);
  switch (e.key.keysym.sym){
  case SDLK_UP:
    movePosition(0, -dist);
    break;		
  case SDLK_DOWN:
    movePosition(0, dist);
    break;
  case SDLK_LEFT:
    movePosition(-dist, 0);
    break;
  case SDLK_RIGHT:
    movePosition(dist,0);
    break;
  deault:
    break;      
  }
  return;
}

void movePosition(int dx, int dy) {
  //to prevent camera from moving off map.
  //would have to know how many grids fit onscreen, then doing some math
  
  //grabbing resolution of background(how exactly)
  //then keeping c < 0 and c + Screen_c < map.c
  //will be another issue, one of eventually implementing walls

  drawnMap.x += dx;
  drawnMap.y += dy;
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
  currMap = loadTexture("/home/nudon/prg/gam/media/p/ran.jpg", gRan);
  if (gTex == NULL) {
    fail == 1;
  }
  return fail;
}

void myClose() {
  SDL_DestroyTexture(currMap);
  currMap = NULL;
  SDL_DestroyRenderer(gRan);
  gRan = NULL;
  SDL_DestroyWindow(gWin);
  gWin = NULL;
  IMG_Quit();
  free(currMap);
  SDL_Quit();
}



void updateScreen() {
  //SDL_BlitSurface(currMap, &drawnMap, gScreenSurf, NULL);
  //SDL_UpdateWindowSurface(gWin);
  SDL_RenderClear(gRan);
  SDL_RenderCopy(gRan, currMap, &drawnMap, NULL);
  SDL_RenderPresent(gRan);
}


