#include "myInput.h"
//so, got pause working. Only issue now is that I reintroduces diagonal movement
//When I was just acting upon first input, the keyup releases of esc got tossed
//so I had to push it twice to unpause
//better solution would be to have 2 different functions for handling keydown/keyup inputs
//only have one keydown be processed per call to singleInput, but have multiple keyUps be processed
//key states for SDL_keysym keycodes


static uint32_t keyStates[KEY_LAST];

extern int quit;

void singleInput(NPC_node_t* npcNode) {
  SDL_Event e;
   while(SDL_PollEvent(&e) != 0 ) {
    if (e.type == SDL_QUIT) {
      quit = 1;
    }
    else if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)) {
      handleSingleInput(npcNode, &e);
    }
  }
}

int handleSingleInput(NPC_node_t* npcNode, SDL_Event* e) {
  //specifics for this, returns a positive number if recieved input that means something
  //negative number if key pressed meant nothing
  //currently not used in any way, it's just there. 
  int val = -1;
  tile_pos_t* pos =  npcNode->storedNPC->position;
  keyArg activeKey;
  switch (e->key.keysym.sym){
  case SDLK_UP:
    if (e->type == SDL_KEYDOWN) {
      setDestByCord(npcNode, pos->posX, pos->posY - 1);
      val = 1;
    }
    break;		
  case SDLK_DOWN:
    if (e->type == SDL_KEYDOWN) {
      setDestByCord(npcNode, pos->posX, pos->posY + 1);
      val = 1;
    }
    break;
  case SDLK_LEFT:
    if (e->type == SDL_KEYDOWN) {
      setDestByCord(npcNode, pos->posX - 1, pos->posY);
      val = 1;
    }
    break;
  case SDLK_RIGHT:
    if (e->type == SDL_KEYDOWN) {
      setDestByCord(npcNode, pos->posX + 1, pos->posY);
      val = 1;
    }
    break;
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      if (e->type == SDL_KEYDOWN) {
	state = GAMEPAUSE;
	val = 1;
      }
    }
  default:
    break;      
  }
  return val;
}

void pauseMenu() {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      quit = 1;
    }
    handlePauseInput(&e);
  }
}

int handlePauseInput(SDL_Event* e) {
  int val = -1;
  keyArg activeKey;
  switch(e->key.keysym.sym){
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      if (e->type == SDL_KEYDOWN) {
	state = GAMERUN;
	val = 1;
      }
    }
  default:
    break;
  }
  return val;
}

int checkAndUpdateKey(keyArg key, SDL_Event* e) {
  //returns 0 if key didn't change state, 1 if it did
  int val = 0;
  if (keyStates[key] != e->type) {
    keyStates[key] = e->type;
    val = 1;
  }
  return val;
}
int keyDownCheck(keyArg key, SDL_Event* e) {
  //just a combinaiton of checking if key changed, and if it's a keyDown. 
  int val;
  if (checkAndUpdateKey(key, e) && e->type == SDL_KEYDOWN) {
    val = 1;
  }
  else {
    val = 0;
  }
  return val;
}

int keyUpCheck(keyArg key, SDL_Event* e) {
  int val;
  if (checkAndUpdateKey(key, e) && e->type == SDL_KEYUP) {
    val = 1;
  }
  else {
    val = 0;
  }
  return val;
}


//also, think I want to have two seperate functions for a box
//one for handling input, another being a generic function
//trade off to generiic function is that It'd have a generic arguement
//so either nothing, or a void*, and I wrap whatever I want to pass into a struct

//basic menu input handler
//either have a global menu for active menu
//or pass in an arguement
//basically, up/down changes activeItem
//esc goes to parent menu
//ret either enters menu, or executes a fuction
//basically intend to design them like a union, so only leaf menus have non null actions
void basicAction(SDL_Event* e) {
  keyArg key;
  menu* activeMenu;
  switch(e->key.keysym.sym) {
  case SDLK_DOWN:
    key = KEY_DOWN;
    if ( keyDownCheck(key, e)) {
      if (activeMenu->activeItem->next == NULL) {
	activeMenu->activeItem = activeMenu->itemList->start;
      }
    }
    else {
      activeMenu->activeItem = activeMenu->activeItem->next;
      }
    break;
    
  case SDLK_UP:
    key = KEY_UP;
    if (keyDownCheck(key, e)) {
      if (activeMenu->activeItem->prev == NULL) {
	activeMenu->activeItem = activeMenu->itemList->end;
      }
    }
    else {
      activeMenu->activeItem = activeMenu->activeItem->prev;
    }
    break;

  case SDLK_RETURN:
    key = KEY_RETURN;
    //do things associated with active menu, probably
    if (keyDownCheck(key, e)) {
      if (activeMenu->activeItem != NULL) {
	activeMenu = activeMenu->activeItem->stored;
	/*
	if (activeMenu->action != NULL) {
	  //likely to blo
	  activeMenu->action();
	  activeMenu = activeMenu->parent;
	}
	*/	
      }
    }
    break;

  case SDLK_ESCAPE:
    key = KEY_ESCAPE;;
    if (keyDownCheck(key, e)) {
      if (activeMenu->parent != NULL) {
	activeMenu = activeMenu->parent;
	  }
    }
    else {
      //probably change state to gameRun
    }
    break;

  default:

    break;
  }
}
