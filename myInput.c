#include "myInput.h"
#include "myMap.h"
#include "gameState.h"
#include "myMenu.h"
#include "systemLimits.h"

//array over enumerations of keys found in header
//stores whether the last associated with the key a keydown or keyup
static uint32_t keyStates[KEY_LAST];
static char* stringField;
static char stringTemp[ MAXPATHLEN ];



char* getTempString() {
  return stringTemp;
}

void setTempString(char* new) {
  memcpy(stringTemp, new, strlen(new));
}

static void commitStringChanges();

void characterInput(NPC_node_t* node) {
  SDL_Event e;
  gameState curr = getGameState();
  int val = -1;
  while(SDL_PollEvent(&e) != 0 ) {
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
 gameSubState curr = getGameSubState();
 int val = -1;
  while(SDL_PollEvent(&e) != 0 ) {
    if (e.type == SDL_QUIT) {
      setGameState(GAMEQUIT);
    }
    else if (curr == MENUNORM) {
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

int handleSingleInput(NPC_node_t* npcNode, SDL_Event* e) {
  //specifics for this, returns a positive number if recieved input that means something
  //negative number if key pressed meant nothing
  int val = 1;
  tile_pos_t* pos =  npcNode->storedNPC->position;
  keyArg activeKey;
  switch (e->key.keysym.sym){
  case SDLK_UP:
    setDestByCord(npcNode, pos->posX, pos->posY - 1);
    break;		
  case SDLK_DOWN:
    setDestByCord(npcNode, pos->posX, pos->posY + 1);
    break;
  case SDLK_LEFT:
    setDestByCord(npcNode, pos->posX - 1, pos->posY);
    break;
  case SDLK_RIGHT:
    setDestByCord(npcNode, pos->posX + 1, pos->posY);
    break;
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
	setActiveMenu(getMainMenu());
    }
  default:
    val = -1;
    break;      
  }
  return val;
}

//basic menu input handler
//basically, up/down changes activeItem
//esc goes to parent menu
//enter/return to call an associated function
int basicMenuInputHandler(SDL_Event* e) {
  int val = 1;
  menu* activeMenu = getActiveMenu();
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
      setActiveMenu(&(activeMenu->menuEntries[activeMenu->activeIndex]));
    }
    break;
  case SDLK_ESCAPE:
    key = KEY_ESCAPE;
    if (checkAndUpdateKey(key, e)) {
      setActiveMenu(activeMenu->parent);
    }
    break;
  default:
    val = -1;
    break;
  }
  return val;
}

int handleMapEditInput(NPC_node_t* node, SDL_Event* e) {
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
  tile_pos_t* pos = node->dest;
  int val = 1;
  switch(e->key.keysym.sym) {
  case SDLK_UP:
    setDestByCord(node, pos->posX, pos->posY - 1);
    break;		
  case SDLK_DOWN:
    setDestByCord(node, pos->posX, pos->posY + 1);
    break;
  case SDLK_LEFT:
    setDestByCord(node, pos->posX - 1, pos->posY);
    break;
  case SDLK_RIGHT:
    setDestByCord(node, pos->posX + 1, pos->posY);
    break;
  case SDLK_t:
    activeKey = KEY_t;
    if (checkAndUpdateKey(activeKey, e)) {
      tile_pos_t* position = node->storedNPC->position;
      stringField = getTileFromMapCord(getActiveMap(), position->posX, position->posY)->tilePath;
      memcpy(stringTemp, stringField, strlen(stringField));
      setActiveMenu(&(getMapEditMenu()->menuEntries[1]));
    }
    break;
  case SDLK_m:
    break;
  case SDLK_ESCAPE:
    activeKey = KEY_ESCAPE;
    if (checkAndUpdateKey(activeKey, e)) {
      setActiveMenu(getMapEditMenu());
    }
  default:
    val = -1;
    break;
  }
  return val;
}

int handleTextEntry(SDL_Event* e) {
  //this is running too much. looks like mouse events are also getting read?
  //yeah, had an || SDL_TEXTENTRY in if before calling this. 
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
	setActiveMenu(getActiveMenu()->parent);
      }
      break;
    case SDLK_RETURN:
      key = KEY_RETURN;
      if (checkAndUpdateKey(key, e)) {
	//commit changes
	//ideally do this behind a function call. to also sanitize input
	//possibly have this return 0 if sanitizing was okay, 1 if not
	//actually, can't really sanitize the string, so
	//just having it behind a function call anyways
	commitStringChanges();
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
    fprintf(stderr, "doing the text entry!\n");
    fprintf(stderr, "found text (%s)!\n", e->text.text);
    memcpy(&(stringTemp[strlen(stringTemp)]), e->text.text, strlen(e->text.text));
    fprintf(stderr, "Stringtemp is  (%s)!\n", stringTemp);
    //see StartTextInput and textInput events for user input
    //modify temporary text
    //do something with  e->text.text
    //either overwrite temp, or append things to it. 
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

void commitStringChanges() {
  //for now, just going to overwrites stringField with stringTemp;
  fprintf(stderr, "re cintering tiles\n");
  writeFilePath(stringField, stringTemp);
  SDL_DestroyTexture(getMapBG());
  setMapBG(cinterTiles(getActiveMap()));

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

