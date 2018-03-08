#include "gameState.h"
static gameState state;

gameState getGameState() {
  return state;
}

void setGameState(gameState newState) {
  state = newState;
}
