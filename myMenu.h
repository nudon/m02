#ifndef FILE_MYMENU_SEEN
#define FILE_MYMENU_SEEN
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "myImage.h"
#include "myInput.h"
#include "myList.h"
//sort at an impass. how to handle keeping drack of active menu, and handling all the associated actions
//think idea is I'll have some global, and change it in basicMenuInputHandler.
//then, for most menus, have the action associated with it just drawing the current menu
//also, action might include calling the input 
//for some, it will be something like editing the map
//still want actions to not be their own mini-game loops
//so all the framerate things can be handled in one place


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

  void (*inputHandler) (SDL_Event* e);
  //think I want this void
  void (*action) ();
};

typedef struct menu_struct menu;

menu* getActiveMenu();

menu* getMainMenu();

void setActiveMenu(menu* newMenu);

void setMainMenu(menu* newMenu);

menu* createMainMenu(); 

void freeMenu(menu* aMenu);

void fillMenu(menu* theMenu);

void drawCurrentMenu();



#endif
