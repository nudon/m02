#include <stdio.h>
#include <pthread.h>
#include "myImage.h"
#include "myMap.h"
//#include "myNPC.h"
#include "gameState.h"
#include "myInput.h"
#include "myMenu.h"
#include "myEnv.h"
#include "systemLimits.h"

//prototypes 
static int init();

static void myClose();

static int loadMedia();

static int startDebug();

static void gameLoop();

void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

//Globals
static SDL_Window* gWin = NULL;

static int updateWait = 15;

//extern inits


const int SLEEP_TIME_MS = 25;
  
//todo list 
//basic map editor. got per tile thing good, unable to change map dim though

//travelling between maps would be nice. 

//other things to do, get basic animation working.
//main issue is dealing with "jagged array" sprite sheets

//map navigation, goal is just have npc's perform a fixed amount of naive maze solving guesses per loop

//also, look into storing/reading maps to/from files.
//might be wise to use xml format

//implement depth for tall things
//essentially just draw items from top row to bottom.
//would require some elbow grease for npcs, otherwise simple

//better menus
//like allow submenus, basically have some way of dividing a menu into rectangles, each of those contain menues

//removed all the *_t typdefs I made
//also want to eventually not include struct definititions within header files

int main(int argc, char** args) {
  //  quit = 0;
  if (init() == 0) {
    if (loadMedia() == 0) {
      setupEnvirons();
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


void gameLoop() {
  SDL_Renderer* gRan = getRenderer();


  while(getGameState() != GAMEQUIT) {
    setBGColor(0,0,0,0xff);
    SDL_RenderClear(gRan);

    //Standard I've been thinking about
    //have each gameMstate assosiate with some function,
    //wrap whatever's in the code block within a function
    //want to keep the number of seperate drawLoops going on down to one
    //so for menu's and all that, want to develop actions to not be a loop
    runAction(currentEnv);
    SDL_Delay(updateWait);
    SDL_RenderPresent(gRan);
  }
}

static int startDebug() {
  int fail = 0;
  setMap(currentEnv, debugMap());
  if (getMap(currentEnv) != NULL) {
    setMapBG(currentEnv, cinterTiles(getTileMap(currentEnv)));
    if (getMapBG(currentEnv) != NULL) {
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

int init() {
  SDL_Renderer* rend = NULL;
  setCameraPos(malloc(sizeof(pixPos)));
  int fail = 0;
  if(SDL_Init(SDL_INIT_VIDEO) >= 0) {
    gWin = SDL_CreateWindow("SDL, Now with renderererers",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    SCREEN_WIDTH,
			    SCREEN_HEIGHT,
			    SDL_WINDOW_SHOWN);
    if (gWin != NULL) {
      rend = SDL_CreateRenderer(gWin, -1, SDL_RENDERER_ACCELERATED);
      if (rend != NULL) {
	setRenderer(rend);
	SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0xff, 0xff);
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if ((IMG_Init(imgFlags) & imgFlags)) {
	  if (TTF_Init() != -1) {
	    //setActiveMenu(NULL);
	    //setMainMenu(createMainMenu());
	    //setMapEditMenu(createMapEditMainMenu());
	    setGameState(GAMERUN);
	  }
	  else {
	    fail = 5;
	    fprintf(stderr, "Error in loading TTF libraries: %s\n",
		    TTF_GetError());
	  }
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
  if (setMediaDir() == 0) {
    //I guess in future, load/set up any sprite sheet stuff
  }
  else {
    fprintf(stderr, "unable to set directory to media\n");
  }

  return 0;
}

void myClose() {
  //getting rid of most of my free calls, since crashing on double freeing is unsightly
  
  //freeMap(getMap(currentEnv));
  //freeEnvirons();

  freeRenderer();
  SDL_DestroyWindow(gWin);
  gWin = NULL;
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}


void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
}


