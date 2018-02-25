#ifndef FILE_MYINPUT_SEEN
#define FILE_MYINPUT_SEEN

#include "myNPC.h"
#include "gameState.h"
#include "myMenu.h"

enum keys {
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ESCAPE,
  KEY_RETURN,
  KEY_LAST
};

typedef enum keys keyArg;

void singleInput(NPC_node_t* npcNode);

int handleSingleInput(NPC_node_t* npcNode, SDL_Event* e);

void pauseMenu();

int handlePauseInput(SDL_Event* e);

void menuInput();

void basicMenuInputHandler(SDL_Event* e);

int checkAndUpdateKey(keyArg key, SDL_Event* e);

#endif
