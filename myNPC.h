#ifndef FILE_MYNPC_SEEN
#define FILE_MYNPC_SEEN

#include <SDL2/SDL_image.h>
#include "myMap.h"
//#include "temp.h"


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
} spriteHolder;

typedef
struct {
  spriteHolder* holder;
  int spriteRow;
  int spriteCol;  
} animateState;

typedef
struct {
  int npcID;
  tilePos* tilePos;
  pixPos* pixelPos;
  int speed;
  animateState* animateState;
  uint8_t flags;
} npc;

struct npc_node_struct{
  npc* storedNpc;
  tilePos* dest;
  struct npc_node_struct * next;
  struct npc_node_struct * prev;
};

typedef struct npc_node_struct npcNode;

typedef
struct {
  npcNode* start;
  npcNode* end;
} npcList;


typedef
struct {
  npcList* idleList;
  npcList* moveList;
} npcSet;

//NPC stuff

//npcNode* getControlledCharacter();

void setControlledCharacter(npcNode* node);
int isControlled(npc* npc);

int isDebuging(npc* npc);

npc* createNpc();

void makeMC(npc* slate);

void makeDebug(npc* slate);

void makeNpc(npc* slate);

void freeNpc(npc* npc);

void setNpcPosition(npc* npc, tilePos* pos);

void setNpcPositionByCord(npc* npc, int x, int y);
 
void setTilePosition(tilePos* npcPos, tilePos* pos);

void setTilePositionByCord(tilePos* npcPos, int x, int y);

void loadSpriteMC(spriteHolder* holder);

void alignNpcToPos(npc* npc, tilePos* pos);

//NPC_node stuff
npcNode* createNpcNode(npc* npc);
void freeNpcNode(npcNode* npcNode);

//NPC_list stuff
npcSet* createNpcSet();
npcList* createNpcList();
void drawAllNpcs(npcSet* list);
void drawNpcList(npcList* list);
void drawNpc(npc* npc);
void freeNpcList(npcList* npcList);
void freeNpcSet(npcSet* set);
void addNpc(npc* npc); //creates node and adds to idle list
void addNpcToList(npc* npc, npcList* list);
void changeToMoveList(npcSet* totNpc, npcNode* npcNode);
void changeToIdleList(npcSet* totNpc, npcNode* npcNode);
void printNpcList(npcList* list);

//utilities and whatnot



void allignPixPosToTilePos(pixPos* pixPos, tilePos* tilePos);
void allignPixPosToTilePosByCord(pixPos* pixPos, int x, int y);
void setPixPos(pixPos* pixPosDest, pixPos* pixPosSrc);
void setPixPosByCord(pixPos* pixPos, int x, int y);
void setTilePos(tilePos* dest, tilePos* src);
void setTilePosByCord(tilePos* dest, int x, int y);

int equalTilePos(tilePos* t1, tilePos* t2);

void pickDest(npcSet* npcList, npcNode* npcNode);

void moveToDest(npcSet* npcList, npcNode* npcNode);

void pickDestLoop(npcSet* npcList);

void moveDestLoop(npcSet* npcList);

int validPos(tilePos* tile);

void positionShift(npcNode* npcNode, int* shiftPos, int shiftAmount);

void updateNpcPos(npc* npc);

SDL_Rect* setDestrectForDrawingSomethingFromTilePos(SDL_Rect* dest, tilePos* tPos);

SDL_Rect* setDestrectForDrawingSomethingFromPixelPos(SDL_Rect* dest, pixPos* pPos);

#endif
