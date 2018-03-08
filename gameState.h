#ifndef FILE_GAMESTATE_SEEN
#define FILE_GAMESTATE_SEEN

enum gameStates {
  GAMERUN,
  GAMEPAUSE,
  GAMEMAPEDIT,
  GAMETEXTENTRY,
  GAMEQUIT
};

typedef enum gameStates gameState;

gameState getGameState();

void setGameState(gameState);

#endif
