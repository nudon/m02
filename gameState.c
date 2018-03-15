#include <stdio.h>
#include "gameState.h"

static gameState state;
static gameSubState subState;

gameState getGameState() {
  return state;
}

void setGameState(gameState newState) {
  fprintf(stderr, "changing gameState to %d\n", newState);
  state = newState;
}

gameSubState getGameSubState() {
  return subState;
}

void setGameSubState(gameSubState newState) {
  fprintf(stderr, "changing gameSubState to %d\n", newState);
  subState = newState;
}
