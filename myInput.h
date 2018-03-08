#ifndef FILE_MYINPUT_SEEN
#define FILE_MYINPUT_SEEN

#include "myNPC.h"

enum keys {
  KEY_t,
  KEY_m,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ESCAPE,
  KEY_RETURN,
  KEY_LAST
};

typedef enum keys keyArg;

char* getTempString();

void setTempString(char* new);

void characterInput(NPC_node_t* node);

void menuInput();

int handleSingleInput(NPC_node_t* npcNode, SDL_Event* e);

int handleMapEditInput(NPC_node_t* npcNode, SDL_Event* e);

int basicMenuInputHandler(SDL_Event* e);

int handleTextEntry(SDL_Event* e);
  
void updateKeys(SDL_Event* e);

int checkAndUpdateKey(keyArg key, SDL_Event* e);

#endif
