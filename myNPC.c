#include <stdlib.h>
#include <stdint.h>
//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myNPC.h"
#include "myImage.h"
#include "gameState.h"
#include "myInput.h"
#include "systemLimits.h"
#include "myEnv.h"
#include <assert.h>

//extern int SCREEN_WIDTH, SCREEN_HEIGHT;

//in terms of memory management for sprite holders
//bit of a pickle here, since the npc_sprite_holder is shared among npcs
//so on calling free npc, freeing the sprite holder would break things if holder is being shared
//might just need to maintain a list of unique spritesheets, then free at end
//actually like that idea, have an array of those spritesheets. the * kind, not the **
//then I just need one free

//static prototype
static void appendToNpcList(npcList* list, npcNode* new);

static void prependToNpcList(npcList* list, npcNode* new);

static void removeFromNpcList(npcList* list, npcNode* node);

void pickDestLoop(npcSet* npcList) {
  npcNode* current = npcList->idleList->start;
  npcNode* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    pickDest(npcList, temp);
  }
}

void pickDest(npcSet* totNpc, npcNode* npcNode) {
  uint8_t lFlags = npcNode->storedNpc->flags;
  tilePos* pos = npcNode->storedNpc->tilePos;
  if (isControlled(npcNode->storedNpc)) {
    characterInput(npcNode);
    if (!equalTilePos(npcNode->dest, pos)) {
      changeToMoveList(totNpc, npcNode);
    }
  }
  else {
    int random = rand();
    if (random % 5 == 0) {
    }
    else {
      if (random % 4 == 0) {
	setTilePosByCord(npcNode->dest, pos->x - 1, pos->y);
      } else if (random % 3 == 0) {
	setTilePosByCord(npcNode->dest, pos->x, pos->y - 1);
      } else if (random % 2 == 0) {
	setTilePosByCord(npcNode->dest, pos->x, pos->y + 1);
      } else {
	setTilePosByCord(npcNode->dest, pos->x + 1, pos->y);
      }
      changeToMoveList(totNpc, npcNode);
    }
  }
}

void moveDestLoop(npcSet* npcList) {
  npcNode* current = npcList->moveList->start;
  npcNode* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    moveToDest(npcList, temp);
  }
}

void moveToDest(npcSet* totNpc, npcNode* npcNode) {
  //have dx and dy, deltas of current pos to dest pos
  //have sx and sy, which indicate sign of dx or dy
  int lSpeed = npcNode->storedNpc->speed;
  lSpeed = lSpeed / TILED ;  
  int dx, dy, sx, sy;
  dx = npcNode->dest->x * TILED - npcNode->storedNpc->pixelPos->x;
  dy = npcNode->dest->y * TILED - npcNode->storedNpc->pixelPos->y;
  sx = 1;
  sy = 1;
  if (dx < 0) {
    sx = -1;
  }
  if (dy < 0) {
    sy = -1;
  }
  if (dy == 0) {
    positionShift(npcNode, &(npcNode->storedNpc->pixelPos->x), lSpeed * sx);
  }
  else if (dx == 0) {
    positionShift(npcNode, &(npcNode->storedNpc->pixelPos->y), lSpeed * sy);
  }
  else {
    if (rand() % 2 == 0) {
      positionShift(npcNode, &(npcNode->storedNpc->pixelPos->x), lSpeed * sx);
    }
    else {
      positionShift(npcNode, &(npcNode->storedNpc->pixelPos->y), lSpeed * sy);
    }
  }
  //checking this if I messed up speed and overshot on last adjustment
  //just recalculating dx and dy, and making sure signs didn't switch
  dx = npcNode->dest->x * TILED - npcNode->storedNpc->pixelPos->x;
  dy = npcNode->dest->y * TILED - npcNode->storedNpc->pixelPos->y;
  assert((dx <= 0 && sx <= 0) || (dx >= 0 && sx >= 0));
  assert((dy <= 0 && sy <= 0) || (dy >= 0 && sy >= 0));
  if (dx == 0 && dy == 0) {
    changeToIdleList(totNpc, npcNode);
  }
}

void positionShift(npcNode* npcNode, int* shiftPos, int shiftAmount) {
  *shiftPos = *shiftPos +  shiftAmount;
  updateNpcPos(npcNode->storedNpc);
  if (!validPos(npcNode->storedNpc->tilePos) && (!isDebuging(npcNode->storedNpc))) {
    *shiftPos = *shiftPos - shiftAmount;
    updateNpcPos(npcNode->storedNpc);
    allignPixPosToTilePos(npcNode->storedNpc->pixelPos, npcNode->storedNpc->tilePos);
    setTilePos(npcNode->dest, npcNode->storedNpc->tilePos);
    //setDestByTile(npcNode, npcNode->storedNpc->position);
  }
}

int validPos(tilePos* tile) {
  if(tile->x < 0 || tile->x >= getMap(currentEnv)->tileMap->cols) {
    return 0;
  }
  else if(tile->y < 0 || tile->y >= getMap(currentEnv)->tileMap->rows) {
    return 0;
  }
  else if(isAWall(getTileFromMapPos(getMap(currentEnv),tile))) {
    return 0;
  }
  return 1;
}

void updateNpcPos(npc* npc) {
  int tileOffset = npc->pixelPos->x - npc->tilePos->x * TILED;  
  if ( tileOffset >= TILED / 2 || tileOffset <= -TILED / 2) {
    npc->tilePos->x += round((double)tileOffset / TILED);
  }
  tileOffset = npc->pixelPos->y - npc->tilePos->y * TILED;
  if ( tileOffset >= TILED / 2 || tileOffset <= -TILED / 2) {
    npc->tilePos->y += round((double)tileOffset / TILED);
  }
}
void drawAllNpcs(npcSet* set) {
  drawNpcList(set->idleList);
  drawNpcList(set->moveList);
}

void drawNpcList(npcList* list) {
  npcNode* current = list->start;
  while(current != NULL) {
    drawNpc(current->storedNpc);
    current = current->next;
  }
}
SDL_Rect* setDestrectForDrawingSomethingFromTilePos(SDL_Rect* dest, tilePos* tPos) {
  pixPos new;
  new.x = tPos->x * TILED;
  new.y = tPos->y * TILED;
  return setDestrectForDrawingSomethingFromPixelPos(dest, &new);
}

  SDL_Rect* setDestrectForDrawingSomethingFromPixelPos(SDL_Rect* dest, pixPos* pPos) {
  pixPos* cameraPos = getCameraPos();
  dest->x = pPos->x + getDrawScreen()->x - getDrawMap()->x;
  dest->y = pPos->y + getDrawScreen()->y - getDrawMap()->y;
  dest->w = TILED;
  dest->h = TILED;
  if (dest->x + TILED < cameraPos->x - (SCREEN_WIDTH / 2) ||
      dest->x - TILED > cameraPos->x + (SCREEN_WIDTH / 2)) {
    dest = NULL;
  }
  else if (dest->y + TILED < cameraPos->y - (SCREEN_HEIGHT / 2) ||
	   dest->y - TILED > cameraPos->y + (SCREEN_HEIGHT / 2)) {
    dest = NULL;
  }
  return dest;
}

void drawNpc(npc* npc) {
  SDL_Rect srcRect, destRect;
  SDL_Renderer* rend = getRenderer();
  pixPos* cameraPos = getCameraPos();
  if (setDestrectForDrawingSomethingFromPixelPos(&destRect, npc->pixelPos) != NULL){
    if (npc->animateState->holder != NULL && npc->animateState->holder->sprite_sheet != NULL ) {
      srcRect.x = npc->animateState->spriteCol * npc->animateState->holder->sprite_width;
      srcRect.y = npc->animateState->spriteRow * npc->animateState->holder->sprite_height;
      srcRect.w = npc->animateState->holder->sprite_width;
      srcRect.h = npc->animateState->holder->sprite_height;
      SDL_RenderCopy(rend,
		     npc->animateState->holder->sprite_sheet,
		     &srcRect,
		     &destRect);
    }
    else {
      //use drawing primitives
      if (isDebuging(npc)) {
	SDL_SetRenderDrawColor(getRenderer(), 255, 10, 10, 56);
	myDrawRect(destRect.x, destRect.y, destRect.x + destRect.w, destRect.y + destRect.h);
      }
      else if (isControlled(npc)== 1) {
	myDrawFunRect(destRect.x, destRect.y, destRect.x + destRect.w, destRect.y + destRect.h, 5);
      }
      else {
	SDL_SetRenderDrawColor( rend, 0x00, 0x00, 0x00, 0xFF );
	myDrawCirc(destRect.x + (TILED / 2), destRect.y + (TILED / 2), TILED / 2);
	SDL_SetRenderDrawColor( rend, 0xff, 0xff, 0xff, 0xFF );
	myDrawCirc(destRect.x + (TILED / 2), destRect.y + (TILED / 2), TILED / 3);
      }
    }
  }
  //else the pixpos was offscreen, no need to draw
}


//goint to divide up npc flags now
//last bit is going to indicate special debug purposes.
//first bit is going to indicate character is controlled by user
//other bit ideas, one for being able to fly
//one for being a predator
//one for being prey
static uint8_t controlledMask = 1;
static uint8_t debugingMask = 1 << 7;

int isControlled(npc* npc) {
  return (npc->flags & controlledMask);
}

int isDebuging(npc* npc) {
  return (npc->flags & debugingMask);
}

npc* createNpc() {
  npc* newNpc = malloc(sizeof(npc));
  newNpc->tilePos = malloc(sizeof(tilePos));
  newNpc->pixelPos = malloc(sizeof(tilePos));
  newNpc->animateState = malloc(sizeof(animateState));
  newNpc->animateState->holder = malloc(sizeof(spriteHolder));
  return newNpc;
}


void makeMC(npc* slate) {
  slate->npcID = 0;
  slate->speed = TILED * 1;
  slate->animateState->spriteRow = 0;
  slate->animateState->spriteCol = 0;
  slate->flags = controlledMask;
  loadSpriteMC(slate->animateState->holder);
}

void loadSpriteMC(spriteHolder* holder) {
  holder->sprite_width = 32;
  holder->sprite_height = 32;
  holder->rows = 1;
  holder->cols = 1;
  holder->sprite_sheet = loadTexture("/sp/def.png");
}

void makeDebug(npc* slate) {
  slate->npcID = -1;
  slate->speed = TILED * TILED;
  slate->animateState->spriteRow = 0;
  slate->animateState->spriteCol = 0;
  slate->animateState->holder = NULL;
  slate->flags = controlledMask | debugingMask;
}


static int NPC_COUNT = 1;
void makeNpc(npc* slate) {
  slate->npcID = NPC_COUNT++;
  slate->speed = TILED * 1;
  slate->animateState->spriteRow = 0;
  slate->animateState->spriteCol = 0;
  slate->flags = 0;
  slate->animateState->holder->sprite_sheet = NULL;
}

void freeNpc(npc* npc) {
  free(npc->tilePos);
  free(npc->pixelPos);
  free(npc->animateState);
  free(npc);
}

void allignPixPosToTilePos(pixPos* pixPos, tilePos* tilePos) {
  allignPixPosToTilePosByCord(pixPos, tilePos->x, tilePos->y);
}

void allignPixPosToTilePosByCord(pixPos* pixPos, int x, int y) {
  pixPos->x = x * TILED;
  pixPos->y = y * TILED;
}

void setPixPos(pixPos* pixPosDest, pixPos* pixPosSrc) {
  setPixPosByCord(pixPosDest, pixPosSrc->x, pixPosSrc->y);
}
void setPixPosByCord(pixPos* pixPos, int x, int y) {
  pixPos->x = x;
  pixPos->y = y;
}
 
void setTilePos(tilePos* tilePosDest, tilePos* tilePosSrc) {
  setTilePosByCord(tilePosDest, tilePosSrc->x, tilePosSrc->y);
}

void setTilePosByCord(tilePos* tilePos, int x, int y) {
  tilePos->x = x;
  tilePos->y = y;
}


npcNode* createNpcNode(npc* npc) {
  npcNode* newNode = malloc(sizeof(npcNode));
  newNode->dest = malloc(sizeof(tilePos));
  newNode->storedNpc = npc;
  setTilePos(newNode->dest, npc->tilePos);
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;       
}

void freeNpcNode(npcNode* npcNode) {
  freeNpc(npcNode->storedNpc);
  free(npcNode->dest);
  free(npcNode);
}

npcSet* createNpcSet() {
  npcSet* newMoveList = malloc(sizeof(npcSet));
  newMoveList->idleList = createNpcList();
  newMoveList->moveList = createNpcList();
  return newMoveList;
}

npcList* createNpcList() {
  npcList* newList = malloc(sizeof(npcList));
  newList->start = NULL;
  newList->end = NULL;
  return newList;
}

void freeNpcSet(npcSet* set) {
  freeNpcList(set->idleList);
  freeNpcList(set->moveList);
}

void freeNpcList(npcList* npcList) {
  npcNode* current = npcList->start;
  npcNode* temp  = current;
  while(current != NULL) {
    temp = current;
    current = temp->next;
    freeNpcNode(temp);
  }
  free(npcList);
}

void addNpc(npc* npc) {
  prependToNpcList(getNpcSet(currentEnv)->idleList, createNpcNode(npc));
}

void addNpcToList(npc* npc, npcList* list) {
  prependToNpcList(list, createNpcNode(npc));
}

void changeToMoveList(npcSet* totNpc, npcNode* npcNode) {
  removeFromNpcList(totNpc->idleList, npcNode);
  prependToNpcList(totNpc->moveList, npcNode);
}
void changeToIdleList(npcSet* totNpc, npcNode* npcNode) {
  removeFromNpcList(totNpc->moveList, npcNode);
  prependToNpcList(totNpc->idleList, npcNode);
}


void printNpcList(npcList* list) {
  npcNode* node = list->start; 
  while(node != NULL) {
    printf("Npc: %d , ", node->storedNpc->npcID);
    node = node->next;
  }
   printf("end \n\n");
}

static void appendToNpcList(npcList* list, npcNode* new) {
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
}

static void prependToNpcList(npcList* list, npcNode* new) {
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

}

static void removeFromNpcList(npcList* list, npcNode* node) {
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
}

int equalTilePos(tilePos* t1, tilePos* t2) {
  if (t1->x == t2->x && t1->y == t2->y) {
    return 1;
  }
  else {
    return 0;
  }
}


