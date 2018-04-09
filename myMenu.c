#include <stdlib.h>
//#include <SDL2/SDL.h>
#include "myImage.h"
#include "myInput.h"
#include "myList.h"
#include "myMenu.h"
#include "gameState.h"
#include "myEnv.h"
#include "systemLimits.h"

//have a spartan menu system now. Would like to eventually change it to allow submenues to sometimes  be displayed in full

//so, menus. important bit about transitions, the main menu should have an appropriate transitionTo____ as it's action. 

//basically individual character dimensions
int ENTRY_HEIGHT= 20 ;
int ENTRY_WIDTH = 14;
static SDL_Color menuTextCol = {.r = 235, .g = 241, .b = 190, .a = 0};
static SDL_Color menuBGCol = {.r = 17, .g = 23, .b = 18, .a = 0};
static SDL_Color inputBGCol = {.r = 243, .g = 221, .b = 233, .a = 0};
static SDL_Color inputTextCol = {.r = 11, .g = 7, .b = 9, .a = 0};


static menu* createResumeMenu(menu* parent);
static menu* createMapEditMenu(menu* parent);
static menu* createQuitMenu(menu* parent);
static menu* createMenu();
static menu* createTileStructInput(menu* parent, char* message);
static menu* createMapStructInput(menu* parent, char* message);
static menu* createStringInput(menu* parent, char* message);
static menu* createMenuOpt(int w, int h, char* msg);
static menu* createTileStructEntry(menu* parent);
static menu* createMapStructEntry(menu* parent);
static void setFont(TTF_Font* font, char* fontPath);
static void quitAction();
static void textEntryAction();
static void freeMenuSub(menu* aMenu);

//extern int SCREEN_WIDTH;
//extern int SCREEN_HEIGHT;

//static menu* currentMenu;

//static menu* mainMenu;

//static menu* mapEditMenu;

char* TILE_EDIT_MENU = "Edit Tile";
char* MAP_EDIT_MENU = "Edit Map";
char* STRING_TILE_BG_PATH = "Path to BG";
char* INT_TILE_WALL = "Wall status";
char* INT_MAP_ROWS = "Rows";
char* INT_MAP_COLS = "Cols";

static TTF_Font* globalFont = NULL;

static void setGlobalFont(char* fontPath) {
  globalFont = TTF_OpenFont(fontPath, 15);
  if (globalFont == NULL) {
    fprintf(stderr, "Error on loading font, %s\n", TTF_GetError());
  }
}



static void setFont(TTF_Font* font, char* fontPath) {
  font = TTF_OpenFont(fontPath, 60);
  if (font == NULL) {
    fprintf(stderr, "Error on loading font, %s\n", TTF_GetError());
  }
}

static menu* createMenu() {
  return createMenuOpt(SCREEN_WIDTH * 2 / 3,  SCREEN_HEIGHT * 2 / 3, "text");
}

static menu* createMenuOpt(int w, int h, char* msg) {
  menu * new = malloc(sizeof(menu));
  new->width = w;
  new->height = h;
  new->bgColor = &menuBGCol;
  new->menuImage = NULL;

  //ideally I'd have this be proportional to either screen dim
  //or at least the menu dim. 
  new->entryHeight = ENTRY_HEIGHT ;
  new->menuEntries = NULL;
  new->activeIndex = 0;
  new->arrayBound = 0;
  
  new->parent = NULL;
  //new->returnState = GAMERUN;
  //new->newState = MENU;
  
  new->text = msg;
  new->textColor = &menuTextCol;

  if (globalFont == NULL) {
    setGlobalFont("aFont.ttf");
    if (globalFont == NULL) {
      fprintf(stderr, "if you don't want comicSans, place some other ttf in local dir, name it aFont.ttf\n");
      //got this particular font from https://github.com/antimatter15/doge/blob/master/Comic%20Sans%20MS.ttf
      //who didn't make it, but Vincent Connare apparently did.
      //setFont(globalFont, "comicSans.ttf");
      setGlobalFont("comicSans.ttf");
    }
  }
  new->font = globalFont;
  new->action = NULL;
  
  //new->action = &basicMenuAction;
  return new;
}





//have to do some odd things for freeing, since I'm mallocing a whole block of things at once
//in mainProcedure, call freeSub on all menuEntries, then free menuEntries, then free menu
//in subProcedure, call freeMenu on all the menuEntries,freet the menuEntries, but don't menu,

void freeMenu(menu* aMenu) {
  for(int i = 0; i < aMenu->arrayBound ; i++) {
    freeMenuSub(&(aMenu->menuEntries[i]));
  }
  free(aMenu->menuEntries);
  free(aMenu);
}

static void freeMenuSub(menu* aMenu) {
  for(int i = 0; i < aMenu->arrayBound ; i++) {
    freeMenuSub(&(aMenu->menuEntries[i]));
  }
  free(aMenu->menuEntries);
}

//specific menu
menu* createMainMenu() {
  int count = 0;
  menu* mainMenu = createMenu();
  mainMenu->text = "MainMenu";
  //mainMenu->newState = GAMERUN;
  mainMenu->arrayBound = 3;
  mainMenu->menuEntries = malloc(sizeof(menu) * mainMenu->arrayBound);
  mainMenu->menuEntries[count++] = *createResumeMenu(mainMenu);
  mainMenu->menuEntries[count++] = *createMapEditMenu(mainMenu);
  mainMenu->menuEntries[count++] = *createQuitMenu(mainMenu);
  mainMenu->action = transitionToGameRun;
  fillMenu(mainMenu);

  return mainMenu;
}

static menu* createResumeMenu(menu* parent) {
  menu* resume = createMenu();
  resume->text = "Resume";
  //maintainState(resume, parent);
  //resume->newState = GAMERUN;
  resume->arrayBound = 0;
  resume->menuEntries = NULL;

  //resume->action = &resumeAction;
  resume->parent = parent;
  resume->action = transitionToGameRun;
  return resume;
}


static menu* createMapEditMenu(menu* parent) {
  menu* mapEdit = createMapEditMainMenu();
  //maintainState(mapEdit, parent);
  //mapEdit->newState = GAMEMAPEDIT;
  mapEdit->text = "Edit map";
  mapEdit->arrayBound = 0;
  mapEdit->menuEntries = NULL;

  mapEdit->parent = parent;
  mapEdit->action = transitionToMapEdit;
  return mapEdit;
}



static menu* createQuitMenu(menu* parent) {
  menu* quitMenu = createMenu();
  quitMenu->text = "Quit";
  //quitMenu->newState = GAMEQUIT
  quitMenu->arrayBound = 0;
  quitMenu->menuEntries = NULL;
  quitMenu->action = quitAction;
  return quitMenu;
}

static void quitAction() {
  // setActiveMenu(NULL);
  setGameState(GAMEQUIT);
}



//map edit things

menu* createMapEditMainMenu() {
  int count = 0;
  menu* new = createMenu();
  new->text = "Edit menu";
  //new->returnState = GAMEMAPEDIT;
  //new->newState = MENU;
  new->arrayBound = 4;
  new->menuEntries = malloc(sizeof(menu) * new->arrayBound);
  menu* resume =  createMenu(new);
  resume->action = transitionToMapEdit;
  resume->text = "resume map editing";
  resume->parent = new;
  new->menuEntries[count++] = *resume;
  resume =  createMenu(new);
  resume->action = transitionToGameRun;
  resume->text = "Back to regular game";
  resume->parent = new;
  new->menuEntries[count++] = *resume;
  //at least want a menu for each thing to edit
  //want tile, and map size
  new->menuEntries[count++] = *createTileStructEntry(new);
  new->menuEntries[count++] = *createMapStructEntry(new);
  //new->menuEntries[count++] = something for map dimension
  new->action = transitionToMapEdit;
  fillMenu(new);
  
  return new;  
}
static menu* createTileStructEntry(menu* parent) {
  int count = 0;
  menu* new = createMenu();
  new->text = TILE_EDIT_MENU;
  //maintainState(new, parent);
  new->arrayBound = 3;
  new->menuEntries = malloc(sizeof(menu) * new->arrayBound);
    new->bgColor = &menuBGCol;
  new->textColor = &menuTextCol;
  new->menuEntries[count++] = *createTileStructInput(new,STRING_TILE_BG_PATH);
  new->menuEntries[count++] = *createTileStructInput(new,"tile contents menu (under construction)");
  new->menuEntries[count++] = *createTileStructInput(new,INT_TILE_WALL);
  new->bgColor = &menuBGCol;
  new->textColor = &menuTextCol;
  new->parent = parent;
  new->action = NULL;
  fillMenu(new);
  return new;
}

static menu* createMapStructEntry(menu* parent) {
  int count = 0;
  menu* new = createMenu();
  new->text = MAP_EDIT_MENU;
  new->arrayBound = 2;
  new->menuEntries = malloc(sizeof(menu) * new->arrayBound);
  new->menuEntries[count++] = *createMapStructInput(new, INT_MAP_ROWS);
  new->menuEntries[count++] = *createMapStructInput(new, INT_MAP_COLS);    
  new->bgColor = &menuBGCol;
  new->textColor = &menuTextCol;
  new->parent = parent;
  new->action = NULL;
  fillMenu(new);
  return new;
}

static menu* createTileStructInput(menu* parent, char* message) {
  menu* new = createStringInput(parent, message);
  new->action = setTileStructField;
  return new;
}

static menu* createMapStructInput(menu* parent, char* message) {
  menu* new = createStringInput(parent, message);
  new->action = setMapStructField;
  return new;
}

static menu* createStringInput(menu* parent, char* message) {
  menu* new = createMenu();
  new->width = 1;
  new->height = 1;
  
  
  new->text = message;
  new->bgColor = &inputBGCol;
  new->textColor = &inputTextCol;
  new->parent = parent;
  new->action = textEntryAction;
  fillMenu(new);
  
  return new;
}

static void textEntryAction() {
  setGameState(MENUTEXTENTRY);
}


void fillMenu(menu* theMenu) {
  int height;
  SDL_Rect dstRect;
  SDL_Surface* temp = createSurfaceFromDim(theMenu->width, theMenu->height);
  height = theMenu->entryHeight;
  SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format,
				      theMenu->bgColor->r,
				      theMenu->bgColor->g,
				      theMenu->bgColor->b));


  dstRect.x = 0;
  dstRect.y = 0;
  dstRect.h = height;
  printf("height is %d\n" , height);
  SDL_Surface* textSurf;
  menu* menuEntry;
  for (int i = 0 ; i < theMenu->arrayBound ; i++) {
    menuEntry = &(theMenu->menuEntries[i]);
    dstRect.w = strlen(menuEntry->text) * 0;
    textSurf = TTF_RenderText_Solid(menuEntry->font, menuEntry->text, *(theMenu->textColor));
    SDL_BlitSurface(textSurf, NULL, temp, &dstRect);
    SDL_FreeSurface(textSurf);
    dstRect.y += height;
  }
  theMenu->menuImage = loadSurfaceToTexture(temp);
  SDL_FreeSurface(temp);
}


void drawCurrentMenu() {
  SDL_Renderer* rend = getRenderer();
  menu* theMenu = getMenu(currentEnv);
  SDL_Rect dstRect;
  
  dstRect.w = theMenu->width;
  dstRect.h = theMenu->height;
  dstRect.x = (SCREEN_WIDTH  - theMenu->width) / 2;
  dstRect.y = (SCREEN_HEIGHT - theMenu->height) / 2;
  //draws menu
  SDL_RenderCopy(rend, theMenu->menuImage, NULL, &dstRect);

  dstRect.y += theMenu->activeIndex * theMenu->entryHeight;
  dstRect.w = strlen(theMenu->menuEntries[theMenu->activeIndex].text) * ENTRY_WIDTH;
  dstRect.h = theMenu->entryHeight;
  //draw selected entry box
  SDL_Color a = *theMenu->textColor;
  SDL_SetRenderDrawColor(rend, a.r, a.g, a.b, a.a);
  SDL_RenderDrawRect(rend, &dstRect);  
}







