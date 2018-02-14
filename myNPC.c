#include <stdlib.h>
#include "myNPC.h"
#include <assert.h>

//static prototype
static void appendToNPC_list(NPC_list_t* list, NPC_node_t* new);

static void prependToNPC_list(NPC_list_t* list, NPC_node_t* new);

static void removeFromNPC_list(NPC_list_t* list, NPC_t* ID);


extern SDL_Renderer* gRan;
extern tile_map_t* activeMap;

NPC_t* createNPC() {
  NPC_t* newNPC = malloc(sizeof(NPC_t));
  newNPC->position = malloc(sizeof(tile_pos_t));
  newNPC->pixelPos = malloc(sizeof(npc_pos_t));
  newNPC->animState = malloc(sizeof(anim_state));
  newNPC->animState->holder = malloc(sizeof(sprite_holder_t));
  return newNPC;
}

void makeMC(NPC_t* slate) {
  slate->npcID = 0;
  slate->speed = TILED * 1;
  slate->animState->spriteRow = 0;
  slate->animState->spriteCol = 0;
  slate->flags = 1;
  loadSpriteMC(slate->animState->holder);
}
static int NPC_COUNT = 1;
void makeNPC(NPC_t* slate) {
  slate->npcID = NPC_COUNT++;
  slate->speed = TILED * 1;
  slate->animState->spriteRow = 0;
  slate->animState->spriteCol = 0;
  slate->flags = 0;
  slate->animState->holder->sprite_sheet = NULL;
}

void freeNPC(NPC_t* npc) {
  //bit of a pickle here, since the npc_sprite_holder is shared among npcs
  //might just need to maintain a list of unique spritesheets, then free at end
  free(npc);
}

void setNPCPosition(NPC_t* npc, tile_pos_t* pos) {
  setNPCPositionByCord(npc, pos->posX, pos->posY);
}
void setNPCPositionByCord(NPC_t* npc, int x, int y) {
  setTilePositionByCord(npc->position, x,y);
  npc->pixelPos->pixPosX = x * TILED;
  npc->pixelPos->pixPosY = y * TILED;
}
 
void setTilePosition(tile_pos_t* npcPos, tile_pos_t* pos) {
  setTilePositionByCord(npcPos, pos->posX, pos->posY);
}

void setTilePositionByCord(tile_pos_t* npcPos, int x, int y) {
  npcPos->posX = x;
  npcPos->posY = y;
}

void loadSpriteMC(sprite_holder_t* holder) {
  holder->sprite_width = 32;
  holder->sprite_height = 32;
  holder->rows = 1;
  holder->cols = 1;
  holder->sprite_sheet = loadTexture("/home/nudon/prg/gam/media/sp/def.png", gRan);
}

NPC_node_t* createNPC_node(NPC_t* npc) {
  NPC_node_t* newNode = malloc(sizeof(NPC_node_t));
  newNode->dest = malloc(sizeof(tile_pos_t));
  newNode->storedNPC = npc;
  setTilePosition(newNode->dest, npc->position);
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;       
}

void freeNPC_node(NPC_node_t* npcNode) {
  free(npcNode->dest);
  free(npcNode);
}

NPC_move_list* createNPC_move_list() {
  NPC_move_list* newMoveList = malloc(sizeof(NPC_move_list));
  newMoveList->idleList = createNPC_list();
  newMoveList->moveList = createNPC_list();
  return newMoveList;
}

NPC_list_t* createNPC_list() {
  NPC_list_t* newList = malloc(sizeof(NPC_list_t));
  newList->start = NULL;
  newList->end = NULL;
  return newList;
}

void freeNPC_list(NPC_list_t* npcList) {
  NPC_node_t* current = npcList->start;
  while(current != NULL) {
    current = current->next;
    freeNPC_node(current->prev);
  }
  free(npcList);
}

void addNPC(NPC_t* npc) {
  prependToNPC_list(activeMap->allNPCS->idleList, createNPC_node(npc));
}
void changeToMoveList(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  //  printf("moving to moveList: %d\n", npcNode->storedNPC->npcID);
  removeFromNPC_list(totNPC->idleList, npcNode->storedNPC);
  prependToNPC_list(totNPC->moveList, npcNode);
}
void changeToIdleList(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  //  printf("moving to idleList: %d\n", npcNode->storedNPC->npcID);
  removeFromNPC_list(totNPC->moveList, npcNode->storedNPC);
  prependToNPC_list(totNPC->idleList, npcNode);
}


void printNPCList(NPC_list_t* list) {
  NPC_node_t* node = list->start; 
  while(node != NULL) {
    printf("NPC: %d , ", node->storedNPC->npcID);
    node = node->next;
  }
   printf("end \n\n");
}

static void appendToNPC_list(NPC_list_t* list, NPC_node_t* new) {
  //  printf("adding node %d\n", new->storedNPC->npcID);
  //  printf("list at start: ");
  //  printNPCList(list);
  if (list->start == NULL) {
    list->start = new;
  }
  if (list->end != NULL) {
    list->end->next = new;
    new->prev = list->end;
  }
  else {
    new->prev = NULL;
    list->start = new;
  }
  list->end = new;
  new->next = NULL;
  //  printf("list at end: ");
  //  printNPCList(list);
}

static void prependToNPC_list(NPC_list_t* list, NPC_node_t* new) {
  //  printf("adding node %d\n", new->storedNPC->npcID);
  //  printf("list at start: ");
  //  printNPCList(list);
  if (list->end == NULL) {
    list->end = new;
  }
  if (list->start != NULL) {
    list->start->prev = new;
    new->next = list->start;
  }
  else {
    new->next = NULL;
    list->end = new;
  }
  list->start = new;
  new->prev = NULL;
  //  printf("list at end: ");
  //  printNPCList(list);

}

static void removeFromNPC_list(NPC_list_t* list, NPC_t* ID) {
  //   printf("removing node %d\n", ID->npcID);
  //printf("list at start: ");
  //printNPCList(list);
  //from begining search
  NPC_node_t* node = list->start; 
  while(node != NULL && node->storedNPC != ID) {
    node = node->next;
  }
  if (node != NULL) {
    if (node->prev == NULL && node->next == NULL) {
      list->start = NULL;
      list->end = NULL;
    }
    else {
      if (node->prev == NULL) {
	list->start = node->next;
	node->next->prev = NULL;
      }
      else if (node->next == NULL ){
	list->end = node->prev;
	node->prev->next = NULL;
      }
      else {
	node->prev->next = node->next;
	node->next->prev = node->prev;
      }
    }
    node->next = NULL;
    node->prev = NULL;
  }
  else {
    //fprintf(stderr, "Error on attempting to remove NPC from list, npc not in list\n");
  }
  //    printf("list at end: ");
  //    printNPCList(list);
}


//utilities
int equalTilePos(tile_pos_t* t1, tile_pos_t* t2) {
  if (t1->posX == t2->posX && t1->posY == t2->posY) {
    return 1;
  }
  else {
    return 0;
  }
}

void pickDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  uint8_t lFlags = npcNode->storedNPC->flags;
  if (lFlags == 1) {
    singleInput(npcNode);
    if (!equalTilePos(npcNode->dest, npcNode->storedNPC->position)) {
      changeToMoveList(totNPC, npcNode);
    }
  }
  else {
    int random = rand();
    if (random % 5 == 0) {
    }
    else {
      if (random % 4 == 0) {
	npcNode->dest->posX = npcNode->storedNPC->position->posX - 1;
      } else if (random % 3 == 0) {
	npcNode->dest->posY = npcNode->storedNPC->position->posY - 1;
      } else if (random % 2 == 0) {
	npcNode->dest->posX = npcNode->storedNPC->position->posX + 1;
      } else {
	npcNode->dest->posY = npcNode->storedNPC->position->posY + 1;
      }
      changeToMoveList(totNPC, npcNode);
    }
  }
}

int validPos(tile_pos_t* tile) {
  int val = 1;
  if(tile->posX < 0 || tile->posX >= activeMap->cols)
    val = 0;
  else if(tile->posY < 0 || tile->posY >= activeMap->rows)
    val = 0;
  else if(isAWall(getTileFromMapPos(activeMap,tile)))
    val = 0;
  return val;
}

void positionShift(NPC_t* npc, int* shiftPos, int shiftAmount) {
  *shiftPos = *shiftPos +  shiftAmount;
  updateNPCPos(npc);
  if (!validPos(npc->position)) {
    //slight issue here, would just move thing 1 step before breaking
    //would want to additionally clear/update npcDest
    //and also move npc back entirely to a valid tile
    *shiftPos = *shiftPos - shiftAmount;
    updateNPCPos(npc);
  }
}

void updateNPCPos(NPC_t* npc) {
  int tileOffset = npc->pixelPos->pixPosX - npc->position->posX * TILED;  
  if ( tileOffset > TILED / 2 || tileOffset < -TILED / 2) {
    npc->position->posX += round((double)tileOffset / TILED);
  }
  tileOffset = npc->pixelPos->pixPosY - npc->position->posY * TILED;
  if ( tileOffset > TILED / 2 || tileOffset < -TILED / 2) {
    npc->position->posY += round((double)tileOffset / TILED);
  }
}

void moveToDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  //what am I doing here
  //have dx and dy, deltas of current pos to dest pos
  //have sx and sy, which indicate sign of dx or dy
  int lSpeed = npcNode->storedNPC->speed;
  lSpeed = lSpeed / TILED ;  
  int dx, dy, sx, sy;
  dx = npcNode->dest->posX * TILED - npcNode->storedNPC->pixelPos->pixPosX;
  dy = npcNode->dest->posY * TILED - npcNode->storedNPC->pixelPos->pixPosY;
  sx = 1;
  sy = 1;
  if (dx < 0) {
    sx = -1;
  }
  if (dy < 0) {
    sy = -1;
  }
  if (dy == 0) {
    positionShift(npcNode->storedNPC, &(npcNode->storedNPC->pixelPos->pixPosX), lSpeed * sx);
  }
  else if (dx == 0) {
    positionShift(npcNode->storedNPC, &(npcNode->storedNPC->pixelPos->pixPosY), lSpeed * sy);
  }
  else {
    if (rand() % 2 == 0) {
      positionShift(npcNode->storedNPC, &(npcNode->storedNPC->pixelPos->pixPosX), lSpeed * sx);
    }
    else {
      positionShift(npcNode->storedNPC, &(npcNode->storedNPC->pixelPos->pixPosY), lSpeed * sy);
    }
  }
  //checking this if I messed up speed and overshot on last adjustment
  //just recalculating dx and dy, and making sure signs didn't switch
  dx = npcNode->dest->posX * TILED - npcNode->storedNPC->pixelPos->pixPosX;
  dy = npcNode->dest->posY * TILED - npcNode->storedNPC->pixelPos->pixPosY;
  //fun thing here, with a npc walking around, somehow mc ends up in moveToDest
  //trying to figure out how
  //this was a fun bug
  //printed out movelist each time, things weren't in there, somehow mc was getting in
  //figured out, not wiping prev/next things when removing
  //at least I think, that would explain how things are still getting linked outside of add/remove from lits funcs
  //also, at some point the 2 form a cycle, and printnpcList just loops forever.
  //also, somehow I'm still getting the cycle, but this time with just 1 element
  //found a related issue, not sure exactly how it worked, but in the destloops, needed a temp var
  //think it's related to me messing around with the list structure whilst attempting to itterate over loop
  assert((dx <= 0 && sx <= 0) || (dx >= 0 && sx >= 0));
  assert((dy <= 0 && sy <= 0) || (dy >= 0 && sy >= 0));
  if (dx == 0 && dy == 0) {
    changeToIdleList(totNPC, npcNode);
  }
}

void pickDestLoop(NPC_move_list* npcList) {
  //printf("here is idle list:");
  //printNPCList(npcList->idleList);
  NPC_node_t* current = npcList->idleList->start;
  NPC_node_t* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    pickDest(npcList, temp);
  }
}


void moveDestLoop(NPC_move_list* npcList) {
  //printf("here is move list:");
  //printNPCList(npcList->moveList);
  NPC_node_t* current = npcList->moveList->start;
  NPC_node_t* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    moveToDest(npcList, temp);

  }
}


enum KeyPress {
  KEY_PRESS_DEFAULT,
  KEY_PRESS_UP,
  KEY_PRESS_DOWN,
  KEY_PRESS_LEFT,
  KEY_PRESS_RIGHT,
  KEY_PRESS_TOTAL
};

extern int quit;
void singleInput(NPC_node_t* npcNode) {
  //think the bug about being able to move diagonally is here
  //since handling poll events in while loop
  //heard that not doing things in loop is bad, essentially inputs just buffer up
  //kind of hard to pull off right now, don't think I want to wait(to make it easier)
  //if I did that, whole game would essentially just sit and wait for userinput
  //easy to get rid of, just set some int, and essentially deplete pollevent
  //might also be some flush thing I could use
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      quit = 1;
    }
    else if (e.type == SDL_KEYDOWN) {
      handleSingleInput(npcNode, e);
    }
  }
}

void handleSingleInput(NPC_node_t* npcNode, SDL_Event e) {
  switch (e.key.keysym.sym){
  case SDLK_UP:
    npcNode->dest->posY = npcNode->storedNPC->position->posY - 1;
    break;		
  case SDLK_DOWN:
    npcNode->dest->posY = npcNode->storedNPC->position->posY + 1;
    break;
  case SDLK_LEFT:
    npcNode->dest->posX = npcNode->storedNPC->position->posX - 1;
    break;
  case SDLK_RIGHT:
    npcNode->dest->posX = npcNode->storedNPC->position->posX + 1;
    break;
  default:
    break;      
  }
  return;
}
