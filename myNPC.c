#include <stdlib.h>
#include <stdint.h>
//#include <SDL2/SDL.h>
#include "myMap.h"
#include "myNPC.h"
#include "myImage.h"
#include "gameState.h"
#include "myInput.h"
#include "systemLimits.h"
#include <assert.h>

//extern int SCREEN_WIDTH, SCREEN_HEIGHT;

//in terms of memory management for sprite holders
//bit of a pickle here, since the npc_sprite_holder is shared among npcs
//so on calling free npc, freeing the sprite holder would break things if holder is being shared
//might just need to maintain a list of unique spritesheets, then free at end
//actually like that idea, have an array of those spritesheets. the * kind, not the **
//then I just need one free

//static prototype
static void appendToNPC_list(NPC_list_t* list, NPC_node_t* new);

static void prependToNPC_list(NPC_list_t* list, NPC_node_t* new);

static void removeFromNPC_list(NPC_list_t* list, NPC_node_t* node);

static NPC_node_t* controlledCharacter;

NPC_node_t* getControlledCharacter() {
  return controlledCharacter;
}

void setControlledCharacter(NPC_node_t* node) {
  controlledCharacter = node;
}

void pickDestLoop(NPC_move_list* npcList) {
  NPC_node_t* current = npcList->idleList->start;
  NPC_node_t* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    pickDest(npcList, temp);
  }
}

void pickDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  uint8_t lFlags = npcNode->storedNPC->flags;
  tile_pos_t* pos = npcNode->storedNPC->position;
  if (isControlled(npcNode->storedNPC)) {
    characterInput(npcNode);
    if (!equalTilePos(npcNode->dest, pos)) {
      changeToMoveList(totNPC, npcNode);
    }
  }
  else {
    int random = rand();
    if (random % 5 == 0) {
    }
    else {
      if (random % 4 == 0) {
	setDestByCord(npcNode, pos->posX - 1, pos->posY);
      } else if (random % 3 == 0) {
	setDestByCord(npcNode, pos->posX, pos->posY - 1);
      } else if (random % 2 == 0) {
	setDestByCord(npcNode, pos->posX, pos->posY + 1);
      } else {
	setDestByCord(npcNode, pos->posX + 1, pos->posY);
      }
      changeToMoveList(totNPC, npcNode);
    }
  }
}
void setDestByTile(NPC_node_t* node, tile_pos_t* tile) {
  setDestByCord(node, tile->posX, tile->posY);
}

void setDestByCord(NPC_node_t* node, int x, int y) {
  node->dest->posX = x;
  node->dest->posY = y;
}
void moveDestLoop(NPC_move_list* npcList) {
  NPC_node_t* current = npcList->moveList->start;
  NPC_node_t* temp;
  while(current != NULL) {
    temp = current;
    current = current->next;
    moveToDest(npcList, temp);
  }
}

void moveToDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
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
    positionShift(npcNode, &(npcNode->storedNPC->pixelPos->pixPosX), lSpeed * sx);
  }
  else if (dx == 0) {
    positionShift(npcNode, &(npcNode->storedNPC->pixelPos->pixPosY), lSpeed * sy);
  }
  else {
    if (rand() % 2 == 0) {
      positionShift(npcNode, &(npcNode->storedNPC->pixelPos->pixPosX), lSpeed * sx);
    }
    else {
      positionShift(npcNode, &(npcNode->storedNPC->pixelPos->pixPosY), lSpeed * sy);
    }
  }
  //checking this if I messed up speed and overshot on last adjustment
  //just recalculating dx and dy, and making sure signs didn't switch
  dx = npcNode->dest->posX * TILED - npcNode->storedNPC->pixelPos->pixPosX;
  dy = npcNode->dest->posY * TILED - npcNode->storedNPC->pixelPos->pixPosY;
  assert((dx <= 0 && sx <= 0) || (dx >= 0 && sx >= 0));
  assert((dy <= 0 && sy <= 0) || (dy >= 0 && sy >= 0));
  if (dx == 0 && dy == 0) {
    changeToIdleList(totNPC, npcNode);
  }
}

void positionShift(NPC_node_t* npcNode, int* shiftPos, int shiftAmount) {
  *shiftPos = *shiftPos +  shiftAmount;
  updateNPCPos(npcNode->storedNPC);
  if (!validPos(npcNode->storedNPC->position) && (!isDebuging(npcNode->storedNPC))) {
    *shiftPos = *shiftPos - shiftAmount;
    updateNPCPos(npcNode->storedNPC);
    setNPCPosition(npcNode->storedNPC, npcNode->storedNPC->position);
    setDestByTile(npcNode, npcNode->storedNPC->position);
  }
}

int validPos(tile_pos_t* tile) {
  if(tile->posX < 0 || tile->posX >= getActiveMap()->cols) {
    return 0;
  }
  else if(tile->posY < 0 || tile->posY >= getActiveMap()->rows) {
    return 0;
  }
  else if(isAWall(getTileFromMapPos(getActiveMap(),tile))) {
    return 0;
  }
  return 1;
}

void updateNPCPos(NPC_t* npc) {
  int tileOffset = npc->pixelPos->pixPosX - npc->position->posX * TILED;  
  if ( tileOffset >= TILED / 2 || tileOffset <= -TILED / 2) {
    npc->position->posX += round((double)tileOffset / TILED);
  }
  tileOffset = npc->pixelPos->pixPosY - npc->position->posY * TILED;
  if ( tileOffset >= TILED / 2 || tileOffset <= -TILED / 2) {
    npc->position->posY += round((double)tileOffset / TILED);
  }
}
void drawAllNPCS(NPC_move_list* list) {
  drawNPCList(list->idleList);
  drawNPCList(list->moveList);
}

void drawNPCList(NPC_list_t* list) {
  NPC_node_t* current = list->start;
  while(current != NULL) {
    drawNPC(current->storedNPC);
    current = current->next;
  }
}

void drawNPC(NPC_t* npc) {
  SDL_Rect srcRect, destRect;
  SDL_Renderer* rend = getRenderer();
  npc_pos_t* cameraPos = getCameraPos();
  destRect.x = npc->pixelPos->pixPosX + getDrawScreen()->x;
  destRect.y = npc->pixelPos->pixPosY + getDrawScreen()->y;
  destRect.w = TILED;
  destRect.h = TILED;
  if (destRect.x + TILED < cameraPos->pixPosX - (SCREEN_WIDTH / 2) ||
      destRect.x - TILED > cameraPos->pixPosX + (SCREEN_WIDTH / 2)) {
  }
  else if (destRect.y + TILED < cameraPos->pixPosY - (SCREEN_HEIGHT / 2) ||
	   destRect.y - TILED > cameraPos->pixPosY + (SCREEN_HEIGHT / 2)) {
  }
  else {
    if (npc->animState->holder != NULL && npc->animState->holder->sprite_sheet != NULL ) {
      
      srcRect.x = npc->animState->spriteCol * npc->animState->holder->sprite_width;
      srcRect.y = npc->animState->spriteRow * npc->animState->holder->sprite_height;
      srcRect.w = npc->animState->holder->sprite_width;
      srcRect.h = npc->animState->holder->sprite_height;
      
      SDL_RenderCopy(rend,
		     npc->animState->holder->sprite_sheet,
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
}


//goint to divide up npc flags now
//last bit is going to indicate special debug purposes.
//first bit is going to indicate character is controlled by user
//other bit ideas, one for being able to fly
//one for being a predator
//one for being prey
static uint8_t controlledMask = 1;
static uint8_t debugingMask = 1 << 7;

int isControlled(NPC_t* npc) {
  return (npc->flags & controlledMask);
}

int isDebuging(NPC_t* npc) {
  return (npc->flags & debugingMask);
}

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
  slate->flags = controlledMask;
  loadSpriteMC(slate->animState->holder);
}

void loadSpriteMC(sprite_holder_t* holder) {
  holder->sprite_width = 32;
  holder->sprite_height = 32;
  holder->rows = 1;
  holder->cols = 1;
  holder->sprite_sheet = loadTexture("/home/nudon/prg/gam/media/sp/def.png");
}

void makeDebug(NPC_t* slate) {
  slate->npcID = -1;
  slate->speed = TILED * TILED;
  slate->animState->spriteRow = 0;
  slate->animState->spriteCol = 0;
  slate->animState->holder = NULL;
  slate->flags = controlledMask | debugingMask;
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
  free(npc->position);
  free(npc->pixelPos);
  free(npc->animState);
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
  freeNPC(npcNode->storedNPC);
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
  NPC_node_t* temp  = current;
  while(current != NULL) {
    temp = current;
    current = temp->next;
    freeNPC_node(temp);
  }
  free(npcList);
}

void addControlledCharacter(NPC_t* cont) {
  NPC_node_t* new = createNPC_node(cont);
  if (getControlledCharacter() != NULL) {
    fprintf(stderr, "replacing a controlled Character!\n");
  }
  setControlledCharacter(new);
  prependToNPC_list(getActiveMap()->allNPCS->idleList, new);
}

void addNPC(NPC_t* npc) {
  prependToNPC_list(getActiveMap()->allNPCS->idleList, createNPC_node(npc));
}

void addNPCToList(NPC_t* npc, NPC_list_t* list) {
  prependToNPC_list(list, createNPC_node(npc));
}

void changeToMoveList(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  removeFromNPC_list(totNPC->idleList, npcNode);
  prependToNPC_list(totNPC->moveList, npcNode);
}
void changeToIdleList(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  removeFromNPC_list(totNPC->moveList, npcNode);
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

static void prependToNPC_list(NPC_list_t* list, NPC_node_t* new) {
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

static void removeFromNPC_list(NPC_list_t* list, NPC_node_t* node) {
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

int equalTilePos(tile_pos_t* t1, tile_pos_t* t2) {
  if (t1->posX == t2->posX && t1->posY == t2->posY) {
    return 1;
  }
  else {
    return 0;
  }
}


