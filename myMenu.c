#include "myMenu.h"
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

  //ideally I'd have this be proportional to either screen dim
  //or at least the menu dim. 
  new->entryHeight = 48;
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
  height = theMenu->entryHeight;
  SDL_Rect dstRect;
  dstRect.x = subX;
  dstRect.y = subY;
  dstRect.h = height;
  
  SDL_Surface* textSurf;
  menu* menuEntry;
  for (int i = 0 ; i < theMenu->arrayBound ; i++) {
    menuEntry = &(theMenu->menuEntries[i]);
    //this line apparenly doesn't do anything
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
  
  SDL_RenderCopy(rend, theMenu->menuImage, NULL, &dstRect);

  //so, how to indicate active entry
  //have some SDL_texture, eventually
  //text aligning has worked nicely, but I'm not exactly sure about how it's being nice
  //for example, originally planned text to just start at top of menu, but it doesn't.
  //it's kind of centered in the middle. Also, seems like changing the font type changes allignment
  //so Until I pin that down, just get a rough approximation for where to draw the circle
  
  int x = (SCREEN_WIDTH) * 1 / 4;
  int y = (SCREEN_HEIGHT - theMenu->height) / 2;
  y += (theMenu->activeIndex + 1) * theMenu->entryHeight + 25 ;
  
  SDL_SetRenderDrawColor( rend, 0xff, 0xff, 0xff, 0xFF );
  myDrawCirc(x , y , theMenu->entryHeight / 3);
  
}



