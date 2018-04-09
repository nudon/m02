#ifndef FILE_MYINPUT_SEEN
#define FILE_MYINPUT_SEEN

#include "myNPC.h"
#include "myMap.h"

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

void characterInput(npcNode* node);

void menuInput();

int handleSingleInput(npcNode* npcNode, SDL_Event* e);

int handleMapEditInput(npcNode* npcNode, SDL_Event* e);

int basicMenuInputHandler(SDL_Event* e);

int handleTextEntry(SDL_Event* e);

void updateKeys(SDL_Event* e);

void updateMap(map* theMap);

int checkAndUpdateKey(keyArg key, SDL_Event* e);

char* getTempString();


void setStringField(char* fieldArg);
void setStringTemp(char* tempArg);
void setIntField(int* intArg);
void setIntTemp(int tempArg);

#endif
