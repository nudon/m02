#include "myInput.h"
#include "gameState.h"
#include "myMenu.h"
#include "systemLimits.h"
#include "myEnv.h"
#include "myImage.h"

//array over enumerations of keys found in header
//stores whether the last associated with the key a keydown or keyup
static uint32_t keyStates[KEY_LAST];

//used for field input
int changeType;
const int MAP_DIM_CHANGE;
const int MAP_TILE_CHANGE;
int fieldType;
const int STRING_CHANGE = 1;
const int INT_CHANGE = 2;
static char* stringField;
static char stringTemp[ MAXPATHLEN ];
static int* intField;
static int intTemp;


static void handleMoveMent(npcNode* npcNode, SDL_Event* e);
static void commitChanges();
static void commitStringChanges();
static void commitIntChanges();

static int intLen(int num);



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
  //want t and m to be shortcuts for editing tile and map
  //though I broke starting at a non-main-menu
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
      //would be nice to handle copying, and pasting
    case SDLK_BACKSPACE:;
      int len;
      if (fieldType == STRING_CHANGE) {
	len = strlen(stringTemp);
	if (len != 0) {
	  stringTemp[len - 1] = '\0';
	}
      }
      else if (fieldType == INT_CHANGE) {
	len = intLen(intTemp);
	if (len == 1) {
	  intTemp = 0;
	}
	else {
	  intTemp = intTemp / 10;
	}
      }


    default:
      val = -1;
      break;
    }
  }
  //not actually working currenctly
  else if (SDL_GetModState() & KMOD_CTRL) {
    val = 1;
    if (e->key.keysym.sym == SDLK_v) {
      SDL_SetClipboardText(stringTemp);
    }
    else if (e->key.keysym.sym == SDLK_c) {
      memcpy(stringTemp, SDL_GetClipboardText(), MAXPATHLEN);
    }
  }
  else if (e->type == SDL_TEXTINPUT) {
    //will want to descriminate here based on type of entry,
    if (fieldType == STRING_CHANGE) {
      val = 1;
      memcpy(&(stringTemp[strlen(stringTemp)]), e->text.text, strlen(e->text.text));
    }
    else if (fieldType == INT_CHANGE) {
      val = 1;
      //surely the user would never give a bad number...
      int new = atoi(e->text.text);
      int len = intLen(new);
      for (int i = 0; i < len; i++) {
	intTemp *= 10;
      }
      intTemp += new;
    }
  }
  return val;
}

void drawTextEntry() {
  //basically, just render some text to center of screen
  //backgroun would also be nice
  

  SDL_Rect dstRect;
  menu* currentMenu = getMenu(currentEnv);
  int len;
  dstRect.h = currentMenu->entryHeight;
  dstRect.y = (SCREEN_HEIGHT - dstRect.h) / 2;

  if (fieldType == STRING_CHANGE) {
    char* text = getTempString();
    len = strlen(text);
    dstRect.w = len * ENTRY_WIDTH;
    dstRect.x = (SCREEN_WIDTH - dstRect.w) / 2;
    SDL_RenderCopy(getRenderer(), currentMenu->menuImage, NULL, &dstRect);
    drawText(currentMenu->font, text, currentMenu->textColor, &dstRect);    
  }
  else if (fieldType == INT_CHANGE) {
    int number = intTemp;;
    len = intLen(number);
    dstRect.w = len * ENTRY_WIDTH;
    dstRect.x = (SCREEN_WIDTH - dstRect.w) / 2;
    SDL_RenderCopy(getRenderer(), currentMenu->menuImage, NULL, &dstRect);
    drawNumber(currentMenu->font, number, currentMenu->textColor, &dstRect);    
  }
  else {
    fprintf(stderr, "attempting to drawTextEntry with wonky fieldType\n");
  }


 
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
  menu* theMenu = getMenu(currentEnv);
  if (theMenu->parent->text == MAP_EDIT_MENU) {
    fprintf(stderr, "updating map\n");
    updateMapEditMap();
  }
  else if (theMenu->parent->text == TILE_EDIT_MENU) {
    //think I'll also have to branch off of current menu things
    if (theMenu->text == STRING_TILE_BG_PATH) {
      fprintf(stderr, "re cintering tiles\n");
      commitStringChanges();
      SDL_DestroyTexture(getMapBG(currentEnv));
      setMapBG(currentEnv, cinterTiles(getTileMap(currentEnv)));
    }
    else if (theMenu->text == INT_TILE_WALL) {
      commitIntChanges();
    }
  }
  else if (fieldType == STRING_CHANGE) {
    commitStringChanges();
  }
  else if (fieldType == INT_CHANGE) {
    commitIntChanges();
  }

}



    
  //probably want to do something else, like check which menu/struct i was changing
  //so if menu was the map or tiles, recinter the map or whatnot
  //don't want to always reload everything. ideall commitStringChange just updates the string value, doesn't call anyything else
void commitStringChanges() {
  //for now, just going to overwrites stringField with stringTemp;

  writeFilePath(stringField, stringTemp);
  //were does thigs go?
  //one way, could have a massive if statement at end of this
  //check which menu string I changed, and redo stuff bassed on that
  //well, have a thing where I specifically call tileStruct
  //could just mirror however I do that, for
  


}

void commitIntChanges() {
  *intField = intTemp;
 }

void updateMap(map* theMap) {
  //have to do some trickery for old map to get coppied to new map
  //essentially need the old dimensions to know bounds of old tiles*, and the new dim to resize to
  //commiting string whipes out 1 old dim, which is a problem
  int oldRows, newRows, oldCols, newCols;
  oldCols = getMapDimX(theMap);
  oldRows = getMapDimY(theMap);
  commitIntChanges();
  if (getMapDimX(theMap) != oldRows || getMapDimY(theMap) != oldCols) {
    newCols = getMapDimX(theMap);
    newRows = getMapDimY(theMap);
    setMapDimX(theMap, oldCols);
    setMapDimY(theMap, oldRows);    
    changeMapDim(theMap, newRows, newCols);
    setMapBG(currentEnv, cinterTiles(getTileMap(currentEnv)));
  }
  else {
    fprintf(stderr, "Didn't actually change the dim of map, odd things might happen\n");
  } 
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
    intTemp = *intField;
  }
}

void setStringField(char* fieldArg) {
  stringField = fieldArg;
}
void setStringTemp(char* tempArg) {
  memcpy(stringTemp, tempArg, strlen(tempArg));
}

void setIntField(int* intArg) {
  intField = intArg;
}
void setIntTemp(int tempArg){
  intTemp = tempArg;
}


char* getTempString() {
  return stringTemp;
}

void setTempString(char* new) {
  memcpy(stringTemp, new, strlen(new));
}

static void commitStringChanges();


int intLen(int num) {
  //assuming I'm printing in base 10
  int len = 1;
  int div = 10;
  while (num / div != 0) {
    len++;
    div *= 10;
  }
  return len;
}
