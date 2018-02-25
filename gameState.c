#include "gameState.h"
static enum GameState state;

enum GameState getGameState() {
  return state;
}

void setGameState(enum GameState newState) {
  state = newState;
}
