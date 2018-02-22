#ifndef FILE_MYMENU_SEEN
#define FILE_MYMENU_SEEN
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "myImage.h"
#include "myList.h"


struct menu_struct {
  int width;
  int height;
  char* text;
  SDL_Color* bgColor;
  SDL_Color* textColor;
  struct menu_struct * parent;
  gen_list* itemList;
  gen_node* activeItem;
  int activeIndex;
  //think I want this void
  void (*inputHandler) (SDL_Event* e);
  void (*action) ();
};

typedef struct menu_struct menu;

menu* createMenu();

menu* createMenuOpt(int w, int h, char* msg);

void addItemToMenu(menu* dest, menu* toAdd);

void drawMenu(menu* theMenu);

#endif
