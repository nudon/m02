#include "myEnv.h"
#include "myImage.h"
#include "myInput.h"

//transitions, are done at end of action, because otherwise changing environ mid-action gets odd
//also, outside of menus, expecting some keypres to bring up menu
//while inside menus, expecting menu-actions to handle transitions, among other things

static void gameRunAction();

static void basicMenuAction();

static void mapEditAction();

static void freeEnviron(environ* env);

void transferToGameRun();
void transferToPauseMenu();
void transferToMapEdit();

struct environ_ {
  npcSet* npcSet;
  map* map;
  void (*action)();
  menu* menu;
};

environ  *gameRun, *pauseMenu, *mapEdit, *gameQuit, *transferEnv;

void setupEnvirons() {
  //each setup. probably just assign the actions and menu
  gameRun = createEnviron();
  pauseMenu =createEnviron();
  mapEdit = createEnviron();
  gameQuit = createEnviron();
  transferEnv = NULL;
  
  //regualr environ
  currentEnv = gameRun;
  gameRun->menu = createMainMenu();
  gameRun->action = gameRunAction;
  gameRun->npcSet = createNpcSet();
  //pausemenu environ
  pauseMenu->action = basicMenuAction;
  //mapedit environ
  currentEnv = mapEdit;
  mapEdit->menu = createMapEditMainMenu();
  mapEdit->action = mapEditAction;
  npcSet* mapEditSet = createNpcSet();
  mapEdit->npcSet = mapEditSet;
  npc* debug = createNpc();
  setTilePosByCord(debug->tilePos, 0, 0);
  allignPixPosToTilePos(debug->pixelPos, debug->tilePos);
  makeDebug(debug);
  addNpc(debug);
  //gameQuit
  

  currentEnv = gameRun;
}

void freeEnvirons() {
  freeEnviron(gameRun);
  freeEnviron(pauseMenu);
  freeEnviron(mapEdit);
}

void transferIfNeeded() {
  if (transferEnv != currentEnv && transferEnv != NULL) {
    if (transferEnv == gameRun) {
      transferToGameRun();
    }
    else if (transferEnv == pauseMenu) {
      transferToPauseMenu();
    }
    else if (transferEnv == mapEdit) {
      transferToMapEdit();
    }
    else {
      fprintf(stderr, "transferEnv got mangled somewhere\n");
    }
  }
}

void transitionToNewEnv(environ* env) {
  transferEnv = env;
}

void transitionToGameRun() {
  transferEnv = gameRun;
}

void transferToGameRun() {
  setGameState(GAMERUN);
  currentEnv = gameRun;
}

static void gameRunAction() {
  setDrawnMap(getMap(currentEnv), getControlledNpc(currentEnv)->pixelPos);
  pickDestLoop(currentEnv->npcSet);
  moveDestLoop(currentEnv->npcSet);
  SDL_RenderCopy(getRenderer(), getMapBG(currentEnv), getDrawMap(), getDrawScreen());
  drawAllNpcs(currentEnv->npcSet);
  transferIfNeeded();
}

void transitionToPauseMenu() {
  transferEnv = pauseMenu;
  
}

void transferToPauseMenu() {
  //don't do anything?
  //might set returnState of menu to what it was before
  setGameState(MENU);
  pauseMenu->map = currentEnv->map;
  pauseMenu->menu = currentEnv->menu;
  currentEnv = pauseMenu;
}

static void basicMenuAction() {
  SDL_RenderCopy(getRenderer(), getMapBG(currentEnv), getDrawMap(), getDrawScreen());
  //drawAllNpcs(currentEnv->npcSet);
  if (getGameState() == MENU) {
    drawCurrentMenu();
  }
  //fun thing here, trying to figure out how/when to transition to map edit
  //got rid of actions for each menu, so I think I'd have to check either here
  //or menuInput, if newMenu text is the thing for mapEditing.
  else if (getGameState() == MENUTEXTENTRY) {
    
    drawTextEntry();
  }
  menuInput();
  transferIfNeeded();
}

void transitionToMapEdit() {
  transferEnv = mapEdit;
}


void transferToMapEdit() {
  //basically, just have mapEdit point to gameRun map
  //maybe also have map edit where controlled character is?
  setGameState(GAMEMAPEDIT);
  mapEdit->map = currentEnv->map;
  npc* mc = getControlledNpc(currentEnv);
  npcNode* debug = getControlledNpcNode(mapEdit);

  setTilePos(debug->dest, debug->storedNpc->tilePos);
  currentEnv = mapEdit;
}

static void mapEditAction() {
  //same as regular action?
  setDrawnMap(getMap(currentEnv), getControlledNpc(currentEnv)->pixelPos);
  pickDestLoop(currentEnv->npcSet);
  moveDestLoop(currentEnv->npcSet);
  SDL_RenderCopy(getRenderer(), getMapBG(currentEnv), getDrawMap(), getDrawScreen());
  drawAllNpcs(currentEnv->npcSet);
  transferIfNeeded();
}

environ* createEnviron() {
  environ* new = malloc(sizeof(environ));
  new->npcSet = NULL;
  new->map = NULL;
  new->menu = NULL;
  return new;
}

environ* currentEnv;


void setNpcSet(environ* env, npcSet* new) {
  env->npcSet = new;
}

npcSet* getNpcSet(environ* env) {
  return env->npcSet;
}

static void freeEnviron(environ* env) {
  //ideally just call specific free functions on fields
  //bit tricker when things are shared between environs. 
  freeNpcSet(env->npcSet);
  freeMap(env->map);
  freeMenu(env->menu);
}

void setMap(environ* env, map* newMap) {
    env->map = newMap;
}

map* getMap(environ* env) {
  return env->map;
}

tileMap* getTileMap(environ* env) {
  return env->map->tileMap;
}

SDL_Texture* getMapBG(environ* env) {
  return env->map->mapBG;
}

void setMapBG(environ* env, SDL_Texture* newBG) {
  env->map->mapBG = newBG;
}

menu* getMenu(environ* env ) {
  return env->menu;
}

void setMenu(environ* env, menu* newMenu) {
  if (newMenu == NULL) {
    env->menu->action();
  }
  else {
    env->menu = newMenu;
    if (newMenu->action != NULL) {
      newMenu->action();
    }

  }
}

void runAction(environ* env) {
  env->action();
}

npc* getControlledNpc(environ * env) {
  npcNode* ret = getControlledNpcNode(env);
  if (ret != NULL) {
    return ret->storedNpc;
  }
  else {
    return NULL;;
  }
}

npcNode* getControlledNpcNode(environ* env) {
  npcSet *set = getNpcSet(env);
  if (set != NULL) {
    for (npcNode* search = set->idleList->start; search; search = search->next) {
      if (isControlled(search->storedNpc)) {
	return search;
      }
    }
    for (npcNode* search = set->moveList->start; search; search = search->next) {
      if (isControlled(search->storedNpc)) {
	return search;
      }
    }
  }
  return NULL;
}



void updateMapEditMap() {
  updateMap(getMap(mapEdit));
}


//because this accesses stinrg/int temp/field, should go in my input
//that or have set/access functions. also needs access to string consts for the menu text. 


//also, need to have access to mapEdit env. to get controlled npc and also map
void setTileStructField() {
  environ* mapEditEnv = mapEdit;
  char* text = getMenu(currentEnv)->text;
  char* fieldVal;
  //could either string compare or compare addresses if using const strings
  if (text == STRING_TILE_BG_PATH) {
    tilePos* position = getControlledNpc(mapEditEnv)->tilePos;
    fieldVal  = getTileFromMapPos(getMap(currentEnv), position)->tilePath;
    fieldType = STRING_CHANGE;
    setStringField(fieldVal);
    setStringTemp(fieldVal);
  }
  else if (text == INT_TILE_WALL) {
    tilePos* position = getControlledNpc(mapEditEnv)->tilePos;
    int* field = &(getTileFromMapPos(getMap(currentEnv), position)->isWall);
    setIntField(field);
    setIntTemp(*field);
    fieldType = INT_CHANGE;
  }

  setGameState(MENUTEXTENTRY);
}

void setMapStructField() {
  environ* mapEditEnv = mapEdit;
  char* text = getMenu(currentEnv)->text;
  if (text == INT_MAP_ROWS) {
    map* theMap = mapEditEnv->map;
    int* field = &(theMap->tileMap->rows);
    setIntField(field);
    setIntTemp(*field);
    fieldType = INT_CHANGE;
  }
  else if (text == INT_MAP_COLS) {
    map* theMap = mapEditEnv->map;
    int* field = &(theMap->tileMap->cols);
    setIntField(field);
    setIntTemp(*field);
    fieldType = INT_CHANGE;
  }
  setGameState(MENUTEXTENTRY);
}


void selectMenu(char* entry) {
  int found = 0;
  int index = 0;
  menu* activeMenu = pauseMenu->menu;
  while(!found) {
    if (index == activeMenu->arrayBound) {
      fprintf(stderr, "Unable to find %s in menu %s\n", entry, activeMenu->text);
      found = -1;
	}
    //reason why hardcoding the puaseMenu environ doesn't work
    //the transferToPauseMenu setis it back to the main menu
    //so probably scrap shortcuts or think of a better way to do things
    else if (!strcmp(activeMenu->menuEntries[index].text, entry)) {
      setMenu(pauseMenu, &(activeMenu->menuEntries[index]));
      found = 1;
    }
    else {
      index++;
    }
  }
}
