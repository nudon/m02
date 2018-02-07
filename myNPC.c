#include <stdlib.h>
#include "myNPC.h"
#include <assert.h>

extern SDL_Renderer* gRan;

void appendToNPC_list(NPC_list_t* list, NPC_node_t* new) {
  if (list->end != NULL) {
    list->end->next = new;
  }
  else if (list->start == NULL) {
    list->start = new;
    new->prev = NULL;
  }
  list->end = new;
  new->next = NULL;
}

void prependToNPC_list(NPC_list_t* list, NPC_node_t* new) {
  if (list->start != NULL) {
    list->start->prev = new;
  }
  else if (list->end == NULL) {
    list->end = new;
    new->next = NULL;
  }
  list->start = new;
  new->prev = NULL;
}

void removeFromNPC_list(NPC_list_t* list, NPC_t* ID) {
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
      }
      else if (node->next == NULL ){
	list->end = node->prev;
      }
      else {
	node->prev->next = node->next;
	node->next->prev = node->prev;
      }
    }
  }
  else {
    fprintf(stderr, "Error on attempting to remove NPC from list, npc not in list\n");
  }
}

NPC_move_list* createNPC_move_list() {
  NPC_move_list* newMoveList = malloc(sizeof(NPC_move_list));
  newMoveList->idleNPC = createNPC_list();
  newMoveList->moveNPC = createNPC_list();
  return newMoveList;
}

NPC_list_t* createNPC_list() {
  NPC_list_t* newList = malloc(sizeof(NPC_list_t));
  newList->start = NULL;
  newList->end = NULL;
  return newList;
}

NPC_node_t* createNPC_node(NPC_t* npc) {
  NPC_node_t* newNode = malloc(sizeof(NPC_node_t));
  newNode->dest = malloc(sizeof(tile_pos_t));
  newNode->storedNPC = npc;
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;       
}

void freeNPC_node(NPC_node_t* npcNode) {
  free(npcNode->dest);
  free(npcNode);
}

void freeNPC_list(NPC_list_t* npcList) {
  NPC_node_t* current = npcList->start;
  while(current != NULL) {
    current = current->next;
    freeNPC_node(current->prev);
  }
  free(npcList);
}


NPC_t* createNPC() {
  NPC_t* newNPC = malloc(sizeof(NPC_t));
  newNPC->position = malloc(sizeof(tile_pos_t));
  newNPC->pixelPos = malloc(sizeof(npc_pos_t));
  newNPC->animState = malloc(sizeof(anim_state));
  return newNPC;
}

void makeMC(NPC_t* slate) {
  slate->npcID = 0;
  slate->speed = 8;
  slate->animState->spriteRow = 0;
  slate->animState->spriteCol = 0;
  slate->flags = 1;
  loadSpriteMC(slate->animState->holder);
}

void loadSpriteMC(sprite_holder_t* holder) {
  holder->sprite_width = 32;
  holder->sprite_height = 32;
  holder->rows = 1;
  holder->cols = 1;
  holder->sprite_sheet = loadTexture("/home/nudon/prg/gam/media/sp/def.png", gRan);
}

void freeNPC(NPC_t* npc) {
  //bit of a pickle here, since the npc_sprite_holder is shared among npcs
  free(npc);
}

int equalTilePos(tile_pos_t* t1, tile_pos_t* t2) {
  if (t1->posX == t2->posX && t1->posY == t2->posY) {
    return 1;
  }
  else {
    return 0;
  }
}

void pickDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  tile_pos_t* lDest = npcNode->dest;
  uint8_t lFlags = npcNode->storedNPC->flags;
  //presumably parse npc flags and determine some behavior
  //then set lDest to something/somewhere
  //possibly cull the distance
  //would be int dx, dy
  //dx = npc->position->posX - lDest->posX
  //dy = npc->posiiton->posY - lDest->posY
  //dx = dx / c; dy = dy / c;
  //could negate c to have something run from lDest
  //right now, just do random movement
  if (lFlags == 1) {
    //receive input
    //parse input
    //correctly set dest
    singleInput(npcNode);
    if (!equalTilePos(npcNode->dest, npcNode->storedNPC->position)) {
	removeFromNPC_list(totNPC->idleNPC, npcNode->storedNPC);
	prependToNPC_list(totNPC->moveNPC, npcNode);
      }
  }
  else {
    int random = rand();
    if (random % 5 == 0) {
      //do nothing
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
      removeFromNPC_list(totNPC->idleNPC, npcNode->storedNPC);
      prependToNPC_list(totNPC->moveNPC, npcNode);
    }
  }
}

void moveToDest(NPC_move_list* totNPC, NPC_node_t* npcNode) {
  //what am I doing here
  //have dx and dy, deltas of current pos to dest pos
  //have sx and sy, which indicate sign of dx or dy
  //also using it to indicate which direction npc was moved, after movement
  //standard is, if sx is zero, just moved in y dir, and  vice versa
  int lSpeed = npcNode->storedNPC->speed;
  lSpeed = lSpeed / TILED ;
  int dx, dy, sy, sx;
  dx = npcNode->dest->posX - npcNode->storedNPC->position->posX;
  dy = npcNode->dest->posY - npcNode->storedNPC->position->posY;
  sx = 1;
  sy = 1;
  if (dx < 0) {
    sx = -1;
  }
  if (dy < 0) {
    sy = -1;
  }
  if (dy == 0) {
    npcNode->storedNPC->pixelPos->pixPosX += lSpeed * sx;
    sy = 0;
  }
  else if (dx == 0) {
    npcNode->storedNPC->pixelPos->pixPosY += lSpeed * sy;
    sx = 0;
  }
  else {
    if (rand() % 2 == 0) {
      npcNode->storedNPC->pixelPos->pixPosX += lSpeed * sx;
      sy = 0;
    }
    else {
      npcNode->storedNPC->pixelPos->pixPosY += lSpeed * sy;
      sx = 0;
    }
  }
  if (sy == 0) {
    if ( npcNode->storedNPC->pixelPos->pixPosX % TILED > TILED / 2) {
      npcNode->storedNPC->position->posX = npcNode->storedNPC->pixelPos->pixPosX % TILED + 1;
    }
    else {
      npcNode->storedNPC->position->posX = npcNode->storedNPC->pixelPos->pixPosX % TILED ;
    }
  }
  else {
    if ( npcNode->storedNPC->pixelPos->pixPosY % TILED > TILED / 2) {
      npcNode->storedNPC->position->posY = npcNode->storedNPC->pixelPos->pixPosY % TILED + 1;
    }
    else {
      npcNode->storedNPC->position->posY = npcNode->storedNPC->pixelPos->pixPosY % TILED ;
    }
  }
  //checking this if I fucked up speed and overshot on last adjustment
  //just recalculating dx and dy, and making sure signs didn't switch
  dx = npcNode->dest->posX - npcNode->storedNPC->position->posX;
  dy = npcNode->dest->posY - npcNode->storedNPC->position->posY;
  assert((dx <= 0 && sx <= 0) || (dx >= 0 && sx >= 0));
  assert((dy <= 0 && sy <= 0) || (dy >= 0 && sy >= 0));
  if (dx == 0 && dy == 0) {
    //might also be a good idea to explicity set npc_pos to center of tile
    //though the current rounding I'm doing might approximately have that affect
    //if things look off though, do it. 
    removeFromNPC_list(totNPC->moveNPC, npcNode->storedNPC);
    prependToNPC_list(totNPC->idleNPC, npcNode);
  }
}

void pickDestLoop(NPC_move_list* npcList) {
  NPC_node_t* current = npcList->idleNPC->start;
  while(current != NULL) {
    //migth want to through some random in here
    pickDest(npcList, current);
    current = current->next;
  }
}


void moveDestLoop(NPC_move_list* npcList) {
  NPC_node_t* current = npcList->moveNPC->start;
  while(current != NULL) {
    moveToDest(npcList, current);
    current = current->next;
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


//just handling interthread communications through global variable globQuit
//since acessing the return of input loop causes drawLoop to block
//wasn't able to modify contentes of any pointer args I sent to input loop
//compiler didn't like me touching contents of void*'s.
//with way i handled generic NPC movement, kind of useless to have a seperate input loop
//unless I change my movement thing to also live in a seperate thread/loop/fork

extern int quit;

void breakpoint() {};

void singleInput(NPC_node_t* npcNode) {
  SDL_Event e;
  /* while(SDL_PollEvent(&e) != 0) { */
  /*   breakpoint(); */
  /*   if (e.type == SDL_QUIT) { */
  /*     quit = 1; */
  /*   } */
  /*   else if (e.type == SDL_KEYDOWN) { */
  /*     handleSingleInput(npcNode, e); */
  /*   } */
  /* } */
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
