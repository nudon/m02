#ifndef FILE_MYMENU_SEEN
#define FILE_MYMENU_SEEN

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "gameState.h"

struct menu_struct {
  //specifice of overall box. menuImage is the box with bg color with drawn in menu entries
  int width;
  int height;
  SDL_Color* bgColor;
  SDL_Texture* menuImage;

  //list of menu entries for the menu
  int entryHeight;
  struct menu_struct * menuEntries;
  int activeIndex;
  int arrayBound;

  //self explanatory
  struct menu_struct * parent;
  
  //if the menu has a parent, this is what will be shown in the parent for the menu entry
  char* text;
  SDL_Color* textColor;
  TTF_Font* font;
  gameState newState;
  //used for transitioniong between states/environments
  void (*action) ();
};

typedef struct menu_struct menu;

extern char* TILE_EDIT_MENU;
extern char* MAP_EDIT_MENU;
extern char* STRING_TILE_BG_PATH;
extern char* INT_TILE_WALL;
extern char* INT_MAP_ROWS;
extern char* INT_MAP_COLS;
extern char* SAVE_MAP;
extern char* LOAD_MAP;

extern int fieldType;
extern const int STRING_CHANGE;
extern const int INT_CHANGE; 
extern int ENTRY_HEIGHT;
extern int ENTRY_WIDTH;
menu* getActiveMenu();

menu* getMainMenu();

menu* getMapEditMenu();

void setActiveMenu(menu* newMenu);

void setMainMenu(menu* newMenu);

void setMapEditMenu(menu* newMenu);

menu* createMainMenu();

menu* createMapEditMainMenu();

void freeMenu(menu* aMenu);

void fillMenu(menu* theMenu);

void drawCurrentMenu();

void drawTextEntry();

void drawText(TTF_Font* font, char* text, SDL_Color* textColor, SDL_Rect* dstRect);





#endif
