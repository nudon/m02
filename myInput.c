#include "myInput.h"

//array over enumerations of keys found in header
//stores whether the last associated with the key a keydown or keyup
static uint32_t keyStates[KEY_LAST];

void singleInput(NPC_node_t* npcNode) {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0 ) {
    if (e.type == SDL_QUIT) {
      setGameState(GAMEQUIT);
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
  case SDLK_RETURN:
    //appears empty, just here to detect if/when return key goes up
    //otherwise, have to press it twice sometimes for menuInput to work
    //seems like a safe thing to do, is that for every key i'm keeping track of
    //no matter what input state I'm currently in, have a case to detect key
    //and at least have a call to checkAndUpdateKey in the case
    activeKey = KEY_RETURN;
    checkAndUpdateKey(activeKey, e);
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      if (e->type == SDL_KEYDOWN) {
	setGameState(GAMEPAUSE);
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

void menuInput() {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      setGameState(GAMEQUIT);
    }
    else if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)) {
      basicMenuInputHandler(&e);
    }
  }
}

//basic menu input handler
//basically, up/down changes activeItem
//esc goes to parent menu
//enter/return to call an associated function
void basicMenuInputHandler(SDL_Event* e) {
  menu* activeMenu = getActiveMenu();
  keyArg key;
  switch(e->key.keysym.sym) {
  case SDLK_DOWN:
    key = KEY_DOWN;
    if ( keyDownCheck(key, e)) {
      activeMenu->activeIndex += 1;
      if (activeMenu->activeIndex == activeMenu->arrayBound) {
	activeMenu->activeIndex = 0;
      }
    }
    break;
    
  case SDLK_UP:
    key = KEY_UP;
    if (keyDownCheck(key, e)) {
      activeMenu->activeIndex -= 1;
      if (activeMenu->activeIndex == -1) {
	activeMenu->activeIndex = activeMenu->arrayBound - 1;
      }
    }
    break;

  case SDLK_RETURN:
    key = KEY_RETURN;
    if (keyDownCheck(key, e)) {
      setActiveMenu(&(activeMenu->menuEntries[activeMenu->activeIndex]));
    }
    break;
    
  case SDLK_ESCAPE:
    key = KEY_ESCAPE;
    if (keyDownCheck(key, e)) {
      if (activeMenu->parent != NULL) {
	setActiveMenu(activeMenu->parent);
      }
      else {
	setActiveMenu(NULL);
	setGameState(GAMERUN);
      }
    }
    break;

  default:

    break;
  }
}
