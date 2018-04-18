#ifndef FILE_MYENV_SEEN
#define FILE_MYENV_SEEN

#include <SDL2/SDL_image.h>
#include "gameState.h"
//#include "temp.h"
#include "myNPC.h"
#include "myMap.h"
#include "myMenu.h"

//just to pass c compliler, actual definition lies in myEnv.c
typedef struct environ_ environ;

environ* createEnviron();

void freeEnvirons();

extern environ* currentEnv;

void setNpcSet(environ* env, npcSet* new);

npcSet* getNpcSet(environ* env);

void setMap(environ* env, map* newMap);

map* getMap(environ* env);

tileMap* getTileMap(environ* env);

void setMapBG(environ* env, SDL_Texture* newBG);

SDL_Texture* getMapBG(environ* env);

void setMapBG(environ* env, SDL_Texture* newBG);

menu* getMenu(environ* env);

void setMenu(environ* env, menu* newMenu);

void runAction(environ* env);

npc* getControlledNpc(environ * env);

npcNode* getControlledNpcNode(environ* env);
void updateMapEditMap();


void setTileStructField();
void setMapStructField();

void setupEnvirons();

void transitionToGameRun();
void transitionToPauseMenu();
void transitionToMapEdit();

void selectMenu(char* entry);

void setupMapSave();
void setupMapLoad();
void saveMap(char* path);
void loadMap(char* path);

#endif 
