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

void characterInput(npcNode* node);

void menuInput();

int handleSingleInput(npcNode* npcNode, SDL_Event* e);

int handleMapEditInput(npcNode* npcNode, SDL_Event* e);

int basicMenuInputHandler(SDL_Event* e);

int handleTextEntry(SDL_Event* e);
  
void updateKeys(SDL_Event* e);

int checkAndUpdateKey(keyArg key, SDL_Event* e);

void setStringField(char* fieldArg);
void setStringTemp(char* tempArg);
void setIntField(int* intArg);
void setIntTemp(int tempArg);

#endif
