#ifndef FILE_MYNPC_SEEN
#define FILE_MYNPC_SEEN

#include <SDL2/SDL_image.h>
#include "temp.h"


//structs
typedef
struct {
  //thinking of putting a magic pixel in upper left hand corner of valid sprite
  //handles issue of having jagged arrays of sprite clips in a single sheet
  //would check if the next sprite over has the magic pixel in the corner
  //think you can read straight pixel  values from surface
  //and if the magic pixel isn't there, wrap around to zero
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
  NPC_list_t* idleList;
  NPC_list_t* moveList;
} NPC_move_list;

//NPC stuff

NPC_node_t* getControlledCharacter();

void setControlledCharacter(NPC_node_t* node);
int isControlled(NPC_t* npc);

int isDebuging(NPC_t* npc);

NPC_t* createNPC();

void makeMC(NPC_t* slate);

void makeDebug(NPC_t* slate);

void makeNPC(NPC_t* slate);

void freeNPC(NPC_t* npc);

void setNPCPosition(NPC_t* npc, tile_pos_t* pos);

void setNPCPositionByCord(NPC_t* npc, int x, int y);
 
void setTilePosition(tile_pos_t* npcPos, tile_pos_t* pos);

void setTilePositionByCord(tile_pos_t* npcPos, int x, int y);

void loadSpriteMC(sprite_holder_t* holder);

void alignNPCToPos(NPC_t* npc, tile_pos_t* pos);

//NPC_node stuff
NPC_node_t* createNPC_node(NPC_t* npc);
void freeNPC_node(NPC_node_t* npcNode);

//NPC_list stuff
NPC_move_list* createNPC_move_list();
NPC_list_t* createNPC_list();
void drawAllNPCS(NPC_move_list* list);
void drawNPCList(NPC_list_t* list);
void drawNPC(NPC_t* npc);
void freeNPC_list(NPC_list_t* npcList);
void addNPC(NPC_t* npc); //creates node and adds to idle list
void addNPCToList(NPC_t* npc, NPC_list_t* list);
void changeToMoveList(NPC_move_list* totNPC, NPC_node_t* npcNode);
void changeToIdleList(NPC_move_list* totNPC, NPC_node_t* npcNode);
void printNPCList(NPC_list_t* list);

//utilities and whatnot

void setDestByTile(NPC_node_t* node, tile_pos_t* tile);

void setDestByCord(NPC_node_t* node, int x, int y);

int equalTilePos(tile_pos_t* t1, tile_pos_t* t2);

void pickDest(NPC_move_list* npcList, NPC_node_t* npcNode);

void moveToDest(NPC_move_list* npcList, NPC_node_t* npcNode);

void pickDestLoop(NPC_move_list* npcList);

void moveDestLoop(NPC_move_list* npcList);

int validPos(tile_pos_t* tile);

void positionShift(NPC_node_t* npcNode, int* shiftPos, int shiftAmount);

void updateNPCPos(NPC_t* npc);

#endif
