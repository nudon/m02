#include "myInput.h"
#include "myMap.h"
#include "gameState.h"
#include "myMenu.h"
#include "systemLimits.h"
#include "myEnv.h"

//array over enumerations of keys found in header
//stores whether the last associated with the key a keydown or keyup
static uint32_t keyStates[KEY_LAST];

//used for field input
int fieldType;
const int STRING_CHANGE = 1;
const int INT_CHANGE = 2;
static char* stringField;
static char stringTemp[ MAXPATHLEN ];
static int* intField;
static int* intTemp;


static void handleMoveMent(npcNode* npcNode, SDL_Event* e);
static void commitChanges();
static void commitStringChanges();
static void commitIntChanges();

char* getTempString() {
  return stringTemp;
}

void setTempString(char* new) {
  memcpy(stringTemp, new, strlen(new));
}

static void commitStringChanges();

void characterInput(npcNode* node) {
  SDL_Event e;
  gameState curr;
  int val = -1;
  while(SDL_PollEvent(&e) != 0 ) {
    curr = getGameState();
    if (e.type == SDL_QUIT) {
      setGameState(GAMEQUIT);
    }
    else if (curr == GAMERUN) {
      if (e.type == SDL_KEYDOWN) {
	val = handleSingleInput(node, &e);
      }
    }
    else if (curr == GAMEMAPEDIT) {
      if (e.type == SDL_KEYDOWN) {
	val = handleMapEditInput(node, &e);
      }
    }
    if (val == -1) {
      //call some generic function which sees if key is being kept track of
      //then check/update it. 
      if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
	updateKeys(&e);
      }
    }
  }
}


void menuInput() {
 SDL_Event e;
 gameState curr = getGameState();
 int val = -1;
  while(SDL_PollEvent(&e) != 0 ) {
    if (e.type == SDL_QUIT) {
      setGameState(GAMEQUIT);
    }
    else if (curr == MENU) {
      if (e.type == SDL_KEYDOWN) {
	val = basicMenuInputHandler(&e);
      }
    }
    else if (curr == MENUTEXTENTRY) {
      if (e.type == SDL_TEXTINPUT || e.type == SDL_KEYDOWN) {
	val = handleTextEntry(&e);
      }
    }
    if (val == -1) {
      //call some generic function which sees if key is being kept track of
      //then check/update it. 
      if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
	updateKeys(&e);
      }
    }
  }
}

int handleSingleInput(npcNode* npcNode, SDL_Event* e) {
  //specifics for this, returns a positive number if recieved input that means something
  //negative number if key pressed meant nothing
  int val = 1;
  keyArg activeKey;
  switch (e->key.keysym.sym){
  case SDLK_UP:
    handleMoveMent(npcNode, e);
    break;		
  case SDLK_DOWN:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_LEFT:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_RIGHT:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      //setActiveMenu(getMainMenu());
      //no, instead call some transition function
      transitionToPauseMenu();
    }
    break;
  default:
    val = -1;
    break;      
  }
  return val;
}

void handleMoveMent(npcNode* npcNode, SDL_Event* e) {
  tilePos* pos =  npcNode->storedNpc->tilePos;
  switch(e->key.keysym.sym) {
  case SDLK_UP:
    setTilePosByCord(npcNode->dest, pos->x, pos->y - 1);
    break;		
  case SDLK_DOWN:
    setTilePosByCord(npcNode->dest, pos->x, pos->y + 1);
    break;
  case SDLK_LEFT:
    setTilePosByCord(npcNode->dest, pos->x - 1, pos->y);
    break;
  case SDLK_RIGHT:
    setTilePosByCord(npcNode->dest, pos->x + 1, pos->y);
    break;
  default:
    break;      
  }
}

//basic menu input handler
//basically, up/down changes activeItem
//esc goes to parent menu
//enter/return to call an associated function
int basicMenuInputHandler(SDL_Event* e) {
  int val = 1;
  menu* activeMenu = getMenu(currentEnv);
  keyArg key;
  switch(e->key.keysym.sym) {
  case SDLK_DOWN:
    key = KEY_DOWN;
    if (checkAndUpdateKey(key, e)) {
      activeMenu->activeIndex += 1;
      if (activeMenu->activeIndex == activeMenu->arrayBound) {
	activeMenu->activeIndex = 0;
      }
    }
    break;
  case SDLK_UP:
    key = KEY_UP;
    if (checkAndUpdateKey(key, e)) {
      activeMenu->activeIndex -= 1;
      if (activeMenu->activeIndex == -1) {
	activeMenu->activeIndex = activeMenu->arrayBound - 1;
      }
    }
    break;
  case SDLK_RETURN:
    key = KEY_RETURN;
    if (checkAndUpdateKey(key, e)) {
      setMenu( currentEnv, &(activeMenu->menuEntries[activeMenu->activeIndex]));
    }
    break;
  case SDLK_ESCAPE:
    key = KEY_ESCAPE;
    if (checkAndUpdateKey(key, e)) {
      setMenu(currentEnv,activeMenu->parent);
    }
    break;
  default:
    val = -1;
    break;
  }
  return val;
}

int handleMapEditInput(npcNode* npcNode, SDL_Event* e) {
  //basically, just move around normally
  //pushing t brings up tile edit menu
  //issue of setting stringField
  //easier in case of pushing t. since I know which field to goto
  //just need to write some code for getting the current tile position
  //actually that's kind of hard
  //would need debugPosition
  //then lookup in the map for the actual tile
  //for storing debug position, could just keep some reference for it.
  //I'm a moron, I already have it in the node.
  
  //accessing things through the menu get awfully terrible though.
  //I could do some additional trickery to set up a mirror tile struct
  //just have each menuEntry point to some field in that
  //in general though that means a copy of dummy copy of every field I want to edit
  //wheras currently I just have 1 for each type of field I want to edit
  //pushing m brings up map edit menu, not implemented, but eventually changes map size
  keyArg activeKey;
  int val = 1;
  switch(e->key.keysym.sym) {
  case SDLK_UP:
    handleMoveMent(npcNode, e);
    break;		
  case SDLK_DOWN:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_LEFT:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_RIGHT:
    handleMoveMent(npcNode, e);
    break;
  case SDLK_t:
    activeKey = KEY_t;
    if (checkAndUpdateKey(activeKey, e)) {
            tilePos* position = npcNode->storedNpc->tilePos;
     stringField = getTileFromMapCord(getMap(currentEnv), position->x, position->y)->tilePath;
      memcpy(stringTemp, stringField, strlen(stringField));
      transitionToPauseMenu();
      //fuck, causing a menu reset because of how I'm changing arguements 
      selectMenu("Tile Menu");
    }
    break;
  case SDLK_m:
    break;
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      transitionToPauseMenu();
    }
  default:
    val = -1;
    break;
  }
  return val;
}

int handleTextEntry(SDL_Event* e) {
  //would be nice to make this also handle integer input
  //would need to store some internal state based on what type the active field is
  //or make the field/temp thing a discriminated union,

  //also, I'd like some functionality for remembering the most recently typed fields
  //so I don't need to type so much
  //that or implement some type of clone features
  //probably some key bindings for copy/paste
  //would probably just be hardcoded to save of fields of tiles
  //or just point to a tile, and copy values over
  int val = 1;
  keyArg key;
  if (e->type == SDL_KEYDOWN) {
    switch(e->key.keysym.sym) {
    case SDLK_ESCAPE:
      //abandon changes
      //don't change anything I guess
      //go back to previouse menu
      key = KEY_ESCAPE;
      if (checkAndUpdateKey(key, e)) {
	setMenu(currentEnv, getMenu(currentEnv)->parent);
	setGameState(MENU);
      }
      break;
    case SDLK_RETURN:
      key = KEY_RETURN;
      if (checkAndUpdateKey(key, e)) {
	commitChanges();
	setMenu(currentEnv, getMenu(currentEnv)->parent);
	setGameState(MENU);
      }
      break;
      //think I also need some handling for backspaces, copying, and pasting
    case SDLK_BACKSPACE:
      val = strlen(stringTemp);
      if (val != 0) {
	stringTemp[val - 1] = '\0';
      }
    default:
      val = -1;
      break;
    }
  }
  else if (e->type == SDL_TEXTINPUT) {
    memcpy(&(stringTemp[strlen(stringTemp)]), e->text.text, strlen(e->text.text));
  }
  return val;
}

void updateKeys(SDL_Event* e) {
  keyArg key;
  switch (e->key.keysym.sym) {
  case SDLK_t:
    key = KEY_t;
    break;
  case SDLK_m:
    key = KEY_m;
        break;
  case SDLK_UP:
    key = KEY_UP;
        break;
  case SDLK_DOWN:
    key = KEY_DOWN;
        break;
  case SDLK_LEFT:
    key = KEY_LEFT;
    break;
  case SDLK_RIGHT:
    key = KEY_RIGHT;
        break;
  case SDLK_ESCAPE:
    key = KEY_ESCAPE;
        break;
  case SDLK_RETURN:
    key = KEY_RETURN;
        break;
  default:
    key = KEY_LAST;
    break;
  }
  if (key != KEY_LAST) {
    checkAndUpdateKey(key, e);
  }
}
void commitChanges() {
  if (fieldType == STRING_CHANGE) {
    commitStringChanges();
  }
  else if (fieldType == INT_CHANGE) {
    commitIntChanges();
  }
}
void commitStringChanges() {
  //for now, just going to overwrites stringField with stringTemp;

  writeFilePath(stringField, stringTemp);
  //were does thigs go?
  //one way, could have a massive if statement at end of this
  //check which menu string I changed, and redo stuff bassed on that
  //well, have a thing where I specifically call tileStruct
  //could just mirror however I do that, for
  fprintf(stderr, "re cintering tiles\n");
  SDL_DestroyTexture(getMapBG(currentEnv));
  setMapBG(currentEnv, cinterTiles(getMap(currentEnv)));

}

void commitIntChanges() {
  *intField = *intTemp;
  
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

//so, having a hard time thinking about field selection.
//right now, just hardcoded the only string I'm processing, the tile path. Want to moddify each individual field.
//have an idea for an annoying solution. basically copy menuInput function, but include code particular to tile_struct
//particularly, on pushing return, look at what the active index is, look at the menu entries text, based on that correctly set the stringfield/stringTemp to the appropriate tile field.

//only issue is the whole copying menu input. gets annoying if I'm adding features to that and I have to copy it to all the different variations of the menu.

//might specify soom other variable/state, which would indicate a specific struct entry. in handling return, check if state is special, if it is, call some generic function to select a specific function for a struct that gets things setup.


//would also be nice to have string constants for path
//would be in some header shared with this and menus
//would also mean I could just compare addresses
void tileFieldEdit(tile* tile, char* menuText) {
  if (!strcmp(menuText, "Path to BG")) {
    stringField = tile->tilePath;
    memcpy(stringTemp, stringField, strlen(stringField));
  }
  else if (!strcmp(menuText, "tile contents")) {
    //probably go to some other menu
  }
  else if (!strcmp(menuText, "wall status")) {
    intField = &(tile->isWall);
    intTemp = intField;
  }
}

void mapFieldEdit(map* map, char* menuText) {
  //field of map
  //rows, cols,
  //tile[][]
  //npc move list
  //mabBG
  if (!strcmp(menuText, "rows")) {
    intField = &(map->rows);
    intTemp = intField;
  }
  else if (!strcmp(menuText, "cols")) {
    intField = &(map->cols);
    intTemp = intField;
  }
  else if (!strcmp(menuText, "tiles")) {
    //?
  }
  else if (!strcmp(menuText, "npcs")) {
    //?
  }
  else if (!strcmp(menuText, "background")) {
    //? probably not going to have this
    //no, i'm not
  }
}


void setStringField(char* fieldArg) {
  stringField = fieldArg;
}
void setStringTemp(char* tempArg) {
  memcpy(stringTemp, tempArg, strlen(tempArg));
}
//might actually have intTemp be a string, and just do string<>int conversions
void setIntField(int* intArg) {
  intField = intArg;
}
void setIntTemp(int tempArg){
  *intTemp = tempArg;
}
