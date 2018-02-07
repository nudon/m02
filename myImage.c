#include "myImage.h"
SDL_Surface* loadToSurface(char* path) {
  SDL_Surface* surface = IMG_Load(path);
  if (surface == NULL) {
    fprintf(stderr, "Error in loading picture %s to surface: %s \n",
	    path, 
	    SDL_GetError());
  }
  return surface;  
}

SDL_Texture* loadTextureAlt(char* path, SDL_Renderer* gRan) {
  SDL_Texture* newText = IMG_LoadTexture(gRan, path);
  if (newText == NULL) {
    fprintf(stderr, "Error in converting surface to Texture: %s \n",
	    SDL_GetError());
  }
  return newText;
}

SDL_Texture* loadSurfaceToTexture(SDL_Surface* loadSurf, SDL_Renderer* gRan) {
  SDL_Texture* newText;
  newText = SDL_CreateTextureFromSurface(gRan, loadSurf);
  if (newText == NULL) {
    fprintf(stderr, "Error in converting surface to Texture: %s \n",
	    SDL_GetError());
    newText = NULL;
  }
  return newText;
}

SDL_Texture* loadTexture(char* path, SDL_Renderer* gRan) {
  SDL_Texture* newText;
  SDL_Surface* loadSurf = loadToSurface(path);
  if (loadSurf != NULL) {
    newText = loadSurfaceToTexture(loadSurf, gRan);
  }
  else {
    fprintf(stderr, "Error in loading file %s: %s \n",
	    path,
	    IMG_GetError());
    newText = NULL;
  }
  return newText;
}
