#ifndef FILE_GAMESTATE_SEEN
#define FILE_GAMESTATE_SEEN

enum gameStates {
  GAMERUN,
  GAMEPAUSE,
  GAMEMAPEDIT,
  GAMEQUIT
};

enum subStates {
  MOVENORM,
  MENUNORM,
  MENUTEXTENTRY
};

typedef enum subStates gameSubState;

typedef enum gameStates gameState;

gameState getGameState();

void setGameState(gameState);

gameSubState getGameSubState();

void setGameSubState(gameSubState newState);


#endif
