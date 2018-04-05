#ifndef FILE_GAMESTATE_SEEN
#define FILE_GAMESTATE_SEEN

enum gameStates {
  GAMERUN,
  GAMEMAPEDIT,
  MENU,
  MENUTEXTENTRY,
  GAMEQUIT
};


typedef enum gameStates gameState;

gameState getGameState();

void setGameState(gameState);

#endif
