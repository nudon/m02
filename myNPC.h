#ifndef FILE_MYNPC_SEEN
#define FILE_MYNPC_SEEN
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "temp.h"
#include "myImage.h"

typedef
struct {
  //thinking of putting a magic pixel in upper left hand corner of valid sprite
  //handles issue of having jagged arrays of sprite clips in a single sheet
  //would call some generic function to advance a frame of animation
  //would load the clip
  //would check if the next sprite over has the magic pixel in the corner
  //think you can read straight pixel  values from surface
  //and if the magic pixel isn't there, wrap around to zero
  //alternatively keep a list of the number of cols indexed by rows
  int sprite_width;
  int sprite_height;
  int rows;
  int cols;
  SDL_Texture* sprite_sheet;  
} sprite_holder_t;

typedef
struct {
  sprite_holder_t* holder;
  int spriteRow;
  int spriteCol;  
} anim_state;

typedef
struct {
  int pixPosX;
  int pixPosY;
} npc_pos_t;

typedef
struct {
  int npcID;
  tile_pos_t* position;
  npc_pos_t* pixelPos;
  int speed;
  anim_state* animState;
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

typedef
struct {
  NPC_list_t* idleNPC;
  NPC_list_t* moveNPC;
} NPC_move_list;

NPC_t* createNPC();

void freeNPC(NPC_t* npc);

void makeMC(NPC_t* slate);

void loadSpriteMC(sprite_holder_t* holder);

void appendToNPC_list(NPC_list_t* list, NPC_node_t* new);

void prependToNPC_list(NPC_list_t* list, NPC_node_t* new);

void removeFromNPC_list(NPC_list_t* list, NPC_t* ID);

NPC_move_list* createNPC_move_list();

NPC_list_t* createNPC_list();

NPC_node_t* createNPC_node(NPC_t* npc);

void freeNPC_node(NPC_node_t* npcNode);

void freeNPC_list(NPC_list_t* npcList);

int equalTilePos(tile_pos_t* t1, tile_pos_t* t2);

void pickDest(NPC_move_list* npcList, NPC_node_t* npcNode);

void moveToDest(NPC_move_list* npcList, NPC_node_t* npcNode);

void pickDestLoop(NPC_move_list* npcList);

void moveDestLoop(NPC_move_list* npcList);

#include "myMap.h"

#endif
