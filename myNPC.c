#include "myNPC.h"
/*
  So for implementing npc's
in each map struct, have a list of npc's
npc struct itself, store position, x/y for now, some fields/flags for npc behavior
also speed, an animation struct *, and an anim index, keeps track of how far in cycle npc is

might want a similar approach to objects like boxes and crates, maybe doors. 
some struct, with some sprite sheet, posiiton, tracked of by map

for sprite struct, was thinking of having on filesystem, 1 sprite sheet per npc
haven't thought much beyond organizing sprite beyond seperate anim cycles get seperate rows/columns
then, in program, have a struct, containing the sprite sheet, and x/y dimensions of single sprite


now, issue of simultaneous character movement.
approach to this is have 2 functions, each having their own list or circular buffer
 1 that, for each npc in buffer, takes info like flags and pos, and spits out a direction to move
then, if a dir was chosen, removes from buffer, add's to 2's
2 that, for each npc in buffer, moves the npc in an appropriate vector. 
then, if npc has reached destination, remove from 2's buffer, add to 1
want destination to be a tile, to allow things to slide on ice or charge in a direction
not really sure how to store that. easiest would be store as a pair in cust struct. 
also, would want to set an npc's anim index from both functions
//might be too advanced, but idea of certain interactions interupting movement would be nice
//like getting knocked around, have dest and speed changed, 

then, for actually drawing everything, npcs stationary, moving, boxes, 
call some specialized functions that goes through relevant lists on map
//also, had an interesting idea. If I'm parsing npc list for things in a subset of positions
//might be worthwhile to have list be parrrallel to map, or in some way store in 2d array npcs
//So I can just search through subset of 2d array, and not whole list
//somewhat applies to non-tile based placement as well, just keep int array, and round npc pos
 */

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

void removeFromNPC_list(NPC_list_t* list, int ID) {
  //from begining search
  NPC_node_t* node = list->start;
  while(node != NULL && node->storedNPC->npcID != ID) {
    node = node->next;
  }
  if (node != NULL) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }
  else {
    fprintf(stderr, "Error on attempting to remove NPC%d from list, npc not in list\n", ID);
  }
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

//for function that decides on path dest for npc
//decide on direction to go
//modify/remove incase there are walls
//then add to another list
//right now just thinking on how to represent this
//1 approach, set a "hopeful" destination, pair dest with npc
//put in travel list, have travle loop through npcs in list and move them appropriately
//can have variable fidelity with how npc's respond to change in environment
//can set dest to farther away, will sort of be stuck and commited to movement
//can set closer, to reevaluate after each step.

//also thinking of just shoving dest in NPC_node_t, can use if want, instead of making new structs for a a single new field, and rewriting mostly good functions


void pickDest(NPC_node_t* npcNode) {
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
  //lDest->poxX = npc->posX + dx;
  //lDest->posY = npc->posY + dy;
}

void moveToDest(NPC_node_t* npcNode) {
  int lSpeed = npcNode->storedNPC->speed;
  //first off, have to do some routing, to not get stuck
  //approach I had in notes, involved dividing areas
  //and having paths between them.
  //various ways to handle this, can either have pick dest pick path
  //or pick actual dest, see if dest is in different sect then npc
  //if it is, find some appropriate path to take, move towards that
  //idea would be to take lSpeed, divide by a c
  //move them that much
}

void pickDestLoop(NPC_list_t* pickList, NPC_list_t* moveList  ) {
  NPC_node_t* current = pickList->start;
  while(current != NULL) {
    //migth want to through some random in here
    pickDest(current);
    removeFromNPC_list(pickList, current->storedNPC->npcID);
    //not sure which one is better
    appendToNPC_list(moveList, current);
    //prependToNPC_list(moveList, current);
    current = current->next;
  }
}


void moveDestLoop(NPC_list_t* pickList, NPC_list_t* moveList  ) {
  NPC_node_t* current = moveList->start;
  tile_pos_t* lDest, * lPos;
  while(current != NULL) {
    //migth want to through some random in here, so NPC's might pause
    moveToDest(current);
    lDest = current->dest;
    lPos = current->storedNPC->position;
    if (lPos->posX == lDest->posX && lPos->posY == lDest->posY) {
      removeFromNPC_list(pickList, current->storedNPC->npcID);
      //not sure which one is better
      appendToNPC_list(moveList, current);
      //prependToNPC_list(moveList, current);
    }
    current = current->next;
  }
}
