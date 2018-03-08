#include <stdlib.h>
//#include <SDL2/SDL.h>
#include "myImage.h"
#include "myInput.h"
#include "myList.h"
#include "myMenu.h"
#include "gameState.h"
//so, slight redesign
//idea is, create the menus, and their submenus, and so on.
//then, maybe after, or maybe only on pulling up a menu, set the menuImage
//menuImage is a texture, of the widht and height, of the bgColor, with all of the submenu text being drawn

//also, working with arrays now, since I'm not going to be adding/removing menu lists on the go
//so structure of menus will be super hardcoded.

//have a spartan menu system now. Would like to eventually change it to allow submenues to sometimes  be displayed in full

static menu* createResumeMenu(menu* parent);
static menu* createMapEditMenu(menu* parent);
static menu* createQuitMenu(menu* parent);
static menu* createMenu();
static menu* createStringInput(menu* parent, char* message);
static menu* createMenuOpt(int w, int h, char* msg);
static menu* createTileStructEntry();
static void setFont(TTF_Font* font, char* fontPath);
static void basicMenuAction();
static void resumeAction();
static void mapEditAction();
static void quitAction();
static void textEntryAction();
static void freeMenuSub(menu* aMenu);

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

static menu* currentMenu;

static menu* mainMenu;

static menu* mapEditMenu;

static TTF_Font* globalFont = NULL;

menu* getActiveMenu() {
  return currentMenu;
}

menu* getMainMenu() {
  return mainMenu;
}

menu* getMapEditMenu() {
  return mapEditMenu;
}

void setActiveMenu(menu* newMenu) {
  currentMenu = newMenu;
}

void setMainMenu(menu* newMenu) {
  mainMenu = newMenu;
}

void setMapEditMenu(menu* newMenu) {
  mapEditMenu = newMenu;
}

static void setGlobalFont(char* fontPath) {
  globalFont = TTF_OpenFont(fontPath, 30);
  if (globalFont == NULL) {
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
  new->bgColor = malloc(sizeof(SDL_Color));
  new->bgColor->r = 17;
  new->bgColor->g = 23;
  new->bgColor->b = 18;
  new->menuImage = NULL;

  //ideally I'd have this be proportional to either screen dim
  //or at least the menu dim. 
  new->entryHeight = 48;
  new->menuEntries = NULL;
  new->activeIndex = 0;
  new->arrayBound = 0;
  
  new->parent = NULL;
  new->returnState = 0;

  new->text = msg;
  new->textColor = malloc(sizeof(SDL_Color));
  new->textColor->r = 235;
  new->textColor->g = 241;
  new->textColor->b = 190;

  setGlobalFont("aFont.ttf");
  if (globalFont == NULL) {
      fprintf(stderr, "if you don't want comicSans, place some other ttf in local dir, name it aFont.ttf\n");
      //got this particular font from https://github.com/antimatter15/doge/blob/master/Comic%20Sans%20MS.ttf
      //who didn't make it, but Vincent Connare apparently did.
      //setFont(globalFont, "comicSans.ttf");
      setGlobalFont("comicSans.ttf");
  }
  new->font = globalFont;
  
  new->action = &basicMenuAction;
  return new;
}

static void basicMenuAction() {
  drawCurrentMenu();
  menuInput();
}

static void setFont(TTF_Font* font, char* fontPath) {
  font = TTF_OpenFont(fontPath, 30);
  if (font == NULL) {
    fprintf(stderr, "Error on loading font, %s\n", TTF_GetError());
  }
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
  mainMenu->returnState = GAMERUN;
  mainMenu->arrayBound = 3;
  mainMenu->menuEntries = malloc(sizeof(menu) * mainMenu->arrayBound);
  mainMenu->menuEntries[count++] = *createResumeMenu(mainMenu);
  mainMenu->menuEntries[count++] = *createMapEditMenu(mainMenu);
  mainMenu->menuEntries[count++] = *createQuitMenu(mainMenu);
  fillMenu(mainMenu);

  return mainMenu;
}


static menu* createResumeMenu(menu* parent) {
  menu* resume = createMenu();
  resume->text = "Resume";
  resume->arrayBound = 0;
  resume->menuEntries = NULL;
  resume->action = &resumeAction;
  resume->parent = parent;
  return resume;
}

static void resumeAction() {
  setActiveMenu(NULL);
  setGameState(GAMERUN);
}
static menu* createMapEditMenu(menu* parent) {
  menu* mapEdit = createMapEditMainMenu();
  mapEdit->action = &mapEditAction;
  
  mapEdit->text = "Edit map";
  mapEdit->arrayBound = 0;
  mapEdit->menuEntries = NULL;

  mapEdit->parent = parent;
  
  return mapEdit;
}

static void mapEditAction() {
  setActiveMenu(NULL);
  setGameState(GAMEMAPEDIT);
}

static menu* createQuitMenu(menu* parent) {
  menu* quitMenu = createMenu();
  quitMenu->text = "Quit";
  quitMenu->arrayBound = 0;
  quitMenu->menuEntries = NULL;
  quitMenu->action = &quitAction;
  return quitMenu;
}

static void quitAction() {
  setActiveMenu(NULL);
  setGameState(GAMEQUIT);
}

//map edit things

//little excercize on how I'd implement a basic adjustment action
//whose goal was to manipulate and set a volume slider
//basically, there'd be some global variable to keep track of a state across function calls
//idealy set up before the first action call to point to some actual data field
//basic action is calling a unique input handler and drawCall
//also, would need gloabal variables total. 1 for temporary storage, other for actual commited change
//probably going to need to replicate this with ints/floats
//includes the custome input handler, the new state, the getter funcs



menu* createMapEditMainMenu() {
  int count = 0;
  menu* new = createMenu();
  new->text = "Edit menu";
  new->returnState = GAMEMAPEDIT;
  new->arrayBound = 2;
  new->menuEntries = malloc(sizeof(menu) * new->arrayBound);
  new->menuEntries[count++] = *createResumeMenu(mainMenu);
  //at least want a menu for each thing to edit
  //want tile, and map size
  new->menuEntries[count++] = *createTileStructEntry();
  //new->menuEntries[count++] = something for map dimension
  fillMenu(new);
  
  return new;  
}
static menu* createTileStructEntry() {
  int count = 0;
  menu* new = createMenu();
  new->text = "Tile Menu";
  new->returnState = GAMEMAPEDIT;
  new->arrayBound = 3;
  new->menuEntries = malloc(sizeof(menu) * new->arrayBound);
  new->menuEntries[count++] = *createStringInput(new,"Path to tile background");
  new->menuEntries[count++] = *createStringInput(new,"tile contents menu (under construction)");
  new->menuEntries[count++] = *createStringInput(new,"wall status");
  fillMenu(new);
  return new;
}

static menu* createStringInput(menu* parent, char* message) {
  //think I'll want the action to be something to get input
  //then draw the current text input
  //I'm forseeing that I'll be needing to use the char* temp from myInput
  //so atleast need a getter function for that
  menu* new = createMenu();
  new->returnState = GAMEMAPEDIT;
  new->width = 1;
  new->height = 1;
  
  new->text = message;
  
  new->action = &textEntryAction;
  new->parent = parent;
  fillMenu(new);
  
  return new;
}

static void textEntryAction() {
  menuInput();
  //no , bad
  //drawCurrentMenu();
  drawTextEntry();
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
  SDL_Surface* textSurf;
  menu* menuEntry;
  for (int i = 0 ; i < theMenu->arrayBound ; i++) {
    menuEntry = &(theMenu->menuEntries[i]);
    dstRect.w = strlen(menuEntry->text) * 0;
    textSurf = TTF_RenderText_Solid(menuEntry->font, menuEntry->text, *(menuEntry->textColor));
    SDL_BlitSurface(textSurf, NULL, temp, &dstRect);
    SDL_FreeSurface(textSurf);
    dstRect.y += height;
  }
  theMenu->menuImage = loadSurfaceToTexture(temp);
  SDL_FreeSurface(temp);
}


void drawCurrentMenu() {
  SDL_Renderer* rend = getRenderer();
  menu* theMenu = currentMenu;
  SDL_Rect dstRect;
  
  dstRect.w = theMenu->width;
  dstRect.h = theMenu->height;
  dstRect.x = (SCREEN_WIDTH  - theMenu->width) / 2;
  dstRect.y = (SCREEN_HEIGHT - theMenu->height) / 2;
  //draws menu
  SDL_RenderCopy(rend, theMenu->menuImage, NULL, &dstRect);

  dstRect.y += theMenu->activeIndex * theMenu->entryHeight;
  dstRect.w = strlen(theMenu->menuEntries[theMenu->activeIndex].text) * 20;
  dstRect.h = theMenu->entryHeight;
  //draw selected entry box
  SDL_RenderDrawRect(getRenderer(), &dstRect);  
}


void drawTextEntry() {
  //basically, just render some text to center of screen
  //backgroun would also be nice

  /*
  some colors I wanted
  bgColor->r = 243;
  bgColor->g = 221;
  bgColor->b = 233;

  textColor->r = 11;
  textColor->g = 7;
  textColor->b = 9;
*/
  SDL_Rect dstRect;
  char* text = getTempString();
  dstRect.h = currentMenu->entryHeight;
  dstRect.y = (SCREEN_HEIGHT - dstRect.h) / 2;
  dstRect.w = strlen(text) * 20;
  dstRect.x = (SCREEN_WIDTH - dstRect.w) / 2;
  SDL_RenderCopy(getRenderer(), currentMenu->menuImage, NULL, &dstRect);
  drawText(currentMenu->font, text, currentMenu->textColor, &dstRect);
}


