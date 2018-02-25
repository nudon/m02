#ifndef FILE_GAMESTATE_SEEN
#define FILE_GAMESTATE_SEEN

enum GameState {
  GAMERUN,
  GAMEPAUSE,
  GAMEQUIT
};

enum GameState getGameState();

void setGameState(enum GameState);

#endif
