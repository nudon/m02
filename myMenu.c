#include "myMenu.h"
//setting up some menu primitives
//first would just be creating boxes
//then creating boxes with things in them, ideally text
//then having some way of keeping track of which box I'm on,
//and what to do if I'm on a box and a button is pressed
//for handling input and for associating actions, just having function pointers

//think I might have a function for drawing "small menus" for displaying lists
//basically, draw the main menu, then for each item in list
//just draw a sequence of small boxes with text
//issue of too many boxes to display in window size
//basic Idea is just have a seperate menu in that case, and have a next/prev button go between the two
//could either do that by hand, or somehow automatically
//could do that by calling a function when I've added all the items I intend to a given menu

//so, slight redesign
//idea is, create the menus, and their submenus, and so on.
//then, maybe after, or maybe only on pulling up a menu, set the menuImage
//menuImage is a texture, of the widht and height, of the bgColor, with all of the submenu text being drawn

//also, working with arrays now, since I'm not going to be adding/removing menu lists on the go
//so structure of menus will be super hardcoded.

static menu* createResumeMenu(menu* parent);

static menu* createMapEditMenu(menu* parent);

static menu* createQuitMenu(menu* parent);

static menu* createMenu();

static menu* createMenuOpt(int w, int h, char* msg);

static void setFont(menu*, char*);

static void basicMenuAction();

static void resumeAction();
  
static void quitAction();

static void freeMenuSub(menu* aMenu);

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

static menu* currentMenu;

static menu* mainMenu;

menu* getActiveMenu() {
  return currentMenu;
}

menu* getMainMenu() {
  return mainMenu;
}

void setActiveMenu(menu* newMenu) {
  currentMenu = newMenu;
}

void setMainMenu(menu* newMenu) {
  mainMenu = newMenu;
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

  new->menuEntries = NULL;
  new->activeIndex = 0;
  new->arrayBound = 0;
  
  new->parent = NULL;

  new->text = msg;
  new->textColor = malloc(sizeof(SDL_Color));
  new->textColor->r = 235;
  new->textColor->g = 241;
  new->textColor->b = 190;
  //  setFont(new,"/usr/share/fonts/DejaVuSansMono.ttf");
  //  setFont(new,"DejaVuSansMono.ttf");
  setFont(new,"aFont.ttf");
  if (new->font == NULL) {
    fprintf(stderr, "if you don't want comicSans, place some other ttf in local dir, name it aFont.ttf\n");
    //got this particular font from https://github.com/antimatter15/doge/blob/master/Comic%20Sans%20MS.ttf
    //who didn't make it, but Vincent Connare apparently did.
    setFont(new, "comicSans.ttf");
  }


  new->inputHandler = &basicMenuInputHandler;
  new->action = &basicMenuAction;
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

static void setFont(menu* theMenu, char* fontPath) {
  TTF_Font* font;
  font = TTF_OpenFont(fontPath, 30);
  if (font == NULL) {
    fprintf(stderr, "Error on loading font, %s\n", TTF_GetError());
  }
  theMenu->font = font;
}

static void basicMenuAction() {
  drawCurrentMenu();
  menuInput();
}

//specific menu

menu* createMainMenu() {
  menu* mainMenu = createMenu();
  mainMenu->text = "MainMenu";
  mainMenu->arrayBound = 3;
  mainMenu->menuEntries = malloc(sizeof(menu) * mainMenu->arrayBound);
  mainMenu->menuEntries[0] = *createResumeMenu(mainMenu);
  //mainMenu->menuEntries[1] = *createResumeMenu(mainMenu);
  mainMenu->menuEntries[1] = *createMapEditMenu(mainMenu);
  mainMenu->menuEntries[2] = *createQuitMenu(mainMenu);
  fillMenu(mainMenu);

  return mainMenu;
}


static menu* createResumeMenu(menu* parent) {
  menu* resume = createMenu();
  resume->text = "Resume";
  resume->arrayBound = 0;
  resume->menuEntries = NULL;
  resume->action = &resumeAction;
  return resume;
}

static void resumeAction() {
  setActiveMenu(NULL);
  setGameState(GAMERUN);
}
static menu* createMapEditMenu(menu* parent) {
  menu* mapEdit = createMenu();
  mapEdit->text = "Edit map";
  mapEdit->arrayBound = 0;
  mapEdit->menuEntries = NULL;
  return mapEdit;
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

void fillMenu(menu* theMenu) {
  int x = (SCREEN_WIDTH  - theMenu->width) / 2;
  int y = (SCREEN_HEIGHT - theMenu->height) / 2;
  SDL_Surface* temp = createSurfaceFromDim(theMenu->width, theMenu->height);
  SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format,
				      theMenu->bgColor->r,
				      theMenu->bgColor->g,
				      theMenu->bgColor->b));

  int subX, subY, height;
  subX = x;
  subY = y;
  height = 48;
  SDL_Rect dstRect;
  dstRect.x = subX;
  dstRect.y = subY;
  dstRect.h = height;
  
  SDL_Surface* textSurf;
  menu* menuEntry;
  for (int i = 0 ; i < theMenu->arrayBound ; i++) {
    menuEntry = &(theMenu->menuEntries[i]);
    dstRect.w = strlen(menuEntry->text) * 20;
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
  
  SDL_RenderCopy(rend, theMenu->menuImage, NULL, &dstRect);
}



