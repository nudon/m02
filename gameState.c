#include <stdio.h>
#include "gameState.h"

static gameState state;

gameState getGameState() {
  return state;
}

void setGameState(gameState newState) {
  fprintf(stderr, "changing gameState to %d\n", newState);
  state = newState;
}


