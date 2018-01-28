#ifndef FILE_MYNPC_SEEN
#define FILE_MYNPC_SEEN
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "myMap.h"

typedef
struct {
  int sprite_width;
  int sprite_height;
  int rows;
  int cols;
  SDL_Texture* sprite_sheet;
  
} sprite_holder_t;



typedef
struct {
  int npcID;
  tile_pos_t* position;
  int speed;
  sprite_holder_t* holder;
  int spriteRow;
  int spriteCcol;
  uint8_t flags;
} NPC_t;

struct NPC_node_struct{
  NPC_t* storedNPC;
  tile_pos_t* dest;
  struct NPC_node_struct * next;
  struct NPC_node_struct * prev;
};

typedef struct NPC_node_struct NPC_node_t;

typedef
struct {
  NPC_node_t* start;
  NPC_node_t* end;
} NPC_list_t;


void appendToNPC_list(NPC_list_t* list, NPC_node_t* new);

void prependToNPC_list(NPC_list_t* list, NPC_node_t* new);

void removeFromNPC_list(NPC_list_t* list, int ID);

NPC_list_t* createNPC_list();

NPC_node_t* createNPC_node(NPC_t* npc);

void freeNPC_node(NPC_node_t* npcNode);

void freeNPC_list(NPC_list_t* npcList);

void pickDest(NPC_node_t* npcNode);

void moveToDest(NPC_node_t* npcNode);

void pickDestLoop(NPC_list_t* pickList, NPC_list_t* moveList);

void moveDestLoop(NPC_list_t* pickList, NPC_list_t* moveList);

#endif
