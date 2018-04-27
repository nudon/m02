#include "myEnv.h"
#include "myImage.h"
#include "myInput.h"
#include "mySave.h"
#include "systemLimits.h"

//transitions, are done at end of action, because otherwise changing environ mid-action gets odd
//also, outside of menus, expecting some keypres to bring up menu
//while inside menus, expecting menu-actions to handle transitions, among other things

static void gameRunAction();

static void basicMenuAction();

static void mapEditAction();

static void freeEnviron(environ* env);

static void envInternalClone(int mode);

void transferToGameRun();
void transferToPauseMenu();
void transferToMapEdit();

struct environ_ {
  npcSet* npcSet;
  map** map;
  void (*action)();
  menu* menu;
};

environ  *gameRun, *pauseMenu, *mapEdit, *gameQuit, *transferEnv;

int*** walls = NULL;

void setWalls(tileMap* map) {
  int rows = map->rows;
  int cols = map->cols;
  walls = (int***) malloc(sizeof(int*) * rows * cols);
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    walls[colIndex] = (int**) malloc(sizeof(int*) * cols);
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      walls[colIndex][rowIndex] = &(getTileFromTileMapCord(getTileMap(mapEdit), colIndex, rowIndex))->isWall;
    }
  }
}

void wallShade() {
  tileMap* tiles = getTileMap(mapEdit);
  SDL_Rect dest;
  if (tiles->rows * tiles->cols >= 0) {
    for (int i = 0; i < tiles->rows; i++) {
      for (int j = 0; j < tiles->cols; j++) {
	//instead of loading texture, load some custom graphic or draw one
	tile* tile = getTileFromTileMapCord(tiles, j, i);
	if (isAWall(tile)) {
	  if (setDestrectForDrawingSomethingFromTilePos(&dest, tile->tilePosition) != NULL) {
	    drawWallIndication(&dest);
	  }
	}
      }
    }  
  }
  else {
    fprintf(stderr, "Tile had one or more demension equal to zero\n");
  }
}

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
  //testSave();
  //loadMap("allBill");
  //exit(EXIT_SUCCESS);
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
  //thinking about having a way to visually indicate which things are walls
  //idea was I'd have some parallell map that would be transparent
  //and fill in some cross-stich for walls where appropriate
  //also had some other ideas that seemed to use a similar idea of parallell array
  //also wanted to find an efficient way of composing and drawing all of these thing
  //like, should I clober things together into a megatexture/surface first, then render? and on which level is the megatexture, 1 per parrallell map, or 1 for every parallell map flattened
  //probably just going to have to run some tests on which is faster
  setGameState(GAMEMAPEDIT);
  mapEdit->map = currentEnv->map;
  //  npc* mc = getControlledNpc(currentEnv);
  npcNode* debug = getControlledNpcNode(mapEdit);
  allignPixPosToTilePos(debug->storedNpc->pixelPos, debug->storedNpc->tilePos);
  setTilePos(debug->dest, debug->storedNpc->tilePos);
  currentEnv = mapEdit;
}

static void mapEditAction() {
  //same as regular action?
  setDrawnMap(getMap(currentEnv), getControlledNpc(currentEnv)->pixelPos);
  pickDestLoop(currentEnv->npcSet);
  moveDestLoop(currentEnv->npcSet);
  SDL_RenderCopy(getRenderer(), getMapBG(currentEnv), getDrawMap(), getDrawScreen());
  wallShade();
  drawAllNpcs(currentEnv->npcSet);
  transferIfNeeded();
}

environ* createEnviron() {
  environ* new = malloc(sizeof(environ));
  new->npcSet = NULL;
  new->map = malloc(sizeof(map*));;
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
  freeMap(getMap(env));
  freeMenu(env->menu);
}

void setMap(environ* env, map* newMap) {
  *(env->map) = newMap;
}

map* getMap(environ* env) {
  return *(env->map);
}

tileMap* getTileMap(environ* env) {
  return getMap(env)->tileMap;
}

SDL_Texture* getMapBG(environ* env) {
  return getMap(env)->mapBG;
}

void setMapBG(environ* env, SDL_Texture* newBG) {
  getMap(env)->mapBG = newBG;
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
  map* theMap = getMap(mapEditEnv);
  if (text == INT_MAP_ROWS) {
    int* field = &(theMap->tileMap->rows);
    setIntField(field);
    setIntTemp(*field);
    fieldType = INT_CHANGE;
  }
  else if (text == INT_MAP_COLS) {
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

void setupMapLoad() {
  setupMapSave();
}

char mapFile[MAXPATHLEN];
void setupMapSave() {
  //some odd things here 
  //mapFile[0] = '^';
  setGameState(MENUTEXTENTRY);
  fieldType = STRING_CHANGE;
  setStringField(mapFile);
  setStringTemp("enter text");
  
}



void saveMap(char* path) {
  //do some read write stuff
  char* savedMap = tileMapToString(getMap(mapEdit)->tileMap);

  char* preamble = generatePreamble(savedMap);
  //....
  char actualPath[MAXPATHLEN];
  copyWithMediaPrefix(actualPath, path);
  SDL_RWops *file = SDL_RWFromFile(actualPath, "w");
  if (file != NULL) {
    SDL_RWwrite(file, preamble, sizeof(char), strlen(preamble));
    SDL_RWwrite(file, savedMap, sizeof(char), strlen(savedMap));
    SDL_RWclose(file);
  }
  else {
    fprintf(stderr, "Unable to save File \'%s\' at location \'%s\'\n",
	    path, actualPath);
  }
}


void loadMap(char* path) {
  //want a better way of doing this.
  //thinking of storing some int at begining of file and reading that first, 

  //so, things are working?
  //able to load all bill, odd thing is that character movement is halved. somehow. 
  //also crashes when actually loading from menu
  //what did I change? before, just had 1 map, which I just changed the tileMap && bg pointers
  //lol, issue is I put the loadMap as a thing in the gameRunAction, so every frame it's reading the file, creating things, creating buffers of length 5000, and all that. 
  
  char actualPath[MAXPATHLEN];
  copyWithMediaPrefix(actualPath, path);
  SDL_RWops *file = SDL_RWFromFile(actualPath, "r");
  if (file != NULL) {
    char preamble[SIZE_OF_PREAMBLE];
    SDL_RWread(file, preamble, sizeof(char), SIZE_OF_PREAMBLE);
    int versionNumber = getVersionNumber(preamble);
    int sizeofFile = getTotalLength(preamble) + SIZE_OF_PREAMBLE;
    char loadedSaveFile[sizeofFile];
    SDL_RWread(file, loadedSaveFile, sizeof(char), sizeofFile);
    SDL_RWclose(file);
    //....
    map* activeMap = getMap(mapEdit);
    if (activeMap != NULL) {
      freeTileMap(activeMap->tileMap);
      SDL_DestroyTexture(getMapBG(mapEdit));
      free(activeMap);
    }
    //maybe have a load map which takes a string and int version number 
    char* mapSegment = getMapSegFromSaveFile(loadedSaveFile);
    activeMap = stringToMap(mapSegment);
    setMap(currentEnv, activeMap);
    free(mapSegment);
  }
  else {
    fprintf(stderr, "File \'%s\' was not found at location \'%s\'\n",
	    path, actualPath);
  }
}

void updateMapBG() {
  SDL_DestroyTexture(getMapBG(currentEnv));
  setMapBG(currentEnv, cinterTiles(getTileMap(currentEnv)));
}

static int cloneTileModeCopy = 1;
static int cloneTileModeSet = 2;
void setCloneTile() {
  envInternalClone(cloneTileModeSet);
}
void carryOutTileClone() {
  envInternalClone(cloneTileModeCopy);
}

static void envInternalClone(int mode) {
  //mode == 1, clone tile
  //mode == 2, set clone tile
  static tile* clone = NULL;
  tilePos* position = getControlledNpc(currentEnv)->tilePos;
  tile* currTile = getTileFromMapPos(getMap(currentEnv), position);
  if (mode == cloneTileModeCopy) {
    if (clone != NULL) {
      //funny thing here
      //was breaking wallShade
      //because I'm also cloning tilePos
      cloneTile(currTile, clone);
      updateMapBG();
      *(currTile->tilePosition) = *position;
    }
  } else if (mode == cloneTileModeSet) {
    if (clone == NULL) {
      tile* temp = createTile(0,0);
      clone = temp;
    }
    cloneTile(clone, currTile);
  }
  else {
    fprintf(stderr, "envInternalClone invoked with invalid mode %d\n", mode);
  }
}
