#include <stdio.h>
#include <pthread.h>

//#include "dirNav.h"
#include "myImage.h"
#include "myMap.h"
#include "myNPC.h"
//#include "myMapSections.h"
#include "gameState.h"
#include "myInput.h"
#include "myMenu.h"

//prototypes 
static int init();

static void myClose();

static int loadMedia();

static int startDebug();

static void gameLoop();

void pauseMenu();

void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

//Globals
static SDL_Window* gWin = NULL;
SDL_Color backgroundColor;
static int updateWait = 15;

//extern inits
int SCREEN_WIDTH = 480;
int SCREEN_HEIGHT = 360;
//const int MAXPATHLEN = 255;

//SDL_Rect drawnScreen;
//SDL_Rect drawnMap;
SDL_Texture* currMapBG = NULL;
//tile_map_t* activeMap = NULL;
const int SLEEP_TIME_MS = 25;
  
//so, m02. Goal is to somehow get a basic topdown view going
//basic controls, have some image as a background
//additionally, having some other animate creatures

//kind of unhappy with how text entry is working out
//mostly realized that anytime I have a different kind of input I need a new state
//believe it's better to have substates. where the action from m02 is the same
//but somewhere down the line menuInput sees the substate and goes to the correct handler
//also, kind of wanting to put some statehandling in menus
//specifically, specifying which substate/state to change to when entering menu
//kind of annoying, but alternate is setting subspace during action calls, 


//done with that, next thing that would be nice is get a map editor going

//also, thought about how I would implement traveling between zones.
//Essentially I'd have some additional fields on tiles, usually set to null
//would contain another tile_map_t, which it would travel to, and also a tile positio to load to
//on input, or maybe automatically, would load the map and set position as specified

//other things to do, get basic animation working. This means setting up sprite sheets
//and also updating draw functions to iterate over the correct sprite cycle. might need more flags

//putting map sections on the backlog of the backlog.
//For now implementation goal is just have npc's perform a fixed amount of naive maze solving guesses per loop

//also, look into storing/reading maps to/from files. Or if not maps, at least specific information, for stuff like saves

//also maybe start playing around with what items should be.

//had some ideas about how to implement depth for items whose sprites extend beyond a single tile
//essentially just draw items from top row to bottom. Also, modify npc drawing
//go through npcList, shove each npc into a list respective to which row they are on
//draw these appropriately with the items. Should rudimently simulate depth

//rework things to access globals to functions, or take them as parameters.

//get a better menu system going. Have an okay basic nesting setup. But ideally I would have it be fancier
//like allow submenus, basically have some way of dividing a menu into rectangles, each of those contain menues

int main(int argc, char** args) {
  //  quit = 0;
  if (init() == 0) {
    if (loadMedia() == 0) {
      SDL_Texture* logo = loadTexture("/home/nudon/prg/gam/sdl/pocus.png");
      logo = NULL;
      if (logo != NULL) {
	SDL_Renderer* rend = getRenderer();
	SDL_RenderClear(rend);
	SDL_RenderCopy(rend, logo, NULL, NULL);
	SDL_RenderPresent(rend);
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

static int startDebug() {
  int fail = 0;
  setActiveMap(debugMap());
  //makeSections(activeMap);
  if (getActiveMap() != NULL) {
    setMapBG(cinterTiles(getActiveMap()));
    if (getMapBG() != NULL) {
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


void gameLoop() {
  SDL_Renderer* gRan = getRenderer();
  setDrawnMap(getActiveMap(), getCameraPos());
  
  //orphan code, needs a home
  NPC_move_list* mapEditList = createNPC_move_list();
  NPC_t* debug = createNPC();
  setNPCPositionByCord(debug, 0, 0);
  makeDebug(debug);
			 
  addNPCToList(debug, mapEditList->idleList);
  //offer your support today

  
  while(getGameState() != GAMEQUIT) {
    setBGColor(0,0,0,0xff);
    SDL_RenderClear(gRan);
    //Standard I've been thinking about
    //have each gameMstate assosiate with some function,
    //wrap whatever's in the code block within a function
    //want to keep the number of seperate drawLoops going on down to one
    //so for menu's and all that, want to develop actions to not be a loop
    SDL_RenderCopy(gRan, getMapBG(), getDrawMap(), getDrawScreen());
    drawAllNPCS(getActiveMap()->allNPCS);
    //gameRun functions
    if(getGameState() == GAMERUN) {
      pickDestLoop(getActiveMap()->allNPCS);
      moveDestLoop(getActiveMap()->allNPCS);
      setDrawnMap(getActiveMap(), getCameraPos());
    }
    else if (getGameState() == GAMEMAPEDIT) {
      pickDestLoop(mapEditList);
      moveDestLoop(mapEditList);
      drawAllNPCS(mapEditList);
    }
    //gamePause functions
    if(getGameState() == GAMEPAUSE) {
      //currently, this is causing a segfault
      //pretty sure it was an issue with me adding a new field to menustruct
      //clearing object files and making fresh worked
      getActiveMenu()->action();
    }
    
    SDL_Delay(updateWait);
    SDL_RenderPresent(gRan);
  }
} 

int init() {
  SDL_Renderer* rend = NULL;
  setCameraPos(malloc(sizeof(npc_pos_t)));
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
	    setActiveMenu(NULL);
	    setMainMenu(createMainMenu());
	    setMapEditMenu(createMapEditMainMenu());
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
  int fail = 0;
  return fail;
}

void myClose() {
  freeMap(getActiveMap());
  SDL_DestroyTexture(currMapBG);
  currMapBG = NULL;
  freeRenderer();
  freeMenu(getMainMenu());
  SDL_DestroyWindow(gWin);
  gWin = NULL;
  TTF_Quit();
  IMG_Quit();
  free(currMapBG);
  SDL_Quit();
}


void setBGColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
}


