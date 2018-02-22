#include "myMenu.h"
//setting up some menu primitives
//first would just be creating boxes
//then creating boxes with things in them, ideally text
//then having some way of keeping track of which box I'm on,
//and what to do if I'm on a box and a button is pressed
//for keeping track of actions, thinking of creating a struct
//who points to some function, in order to tie a box to doing something

//also, looks like sdl has a simple message box, don't think I can use it much
//seemed like the only feedback caller got from box was which button was pushed

//think I might have a function for drawing "small menus" for displaying lists
//basically, draw the main menu, then for each item in list
//just draw a sequence of small boxes with text
//issue of too many boxes to display in window size
//basic Idea is just have a seperate menu in that case, and have a next/prev button go between the two
//could either do that by hand, or somehow automatically
//could do that by calling a function when I've added all the items I intend to a given menu


extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

static menu currentMenu;

menu* createMenu() {
  return createMenuOpt(SCREEN_WIDTH / 3,  SCREEN_HEIGHT / 3, "text");
}

menu* createMenuOpt(int w, int h, char* msg) {
  menu * new = malloc(sizeof(menu));
  new->width = w;
  new->height = h;
  new->text = msg;
  return new;
}

void addItemToMenu(menu* dest, menu* toAdd) {
//
//appendToGen_list(dest->itemList, createNewGen_node(toAdd));
}

//suppose there's going to be 2 types of menu items
//one where, the action associated would just be pull up some other menu
//or another, where it's going to execute some special code and not pull a menu

void drawMenu(menu* theMenu) {
  //first, draw main menu. Assume allignment or specify it as a field
  //then, draw submenu entries,
//first, assuming allignement/screenposition is non specified
//SDL_Renderer* 
int x = (SCREEN_WIDTH  - theMenu->width) / 2;
int y = (SCREEN_HEIGHT - theMenu->height) / 2;
SDL_Surface* temp = createSurfaceFromDim(theMenu->width, theMenu->height);
SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format,
				       theMenu->bgColor->r,
				       theMenu->bgColor->g,
				       theMenu->bgColor->b));


//so, convert this to a texture, rendur to screen
//then do the thing where I draw the tiny boxes
//have to decide whether I keep passing having gRan be an extern global
//or if I start passing it down the grapeVine

}
